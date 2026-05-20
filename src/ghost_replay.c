#include <common.h>
#include <deckard_strings.h>
#include <spyro.h>
#include <prims.h>
#include <font.h>
#include <custom_menu.h>
#include <ghost_replay_api.h>
#include <ghosts_autoplay.h>
#include <ghost_debug.h>
#include <deckard.h>

extern int savestate_selection;

// GHOST REPLAY SYSTEM
// Memory Layout (Deckard BUILD==PS2_DECKARD, ends before PS2M at 0x80A70000):
//   Total span: 0x80A29000 - 0x80A6FFDF = 0x46FE0, split equally:
//   PS2: DeckardGhostBootRequest (8 ints) @ 0x80A28FE0 (see deckard_ghost_boot.h)
//   Ghost A (Playback): 0x80A29000 - 0x80A4C7EF
//   Ghost B (Recording): 0x80A4C7F0 - 0x80A6FFDF
//   Each: GHOST_REGION_SIZE = 0x237F0 (~142 KB)
//
// Each ghost region:
//   - Header (V1: 24 bytes, V2: 168 bytes with dragon pause events)
//   - Frame data: (GHOST_REGION_SIZE-24) / 24 bytes per frame; display time at 30 FPS

#define GHOST_V2_MAGIC          0x47484432 /* 'GHD2' */
#define GHOST_V2_VERSION        2
#define GHOST_MAX_DRAGON_EVENTS 16

typedef struct GhostDragonEvent
{
    int frameIndex;
    int durationFrames;
} GhostDragonEvent;

// Stored ONCE at the start of each ghost's data region
typedef struct GhostHeader
{
    Vec3 initialPosition;    // 12 bytes - starting position for delta reconstruction
    int levelId;             // 4 bytes  - which level this ghost was recorded on
    int frameCount;          // 4 bytes  - total frames in this recording
    int finalTimeFrames;     // 4 bytes  - final time (for display purposes)

    int magic;                // V2 only: GHOST_V2_MAGIC
    int version;              // V2 only: GHOST_V2_VERSION
    int headerSize;           // V2 only: sizeof(GhostHeader)
    int dragonEventCount;     // V2 only: number of valid dragonEvents
    GhostDragonEvent dragonEvents[GHOST_MAX_DRAGON_EVENTS];
} GhostHeader; // V1 logical prefix: 24 bytes; V2 full header: 168 bytes

// Stored PER FRAME
typedef struct ReplayFrameData
{
    // Position deltas (change from previous frame)
    short deltaX;                      // 2 bytes
    short deltaY;                      // 2 bytes
    short deltaZ;                      // 2 bytes

    // Body animation
    unsigned char currentAnim;         // 1 byte
    unsigned char nextAnim;            // 1 byte
    unsigned char currentKeyframe;     // 1 byte
    unsigned char nextKeyframe;        // 1 byte
    unsigned char animSpeed;           // 1 byte

    // Head animation
    unsigned char currentHeadAnim;     // 1 byte
    unsigned char nextHeadAnim;        // 1 byte
    unsigned char currentHeadKeyframe; // 1 byte
    unsigned char nextHeadKeyframe;    // 1 byte
    unsigned char headAnimSpeed;       // 1 byte

    unsigned char padding[4];       // preserve 24-byte frame layout

    // Angle (not only sparx related it seems, its important)
    AngleU8 visualAngle; // 4 bytes
} ReplayFrameData; // 24 bytes

typedef struct GhostPosSnapshot
{
    int   playbackFrame;
    Vec3  playbackPos;
    int   playbackDragonEventIndex;
    int   playbackDragonPauseRemaining;
    int   recordFrame;
    Vec3  recordPrevPos;
    int   recordingDragonEventCount;
    int   recordingDragonEventIndex;
    int   recordingDragonPauseFrames;
    bool  recordingWasInDragon;
} GhostPosSnapshot;

GhostBufferState g_ghostA = { 0 };
GhostBufferState g_ghostB = { 0 };

bool g_ghost_new_best = false;
int g_dbg_ghost_update_calls = 0;
int g_dbg_ghost_update_dragon_skips = 0;
int g_dbg_ghost_record_calls = 0;
int g_dbg_ghost_start_calls = 0;
int g_dbg_ghost_start_insta_calls = 0;
int g_dbg_ghost_start_reg_calls = 0;

GhostPosSnapshot g_ghost_pos_snapshots[3] = { 0 };

char g_ghost_rec_time_line[40];

static int s_playback_dragon_event_index = 0;
static int s_playback_dragon_pause_remaining = 0;
static bool s_recording_was_in_dragon = false;
static int s_recording_dragon_event_index = -1;
static int s_recording_dragon_pause_frames = 0;
static bool s_ghost_render_frame_ready = false;
static bool s_recording_failed_too_long = false;
static int s_recording_too_long_message_timer = 0;

#define GHOST_REC_TOO_LONG_MESSAGE_FRAMES 180

// -----------------------------------------------------------------------------
// Memory Region Definitions
// -----------------------------------------------------------------------------

// EMULATOR MEMORY REGIONS
#if BUILD == REDUX || BUILD == DUCKSTATION
#define TEMP_SPYRO_REGION   0x80500000
#define GHOST_REGION_A      0x80600000
#define GHOST_REGION_B      0x80658000  // Halfway point
#define GHOST_REGION_SIZE   0x58000    
// For console
#elif BUILD == PS2_DECKARD
#define TEMP_SPYRO_REGION   0x80A28000
#define GHOST_REGION_A      0x80A29000  // Playback ghost
#define GHOST_REGION_B      0x80A4C7F0  // Recording; ends before 0x80A70000 (PS2M)
#define GHOST_REGION_SIZE   0x237F0     // half of 0x80A6FFE0-0x80A29000 (one buffer each)
#endif

// Calculate max frames per ghost buffer
// (Region size - Header size) / Frame size
#define GHOST_LEGACY_HEADER_SIZE 24
#define GHOST_HEADER_SIZE   sizeof(GhostHeader)
#define GHOST_FRAME_SIZE    sizeof(ReplayFrameData)
#define MAX_FRAMES_PER_GHOST ((GHOST_REGION_SIZE - GHOST_HEADER_SIZE) / GHOST_FRAME_SIZE)

/* Recording HUD: max time for ghost B only (same byte budget as one GHOST_REGION_SIZE).
   REDUX/emu uses 0x58000 per buffer (~8:17 at 30 fps); Deckard uses 0x237F0 (~3:22). */
#define GHOST_RECORDING_HUD_MAX_FRAMES ((int)MAX_FRAMES_PER_GHOST)

/* Ghost frame count ticks once per game frame; display as 30 fps wall time (see IL). */
#define GHOST_REC_TIME_FPS 30
#define GHOST_REC_SPACE_WARN_SEC 10
#define GHOST_REC_SPACE_WARN_FRAMES (GHOST_REC_SPACE_WARN_SEC * GHOST_REC_TIME_FPS)

#define GET_GHOST_HEADER(region) ((GhostHeader*)(region))


// Deckard region helpers: sub-32-bit accesses in 0x80Axxxxx are half-word swapped
#if BUILD == PS2_DECKARD
#define DECK_READ_U8(addr)          deckard_read_u8((u8*)(addr))
#define DECK_READ_SHORT(addr)       ((short)deckard_read_u16((u32*)(addr)))
#define DECK_READ_INT(addr)         (*(int*)(addr))
#define DECK_WRITE_U8(addr, val)    deckard_write_u8((u8*)(addr), (val))
#define DECK_WRITE_SHORT(addr, val) deckard_write_u16((u16*)(addr), (u16)(val))
#define DECK_WRITE_INT(addr, val)   (*(int*)(addr) = (val))
#define DECK_MEMCPY(d, s, n)        deckard_memcpy((d), (s), (n))
#else
#define DECK_READ_U8(addr)          (*(u8*)(addr))
#define DECK_READ_SHORT(addr)       (*(short*)(addr))
#define DECK_READ_INT(addr)         (*(int*)(addr))
#define DECK_WRITE_U8(addr, val)    (*(u8*)(addr) = (val))
#define DECK_WRITE_SHORT(addr, val) (*(short*)(addr) = (val))
#define DECK_WRITE_INT(addr, val)   (*(int*)(addr) = (val))
#define DECK_MEMCPY(d, s, n)        memcpy((d), (s), (n))
#endif

extern GhostMenu ghost_menu;

static void GetGhostTint(u8RGBA* tint)
{
    tint->R = 0x66;
    tint->G = 0x00;
    tint->B = 0x00;
    tint->opacity = 0x90;

    switch (ghost_menu.ghosts_color)
    {
        case GHOST_COLOR_BLUE:
        tint->R = 0x00;
        tint->G = 0x30;
        tint->B = 0x88;
        break;
        case GHOST_COLOR_GREEN:
        tint->R = 0x00;
        tint->G = 0x80;
        tint->B = 0x20;
        break;
        case GHOST_COLOR_PURPLE:
        tint->R = 0x66;
        tint->G = 0x20;
        tint->B = 0x88;
        break;
        case GHOST_COLOR_GOLD:
        tint->R = 0x88;
        tint->G = 0x60;
        tint->B = 0x00;
        break;
        case GHOST_COLOR_BLACK:
        tint->R = 0x00;
        tint->G = 0x00;
        tint->B = 0x00;
        break;
        case GHOST_COLOR_RED:
        default:
        break;
    }
}

/* GhostHeader lives in Deckard RAM (0x80A29xxx / 0x80A44xxx). Full-word int
 * storage there can appear byte-swapped vs normal EE RAM; plain lw/sw then
 * breaks PB checks (finalTimeFrames reads as 0 -> "new best" every run).
 * Sub-frame fields still use DECK_READ/WRITE_U8/SHORT. */
#if BUILD == PS2_DECKARD
static inline unsigned GhostHdrWordSwap32(unsigned w)
{
    return (w << 24) | ((w << 8) & 0xFF0000u) | ((w >> 8) & 0xFF00u) | (w >> 24);
}

static inline int GhostHdrLoadI32(const int* p)
{
    unsigned u = *(const unsigned*)p;
    return (int)GhostHdrWordSwap32(u);
}

static inline void GhostHdrStoreI32(int* p, int v)
{
    *(unsigned*)p = GhostHdrWordSwap32((unsigned)v);
}
#else
#define GhostHdrLoadI32(p)          (*(p))
#define GhostHdrStoreI32(p, v)      (*(p) = (v))
#endif

static void DrawGhostDiamond(Spyro* temp_spyro);

extern ILMenu il_menu;
extern bool has_started_insta_fly_in;
extern bool has_started_reg_fly_in;


static bool GhostHeaderHasCompletedRun(GhostHeader* header)
{
    return GhostHdrLoadI32(&header->frameCount) > 0 &&
        GhostHdrLoadI32(&header->finalTimeFrames) > 0;
}

static bool GhostHeaderIsV2(GhostHeader* header)
{
    return GhostHdrLoadI32(&header->magic) == GHOST_V2_MAGIC &&
        GhostHdrLoadI32(&header->version) == GHOST_V2_VERSION &&
        GhostHdrLoadI32(&header->headerSize) == GHOST_HEADER_SIZE;
}

static int GhostHeaderFrameDataOffset(GhostHeader* header)
{
    if (GhostHeaderIsV2(header))
        return GhostHdrLoadI32(&header->headerSize);
    return GHOST_LEGACY_HEADER_SIZE;
}

static ReplayFrameData* GhostFramesForRegion(unsigned int region)
{
    GhostHeader* header = GET_GHOST_HEADER(region);
    return (ReplayFrameData*)(region + GhostHeaderFrameDataOffset(header));
}

static int GhostDragonEventCount(GhostHeader* header)
{
    int count;
    if (!GhostHeaderIsV2(header))
        return 0;

    count = GhostHdrLoadI32(&header->dragonEventCount);
    if (count < 0)
        return 0;
    if (count > GHOST_MAX_DRAGON_EVENTS)
        return GHOST_MAX_DRAGON_EVENTS;
    return count;
}

static void ResetGhostDragonRuntime(void)
{
    s_playback_dragon_event_index = 0;
    s_playback_dragon_pause_remaining = 0;
    s_recording_was_in_dragon = false;
    s_recording_dragon_event_index = -1;
    s_recording_dragon_pause_frames = 0;
    s_ghost_render_frame_ready = false;
}

static bool GhostHeaderMatchesCompletedLevel(GhostHeader* header, int levelId)
{
    return GhostHeaderHasCompletedRun(header) &&
        GhostHdrLoadI32(&header->levelId) == levelId;
}

// Start recording a new ghost into a buffer
static void StartRecording(GhostBufferState* state, unsigned int region)
{
    state->isActive = true;
    state->isRecording = true;
    state->currentFrame = 0;
    state->previousPosition = _spyro.position;
    s_recording_failed_too_long = false;
    s_recording_too_long_message_timer = 0;
    s_recording_was_in_dragon = false;
    s_recording_dragon_event_index = -1;
    s_recording_dragon_pause_frames = 0;

    // Initialize header
    GhostHeader* header = GET_GHOST_HEADER(region);
    GhostHdrStoreI32(&header->initialPosition.x, _spyro.position.x);
    GhostHdrStoreI32(&header->initialPosition.y, _spyro.position.y);
    GhostHdrStoreI32(&header->initialPosition.z, _spyro.position.z);
    GhostHdrStoreI32(&header->levelId, _levelID);
    GhostHdrStoreI32(&header->frameCount, 0);
    GhostHdrStoreI32(&header->finalTimeFrames, 0);
    GhostHdrStoreI32(&header->magic, GHOST_V2_MAGIC);
    GhostHdrStoreI32(&header->version, GHOST_V2_VERSION);
    GhostHdrStoreI32(&header->headerSize, GHOST_HEADER_SIZE);
    GhostHdrStoreI32(&header->dragonEventCount, 0);
    {
        int i;
        for (i = 0; i < GHOST_MAX_DRAGON_EVENTS; i++)
        {
            GhostHdrStoreI32(&header->dragonEvents[i].frameIndex, 0);
            GhostHdrStoreI32(&header->dragonEvents[i].durationFrames, 0);
        }
    }
}

// Start playing back a ghost from a buffer
static void StartPlayback(GhostBufferState* state, unsigned int region)
{
    GhostHeader* header = GET_GHOST_HEADER(region);

    // Only finished recordings are valid for playback.
    if (GhostHeaderHasCompletedRun(header))
    {
        state->isActive = true;
        state->isRecording = false;
        state->currentFrame = 0;
        state->currentPosition.x = GhostHdrLoadI32(&header->initialPosition.x);
        state->currentPosition.y = GhostHdrLoadI32(&header->initialPosition.y);
        state->currentPosition.z = GhostHdrLoadI32(&header->initialPosition.z);
        s_playback_dragon_event_index = 0;
        s_playback_dragon_pause_remaining = 0;
        s_ghost_render_frame_ready = false;
    }
}

// Stop a ghost buffer
static void StopGhost(GhostBufferState* state)
{
    state->isActive = false;
    state->isRecording = false;
    state->currentFrame = 0;
    if (state == &g_ghostA)
    {
        s_playback_dragon_event_index = 0;
        s_playback_dragon_pause_remaining = 0;
        s_ghost_render_frame_ready = false;
    }
    else if (state == &g_ghostB)
    {
        s_recording_was_in_dragon = false;
        s_recording_dragon_event_index = -1;
        s_recording_dragon_pause_frames = 0;
    }
}

// Copy ghost data from one region to another (for promoting recording to playback)
static void CopyGhostData(unsigned int srcRegion, unsigned int dstRegion)
{
    GhostHeader* srcHeader = GET_GHOST_HEADER(srcRegion);
    GhostHeader* dstHeader = GET_GHOST_HEADER(dstRegion);
    int srcHeaderSize = GhostHeaderFrameDataOffset(srcHeader);
    int dstHeaderSize;

    if (srcHeaderSize < GHOST_LEGACY_HEADER_SIZE || srcHeaderSize > GHOST_HEADER_SIZE)
        srcHeaderSize = GHOST_LEGACY_HEADER_SIZE;
    dstHeaderSize = srcHeaderSize;

    // Copy header as logical int32 values, preserving Deckard header storage rules.
    {
        int i;
        int* s = (int*)srcHeader;
        int* d = (int*)dstHeader;
        for (i = 0; i < dstHeaderSize / 4; i++)
            GhostHdrStoreI32(&d[i], GhostHdrLoadI32(&s[i]));
        for (; i < GHOST_HEADER_SIZE / 4; i++)
            GhostHdrStoreI32(&d[i], 0);
    }

    // Copy frame data
    ReplayFrameData* srcFrames = GhostFramesForRegion(srcRegion);
    ReplayFrameData* dstFrames = GhostFramesForRegion(dstRegion);

    int bytesToCopy = GhostHdrLoadI32(&srcHeader->frameCount) * GHOST_FRAME_SIZE;
    DECK_MEMCPY(dstFrames, srcFrames, bytesToCopy);
}

static void ClearGhostHeader(unsigned int region)
{
    GhostHeader* header = GET_GHOST_HEADER(region);

    GhostHdrStoreI32(&header->initialPosition.x, 0);
    GhostHdrStoreI32(&header->initialPosition.y, 0);
    GhostHdrStoreI32(&header->initialPosition.z, 0);
    GhostHdrStoreI32(&header->levelId, 0);
    GhostHdrStoreI32(&header->frameCount, 0);
    GhostHdrStoreI32(&header->finalTimeFrames, 0);
    GhostHdrStoreI32(&header->magic, 0);
    GhostHdrStoreI32(&header->version, 0);
    GhostHdrStoreI32(&header->headerSize, 0);
    GhostHdrStoreI32(&header->dragonEventCount, 0);
    {
        int i;
        for (i = 0; i < GHOST_MAX_DRAGON_EVENTS; i++)
        {
            GhostHdrStoreI32(&header->dragonEvents[i].frameIndex, 0);
            GhostHdrStoreI32(&header->dragonEvents[i].durationFrames, 0);
        }
    }
}

// Record a single frame
static void RecordFrame(GhostBufferState* state, unsigned int region)
{
    g_dbg_ghost_record_calls++;

    if (state->currentFrame >= MAX_FRAMES_PER_GHOST)
    {
        // Buffer full: keep the IL valid, but mark this ghost attempt unsavable.
        state->isActive = false;
        state->isRecording = false;
        s_recording_failed_too_long = true;
        s_recording_too_long_message_timer = GHOST_REC_TOO_LONG_MESSAGE_FRAMES;
        return;
    }

    GhostHeader* header = GET_GHOST_HEADER(region);
    ReplayFrameData* frames = GhostFramesForRegion(region);
    ReplayFrameData* frame = &frames[state->currentFrame];

    // Calculate position deltas
    int deltaX = _spyro.position.x - state->previousPosition.x;
    int deltaY = _spyro.position.y - state->previousPosition.y;
    int deltaZ = _spyro.position.z - state->previousPosition.z;

    // Clamp deltas to short range
    DECK_WRITE_SHORT(&frame->deltaX, (short)deltaX);
    DECK_WRITE_SHORT(&frame->deltaY, (short)deltaY);
    DECK_WRITE_SHORT(&frame->deltaZ, (short)deltaZ);

    // Store animation data
    // This field behaved best on Deckard when treated as one 32-bit chunk
    // (same approach the old backup implementation effectively used).
    DECK_MEMCPY(&frame->visualAngle, &_spyro.visualAngle, sizeof(AngleU8));
    DECK_WRITE_U8(&frame->currentAnim, _spyro.currentAnim);
    DECK_WRITE_U8(&frame->nextAnim, _spyro.nextAnim);
    DECK_WRITE_U8(&frame->currentKeyframe, _spyro.currentKeyfame);
    DECK_WRITE_U8(&frame->nextKeyframe, _spyro.nextKeyframe);
    DECK_WRITE_U8(&frame->animSpeed, _spyro.animSpeed);
    DECK_WRITE_U8(&frame->currentHeadAnim, _spyro.currentHeadAnim);
    DECK_WRITE_U8(&frame->nextHeadAnim, _spyro.nextHeadAnim);
    DECK_WRITE_U8(&frame->currentHeadKeyframe, _spyro.currentHeadKeyframe);
    DECK_WRITE_U8(&frame->nextHeadKeyframe, _spyro.nextHeadKeyframe);
    DECK_WRITE_U8(&frame->headAnimSpeed, _spyro.headAnimSpeed);
    DECK_WRITE_U8(&frame->padding[0], 0);
    DECK_WRITE_U8(&frame->padding[1], 0);
    DECK_WRITE_U8(&frame->padding[2], 0);
    DECK_WRITE_U8(&frame->padding[3], 0);

    // Update state for next frame
    state->previousPosition = _spyro.position;
    state->currentFrame++;
    GhostHdrStoreI32(&header->frameCount, state->currentFrame);
}

// Play back a single frame to temp_spyro
static void PlaybackFrame(GhostBufferState* state, unsigned int region, Spyro* temp_spyro)
{
    GhostHeader* header = GET_GHOST_HEADER(region);

    if (state->currentFrame >= GhostHdrLoadI32(&header->frameCount))
    {
        // ghost just stops at final position right now with testing.
        return;
    }

    ReplayFrameData* frames = GhostFramesForRegion(region);
    ReplayFrameData* frame = &frames[state->currentFrame];

    // Reconstruct position from delta (shorts need deckard read)
    state->currentPosition.x += DECK_READ_SHORT(&frame->deltaX);
    state->currentPosition.y += DECK_READ_SHORT(&frame->deltaY);
    state->currentPosition.z += DECK_READ_SHORT(&frame->deltaZ);

    // Apply to temp spyro — direct writes here because DrawSpyro reads
    // through the same hardware swap, so normal writes cancel out correctly
    temp_spyro->position = state->currentPosition;
    DECK_MEMCPY(&temp_spyro->visualAngle, &frame->visualAngle, sizeof(AngleU8));
    temp_spyro->currentAnim = DECK_READ_U8(&frame->currentAnim);
    temp_spyro->nextAnim = DECK_READ_U8(&frame->nextAnim);
    temp_spyro->currentKeyfame = DECK_READ_U8(&frame->currentKeyframe);
    temp_spyro->nextKeyframe = DECK_READ_U8(&frame->nextKeyframe);
    temp_spyro->animSpeed = DECK_READ_U8(&frame->animSpeed);
    temp_spyro->currentHeadAnim = DECK_READ_U8(&frame->currentHeadAnim);
    temp_spyro->nextHeadAnim = DECK_READ_U8(&frame->nextHeadAnim);
    temp_spyro->currentHeadKeyframe = DECK_READ_U8(&frame->currentHeadKeyframe);
    temp_spyro->nextHeadKeyframe = DECK_READ_U8(&frame->nextHeadKeyframe);
    temp_spyro->headAnimSpeed = DECK_READ_U8(&frame->headAnimSpeed);

    // Force non-interpolated rendering for the ghost — these three fields
    // otherwise leak from live _spyro via GhostButtonCheck's memcpy, and
    // combining live's interp factor with ghost's recorded keyframe indices
    // causes vertex accumulator reads past valid bounds -> signed overflow
    // at 0x800246ec (`add s1,s1,s2`) in DrawSpyro.
    temp_spyro->maybe_AnimSpeedRelated = 0;     // offset 0x24 (body)
    temp_spyro->maybe_HeadAnimSpeedRelated = 0; // offset 0x25 (head)
    temp_spyro->maybe_TailAnimSpeedRelated = 0; // offset 0x26 (tail)

    // --- Anim blacklist ---
    // The per-level anim descriptor table at *(0x80076378)+0x38 has NULL
    // entries for anims whose pose data isn't resident in the current level
    // (SUPERCHARGE, SUPERFLY, ROLL_DOWN_SLOPE, PULL_BACK_GLIDE, CANNON_ROTATE,
    //  HURT_YAW, IDLE_SIT, several others — varies by level).
    // If the ghost tries to play one, DrawSpyro does `lw gp, 0x38(gp)` → 0,
    // then `lw a2, 0x10(gp)` reads pose data from ~0x10 (exception vectors
    // via KSEG0 wrap). The inner loop walks that garbage and eventually
    // `add s1, s2` overflows (seen at 0x800246ec and 0x80024510).
    //
    // For now, blacklist only the anim we've actually caught crashing.
    // Fall back to IDLE (anim 0) which is guaranteed resident everywhere.
    // Extend the switch below as more offenders surface.
    {
        u8 bodyAnim = DECK_READ_U8(&frame->currentAnim);
        u8 bodyNext = DECK_READ_U8(&frame->nextAnim);
        u8 headAnim = DECK_READ_U8(&frame->currentHeadAnim);
        u8 headNext = DECK_READ_U8(&frame->nextHeadAnim);

        bool unsafe =
            (bodyAnim == SUPERCHARGE) || (bodyNext == SUPERCHARGE) ||
            (headAnim == SUPERCHARGE) || (headNext == SUPERCHARGE) ||
            (bodyAnim == SUPERCHARGE + 1) || (bodyNext == SUPERCHARGE + 1) ||
            (headAnim == SUPERCHARGE + 1) || (headNext == SUPERCHARGE + 1);

        if (unsafe)
        {
            // Safe fallback: IDLE, keyframe 0. Tail is not recorded, so it
            // already holds live's value from GhostButtonCheck's memcpy and
            // is always valid.
            temp_spyro->currentAnim = CHARGE;
            temp_spyro->nextAnim = CHARGE;
            temp_spyro->currentKeyfame = 0;
            temp_spyro->nextKeyframe = 0;
            temp_spyro->currentHeadAnim = CHARGE;
            temp_spyro->nextHeadAnim = CHARGE;
            temp_spyro->currentHeadKeyframe = 0;
            temp_spyro->nextHeadKeyframe = 0;
        }
    }

    state->currentFrame++;
}

static void UpdateRecordingDragonEvent(unsigned int region)
{
    GhostHeader* header = GET_GHOST_HEADER(region);
    int count;

    if (!GhostHeaderIsV2(header))
        return;

    if (!s_recording_was_in_dragon)
    {
        count = GhostDragonEventCount(header);
        if (count >= GHOST_MAX_DRAGON_EVENTS)
        {
            s_recording_dragon_event_index = -1;
        }
        else
        {
            s_recording_dragon_event_index = count;
            s_recording_dragon_pause_frames = 0;
            GhostHdrStoreI32(&header->dragonEvents[count].frameIndex, g_ghostB.currentFrame);
            GhostHdrStoreI32(&header->dragonEvents[count].durationFrames, 0);
            GhostHdrStoreI32(&header->dragonEventCount, count + 1);
        }
        s_recording_was_in_dragon = true;
    }

    if (s_recording_dragon_event_index >= 0 &&
        s_recording_dragon_event_index < GHOST_MAX_DRAGON_EVENTS)
    {
        s_recording_dragon_pause_frames++;
        GhostHdrStoreI32(
            &header->dragonEvents[s_recording_dragon_event_index].durationFrames,
            s_recording_dragon_pause_frames);
    }
}

static void FinishRecordingDragonEvent(void)
{
    s_recording_was_in_dragon = false;
    s_recording_dragon_event_index = -1;
    s_recording_dragon_pause_frames = 0;
}

static bool GhostPlaybackDragonPauseActive(unsigned int region)
{
    GhostHeader* header = GET_GHOST_HEADER(region);
    int count = GhostDragonEventCount(header);

    if (count <= 0)
        return false;

    while (s_playback_dragon_event_index < count)
    {
        int eventFrame = GhostHdrLoadI32(
            &header->dragonEvents[s_playback_dragon_event_index].frameIndex);
        int durationFrames = GhostHdrLoadI32(
            &header->dragonEvents[s_playback_dragon_event_index].durationFrames);

        if (eventFrame > g_ghostA.currentFrame)
            break;

        s_playback_dragon_event_index++;
        if (eventFrame == g_ghostA.currentFrame && durationFrames > 0)
        {
            s_playback_dragon_pause_remaining = durationFrames;
            break;
        }
    }

    if (s_playback_dragon_pause_remaining > 0)
    {
        s_playback_dragon_pause_remaining--;
        return true;
    }

    return false;
}

// Shared fly-in logic: start ghost session for the current level.
// is_insta: true when triggered by the L1+R1+Triangle instaload hotkey
//           (guaranteed to be entering a sublevel).
//           false for a regular portal fly-in. We still want to record a first
//           attempt for a brand-new sublevel even if ghost A belongs to some
//           other level, but we avoid starting a throwaway recording when the
//           fly-in is just returning to a homeworld.
static void StartGhostOnFlyIn(bool is_insta)
{
    g_dbg_ghost_start_calls++;
    if (is_insta)
        g_dbg_ghost_start_insta_calls++;
    else
        g_dbg_ghost_start_reg_calls++;

    GhostHeader* headerA = GET_GHOST_HEADER(GHOST_REGION_A);
    GhostHeader* headerB = GET_GHOST_HEADER(GHOST_REGION_B);
    StopGhost(&g_ghostA);
    StopGhost(&g_ghostB);

    bool sameLevel = GhostHeaderMatchesCompletedLevel(headerA, _levelID);
    bool enteringRecordableLevel = is_insta || ((_levelID % 10) != 0);

    if (sameLevel)
    {
        // Race against the existing best ghost
        StartPlayback(&g_ghostA, GHOST_REGION_A);
        StartRecording(&g_ghostB, GHOST_REGION_B);
    }
    else if (enteringRecordableLevel)
    {
        ClearGhostHeader(GHOST_REGION_B);
        // Record attempts into B; A is only overwritten after completion.
        StartRecording(&g_ghostB, GHOST_REGION_B);
    }
    // Regular fly-in back to a homeworld: leave ghost A untouched and don't
    // start recording. This preserves the previous sublevel ghost unless a new
    // level run actually gets completed later.
}

// API
void GhostButtonCheck(void)
{
    if (!ghost_menu.ghosts_enabled)
        return;

    // Copy Spyro data to temp region (plain memcpy — DrawSpyro reads through
    // the same hardware swap, so normal writes cancel out correctly)
    memcpy((void*)TEMP_SPYRO_REGION, &_spyro, sizeof(_spyro));

    if (has_started_insta_fly_in && il_menu.il_state)
        StartGhostOnFlyIn(true);
    else if (has_started_reg_fly_in && il_menu.il_state)
        StartGhostOnFlyIn(false);
}

void GhostFrameTick(void)
{
    bool liveInDragon;
    Spyro* temp_spyro;

    g_dbg_ghost_update_calls++;
    s_ghost_render_frame_ready = false;

    if (!ghost_menu.ghosts_enabled)
        return;

    liveInDragon = (_gameState == GAMESTATE_DRAGON_STATE);
    temp_spyro = (Spyro*)TEMP_SPYRO_REGION;

    if (liveInDragon)
        g_dbg_ghost_update_dragon_skips++;

    // Handle recording for ghost B (racing mode - new attempt)
    if (g_ghostB.isActive && g_ghostB.isRecording)
    {
        if (liveInDragon)
            UpdateRecordingDragonEvent(GHOST_REGION_B);
        else
        {
            FinishRecordingDragonEvent();
            RecordFrame(&g_ghostB, GHOST_REGION_B);
        }
    }

    // Handle playback for ghost A. This runs from MainHook so dragon-state
    // waits are measured even when the normal world render hook is not hit.
    if (g_ghostA.isActive && !g_ghostA.isRecording)
    {
        GhostHeader* headerA = GET_GHOST_HEADER(GHOST_REGION_A);

        if (!(liveInDragon && !GhostHeaderIsV2(headerA)) &&
            !GhostPlaybackDragonPauseActive(GHOST_REGION_A) &&
            g_ghostA.currentFrame < GhostHdrLoadI32(&headerA->frameCount))
        {
            PlaybackFrame(&g_ghostA, GHOST_REGION_A, temp_spyro);
            s_ghost_render_frame_ready = true;
        }
    }
}

// Call this when the player finishes a run
void GhostOnRunComplete(int finalTimeFrames)
{
    if (s_recording_failed_too_long)
    {
        StopGhost(&g_ghostB);
        s_recording_failed_too_long = false;
        s_recording_too_long_message_timer = GHOST_REC_TOO_LONG_MESSAGE_FRAMES;
        return;
    }

    // If we were recording in B (racing mode), check if it's a PB
    if (g_ghostB.isActive && g_ghostB.isRecording)
    {
        GhostHeader* headerA = GET_GHOST_HEADER(GHOST_REGION_A);
        GhostHeader* headerB = GET_GHOST_HEADER(GHOST_REGION_B);
        bool existingPbIsForCurrentLevel = GhostHeaderMatchesCompletedLevel(
            headerA, GhostHdrLoadI32(&headerB->levelId));

        GhostHdrStoreI32(&headerB->finalTimeFrames, finalTimeFrames);

        // If this is a new PB (or first completion for this level), promote B to A.
        if (!existingPbIsForCurrentLevel ||
            finalTimeFrames < GhostHdrLoadI32(&headerA->finalTimeFrames))
        {
            CopyGhostData(GHOST_REGION_B, GHOST_REGION_A);
            g_ghost_new_best = true;
        }

        StopGhost(&g_ghostB);
    }
}

// Redirect every hardcoded live-spyro reference inside DrawSpyro to the
// temp_spyro region so the function can be reused to render the ghost.
//
// Four base-load sites need patching:
//   0x80023afc/b00: lui $ra,0x8008 ; addiu $ra,$ra,-0x75A8
//                   (primary: lw $X, 0x0/0x4/0x8($ra) -> position)
//   0x80024424/28 : lui $t9,0x8008 ; addiu $t9,$t9,-0x75A8
//                   (then lw $t9,0x30($t9) -> live.pad2, gates shadow/tail branch)
//   0x80024a5c/60 : lui $at,0x8008 ; addiu $at,$at,-0x75A8
//                   (then lw $t8,0x08($at) -> live.position.z, plus 0x34..0x44 matrix rows)
//   0x80024b60/64 : lui $a0,0x8008 ; addiu $a0,$a0,-0x75A8
//                   (then lw $a0,0x28($a0) -> live.colorFilter)
//
// Without the latter three, during a supercharge-jump (live.pad2 becomes
// non-zero) the ghost's shadow/tail block is emitted using live's matrix and
// live's z, which visually snaps the ghost on top of the player until live
// lands.
//
// The flame rotation matrix stores at 0x80024110..0x80024120 are also NOP'd
// here so that rendering the ghost doesn't perturb the live flame matrix.
static void PatchDrawSpyroForGhost(void)
{
    #if BUILD == REDUX || BUILD == DUCKSTATION
    * (int*)0x80023afc = 0x3C1F8050; // lui $ra, 0x8050
    *(int*)0x80023b00 = 0x27FF0000; // addiu $ra, $ra, 0x0000
    *(int*)0x80024424 = 0x3C198050; // lui $t9, 0x8050
    *(int*)0x80024428 = 0x27390000; // addiu $t9, $t9, 0x0000
    *(int*)0x80024a5c = 0x3C018050; // lui $at, 0x8050
    *(int*)0x80024a60 = 0x24210000; // addiu $at, $at, 0x0000
    *(int*)0x80024b60 = 0x3C048050; // lui $a0, 0x8050
    *(int*)0x80024b64 = 0x24840000; // addiu $a0, $a0, 0x0000
    #elif BUILD == PS2_DECKARD
    * (int*)0x80023afc = 0x3C1F80A3; // lui $ra, 0x80A3
    *(int*)0x80023b00 = 0x27FF8000; // addiu $ra, $ra, 0x8000
    *(int*)0x80024424 = 0x3C1980A3; // lui $t9, 0x80A3
    *(int*)0x80024428 = 0x27398000; // addiu $t9, $t9, 0x8000
    *(int*)0x80024a5c = 0x3C0180A3; // lui $at, 0x80A3
    *(int*)0x80024a60 = 0x24218000; // addiu $at, $at, 0x8000
    *(int*)0x80024b60 = 0x3C0480A3; // lui $a0, 0x80A3
    *(int*)0x80024b64 = 0x24848000; // addiu $a0, $a0, 0x8000
    #endif

    // Disable flame rotation matrix changes
    * (int*)0x80024110 = NOP;
    *(int*)0x80024114 = NOP;
    *(int*)0x80024118 = NOP;
    *(int*)0x8002411C = NOP;
    *(int*)0x80024120 = NOP;
}

// Undo PatchDrawSpyroForGhost: restore every patched instruction to its
// original shipping-ROM encoding so the next DrawSpyro call sees live _spyro.
static void RestoreDrawSpyro(void)
{
    // Restore the four hardcoded live-spyro base loads.
    *(int*)0x80023afc = 0x3C1F8008; // lui $ra, 0x8008
    *(int*)0x80023b00 = 0x27FF8A58; // addiu $ra, $ra, -0x75A8
    *(int*)0x80024424 = 0x3C198008; // lui $t9, 0x8008
    *(int*)0x80024428 = 0x27398A58; // addiu $t9, $t9, -0x75A8
    *(int*)0x80024a5c = 0x3C018008; // lui $at, 0x8008
    *(int*)0x80024a60 = 0x24218A58; // addiu $at, $at, -0x75A8
    *(int*)0x80024b60 = 0x3C048008; // lui $a0, 0x8008
    *(int*)0x80024b64 = 0x24848A58; // addiu $a0, $a0, -0x75A8

    // Re-enable flame rotation matrix stores.
    *(int*)0x80024110 = 0xAC2700B8;
    *(int*)0x80024114 = 0xAC2800BC;
    *(int*)0x80024118 = 0xAC2900C0;
    *(int*)0x8002411C = 0xAC2A00C4;
    *(int*)0x80024120 = 0xAC2B00C8;
}

/*
 * Convert each Spyro DrawSpyro packet emitted for the ghost into an untextured
 * semi-transparent shaded poly AND prepend a Draw Mode (GP0 0xE1) command that
 * forces ABR=00 (50% B + 50% F).
 *
 * The next textured polygon the game emits will overwrite this Draw Mode via
 * its own inline TPAGE word, so the global state doesn't stay broken.
 */
static void MakeGhostDrawSpyroPacketsTransparent(u8* start, u8* end)
{
    static const u32 DR_MODE_50_BLEND = 0xE1000400u;

    u8* prim = start;

    while (prim < end)
    {
        int packet_len = prim[3];
        int packet_size = (packet_len + 1) * 4;

        if (packet_size <= 0)
            break;

        if (prim + packet_size > end)
            break;

        if (prim[7] == POLY3F_OPAQUE_SHADEDANDTEXTURED_RAW)
        {
            u32* words = (u32*)prim;
            /* Capture original GT3 source words before we trample the layout. */
            u32 col0 = words[1] & 0x00FFFFFFu;  /* drop original 0x34 command byte */
            u32 xy0 = words[2];
            u32 col1 = words[4];
            u32 xy1 = words[5];
            u32 col2 = words[7];
            u32 xy2 = words[8];

            /* Tag link/next pointer in low 24 bits is preserved; only set length byte. */
            prim[3] = 7;
            words[1] = DR_MODE_50_BLEND;
            words[2] = (((u32)POLY3F_TRANSPARENT_SHADED) << 24) | col0;
            words[3] = xy0;
            words[4] = col1;
            words[5] = xy1;
            words[6] = col2;
            words[7] = xy2;
        }
        else if (prim[7] == POLY4F_OPAQUE_SHADEDANDTEXTURED_RAW)
        {
            u32* words = (u32*)prim;
            u32 col0 = words[1] & 0x00FFFFFFu;
            u32 xy0 = words[2];
            u32 col1 = words[4];
            u32 xy1 = words[5];
            u32 col2 = words[7];
            u32 xy2 = words[8];
            u32 col3 = words[10];
            u32 xy3 = words[11];

            prim[3] = 9;
            words[1] = DR_MODE_50_BLEND;
            words[2] = (((u32)POLY4F_TRANSPARENT_SHADED) << 24) | col0;
            words[3] = xy0;
            words[4] = col1;
            words[5] = xy1;
            words[6] = col2;
            words[7] = xy2;
            words[8] = col3;
            words[9] = xy3;
        }

        prim += packet_size;
    }
}

/*
 * Convert each Spyro DrawSpyro packet emitted for the ghost into a closed
 * monochrome semi-transparent polyline tracing the original triangle/quad
 * outline -- UNLESS the packet's longest edge is shorter than
 * GHOST_WIRE_LEN_SQ_MIN screen pixels squared, in which case the entire
 * packet is dropped (Option 2: length-threshold simplification).
 */
// #define GHOST_WIREFRAME_COLOR    0x00FFE0E0u  /* warm white; tweak to taste */
// #define POLYLINE_MONO_SEMITRANS  0x4Au
// #define POLYLINE_TERMINATOR      0x55555555u
// #define GHOST_WIRE_LEN_SQ_MIN    64           /* squared screen-pixel length */

// static int GhostWireEdgeLenSq(u32 xy_a, u32 xy_b)
// {
//     int ax = (int)(short)(xy_a & 0xFFFFu);
//     int ay = (int)(short)(xy_a >> 16);
//     int bx = (int)(short)(xy_b & 0xFFFFu);
//     int by = (int)(short)(xy_b >> 16);
//     int dx = bx - ax;
//     int dy = by - ay;
//     return dx * dx + dy * dy;
// }

// static int GhostWireMaxLenSq3(u32 xy0, u32 xy1, u32 xy2)
// {
//     int a = GhostWireEdgeLenSq(xy0, xy1);
//     int b = GhostWireEdgeLenSq(xy1, xy2);
//     int c = GhostWireEdgeLenSq(xy2, xy0);
//     int m = a;
//     if (b > m) m = b;
//     if (c > m) m = c;
//     return m;
// }

// static int GhostWireMaxLenSq4(u32 xy0, u32 xy1, u32 xy3, u32 xy2)
// {
//     /* Quad outline edges: 0-1, 1-3, 3-2, 2-0. */
//     int a = GhostWireEdgeLenSq(xy0, xy1);
//     int b = GhostWireEdgeLenSq(xy1, xy3);
//     int c = GhostWireEdgeLenSq(xy3, xy2);
//     int d = GhostWireEdgeLenSq(xy2, xy0);
//     int m = a;
//     if (b > m) m = b;
//     if (c > m) m = c;
//     if (d > m) m = d;
//     return m;
// }

static void MakeGhostDrawSpyroPacketsWireframe(u8* start, u8* end)
{
    // static const u32 DR_MODE_50_BLEND = 0xE1000400u;

    // u8* prim = start;

    // while (prim < end)
    // {
    //     int packet_len = prim[3];
    //     int packet_size = (packet_len + 1) * 4;

    //     if (packet_size <= 0)
    //         break;
    //     if (prim + packet_size > end)
    //         break;

    //     if (prim[7] == POLY3F_OPAQUE_SHADEDANDTEXTURED_RAW)
    //     {
    //         u32* words = (u32*)prim;
    //         u32 xy0 = words[2];
    //         u32 xy1 = words[5];
    //         u32 xy2 = words[8];

    //         if (GhostWireMaxLenSq3(xy0, xy1, xy2) < GHOST_WIRE_LEN_SQ_MIN)
    //         {
    //             /* All edges shorter than threshold: drop the packet entirely. */
    //             prim[3] = 0;
    //         }
    //         else
    //         {
    //             prim[3] = 7;
    //             words[1] = DR_MODE_50_BLEND;
    //             words[2] = (((u32)POLYLINE_MONO_SEMITRANS) << 24) | GHOST_WIREFRAME_COLOR;
    //             words[3] = xy0;
    //             words[4] = xy1;
    //             words[5] = xy2;
    //             words[6] = xy0;  /* close loop */
    //             words[7] = POLYLINE_TERMINATOR;
    //         }
    //     }
    //     else if (prim[7] == POLY4F_OPAQUE_SHADEDANDTEXTURED_RAW)
    //     {
    //         u32* words = (u32*)prim;
    //         u32 xy0 = words[2];
    //         u32 xy1 = words[5];
    //         u32 xy2 = words[8];
    //         u32 xy3 = words[11];

    //         if (GhostWireMaxLenSq4(xy0, xy1, xy3, xy2) < GHOST_WIRE_LEN_SQ_MIN)
    //         {
    //             prim[3] = 0;
    //         }
    //         else
    //         {
    //             prim[3] = 8;
    //             words[1] = DR_MODE_50_BLEND;
    //             words[2] = (((u32)POLYLINE_MONO_SEMITRANS) << 24) | GHOST_WIREFRAME_COLOR;
    //             words[3] = xy0;
    //             words[4] = xy1;
    //             words[5] = xy3;  /* PS1 quad: walk 0 -> 1 -> 3 -> 2 -> 0 */
    //             words[6] = xy2;
    //             words[7] = xy0;  /* close loop */
    //             words[8] = POLYLINE_TERMINATOR;
    //         }
    //     }

    //     prim += packet_size;
    // }
}

static void FormatFramesAsMinSec(int frames, char* out)
{
    int t = frames / GHOST_REC_TIME_FPS;
    if (t < 0)
        t = 0;
    int m = t / 60;
    int s = t % 60;
    sprintf(out, deckard_str_fmt_min_sec, m, s);
}

static void GhostDrawRecordingTimeHud(void)
{
    int maxF = GHOST_RECORDING_HUD_MAX_FRAMES;
    int curF = g_ghostB.currentFrame;
    if (curF < 0)
        curF = 0;
    if (curF > maxF)
        curF = maxF;
    char t1[16];
    char t2[16];
    FormatFramesAsMinSec(curF, t1);
    FormatFramesAsMinSec(maxF, t2);
    /* '/' is not handled in DrawText (UVs stay stale); '-' is in the atlas. */
    sprintf(g_ghost_rec_time_line, deckard_str_fmt_s_dash_s, t1, t2);
    DrawTextSimple(g_ghost_rec_time_line, 8, 200);
}

void GhostUpdate(void)
{
    Spyro* temp_spyro = (Spyro*)TEMP_SPYRO_REGION;

    if (!ghost_menu.ghosts_enabled)
        return;

    if (s_recording_too_long_message_timer > 0 && _gameState == GAMESTATE_GAMEPLAY)
    {
        DrawTextSimple(deckard_str_ghost_recording_too_long, 8, 168);
        s_recording_too_long_message_timer--;
    }

    savestate_selection = 0; // Always force savestate slot to 0, so you don't overwrite the ghost

    if (_gameState != GAMESTATE_GAMEPLAY)
        return;

    if (s_ghost_render_frame_ready)
    {
        u8RGBA ghost_tint;

        // Apply ghost visual effect. DrawSpyro's hardcoded
        // `lw $a0, 0x28($a0)` load at 0x80024b64 is redirected to
        // temp_spyro by PatchDrawSpyroForGhost(), so the tint must be
        // written to temp_spyro rather than _spyro. No reset needed —
        // GhostButtonCheck's memcpy refreshes temp_spyro each frame.
        /*
         * TEMP_SPYRO_REGION lives in Deckard RAM on PS2; use Deckard-safe
         * byte stores so color filter bytes land on the expected lanes.
         */
        GetGhostTint(&ghost_tint);
        DECK_WRITE_U8(&temp_spyro->colorFilter.R, ghost_tint.R);
        DECK_WRITE_U8(&temp_spyro->colorFilter.G, ghost_tint.G);
        DECK_WRITE_U8(&temp_spyro->colorFilter.B, ghost_tint.B);
        DECK_WRITE_U8(&temp_spyro->colorFilter.opacity, ghost_tint.opacity);

        if (ghost_menu.ghosts_visual == VISUAL_SPYRO ||
            ghost_menu.ghosts_visual == VISUAL_SPYRO_WIREFRAME)
        {
            u8* prim_start = (u8*)_ptr_graphicsRelated;
            u8* prim_end;

            PatchDrawSpyroForGhost();
            DrawSpyro();
            RestoreDrawSpyro();

            prim_end = (u8*)_ptr_graphicsRelated;

            if (ghost_menu.ghosts_visual == VISUAL_SPYRO_WIREFRAME)
                MakeGhostDrawSpyroPacketsWireframe(prim_start, prim_end);
            else
                MakeGhostDrawSpyroPacketsTransparent(prim_start, prim_end);
        }
        else
        {
            DrawGhostDiamond(temp_spyro);
        }
    }

    if (g_ghostB.isActive && g_ghostB.isRecording && _gameState == GAMESTATE_GAMEPLAY)
    {
        int maxF = GHOST_RECORDING_HUD_MAX_FRAMES;
        int curF = g_ghostB.currentFrame;
        if (curF < 0)
            curF = 0;
        if (curF > maxF)
            curF = maxF;
        int remaining = maxF - curF;
        bool low_space = (remaining > 0 && remaining <= GHOST_REC_SPACE_WARN_FRAMES);

        if (low_space)
        {
            DrawTextSimple(deckard_str_ghost_recording_space_warn, 8, 168);
        }
        if (ghost_menu.show_recording_length || low_space)
        {
            GhostDrawRecordingTimeHud();
        }
    }
}

static void DrawGhostPurpleLine(Vec3* point1, Vec3* point2)
{
    ShadedLine line;

    line.point1Pos.x = point1->x;
    line.point1Pos.y = point1->y;
    line.point2Pos.x = point2->x;
    line.point2Pos.y = point2->y;

    line.colorP1.R = 0xB0;
    line.colorP1.G = 0x40;
    line.colorP1.B = 0xFF;
    line.colorP2.R = 0xB0;
    line.colorP2.G = 0x40;
    line.colorP2.B = 0xFF;

    CustomDrawLineShaded(line);
}

static void DrawGhostDiamond(Spyro* temp_spyro)
{
    // Tune these until the diamond snugly covers Spyro's body.
    // All values are half-extents in world units from the diamond center.
    #define GH_HALF_HEIGHT  170  // top and bottom spike (Z axis)
    #define GH_HALF_WIDTH   220  // left and right spike (horizontal plane)
    #define GH_FRONT_DEPTH  500  // head-facing spike
    #define GH_BACK_DEPTH   260  // tail-facing spike
    #define GH_Y_OFFSET      20   // shift center up from pos to better align with body
    #define GH_TRIG_SCALE    4096
    #define GH_ANGLE_MASK    0xFFF

    Vec3 pos = temp_spyro->position;
    int cx = pos.x;
    int cy = pos.y;
    int cz = pos.z + GH_Y_OFFSET;

    // visualAngle yaw/pitch are bytes in the temp_spyro region; use Deckard byte reads.
    int relYaw = (int)DECK_READ_U8(&temp_spyro->visualAngle.yaw);
    int relPitch = (int)DECK_READ_U8(&temp_spyro->visualAngle.pitch);
    // Scale 0-0xFF to SinScaled's 0-0xFFF range.
    int heading = (-(relYaw << 4)) & GH_ANGLE_MASK;
    int pitch = (relPitch << 4) & GH_ANGLE_MASK;
    int pitchCos = SinScaled((unsigned int)((pitch + 0x400) & GH_ANGLE_MASK));
    int pitchSin = SinScaled((unsigned int)pitch);
    int forwardX = SinScaled((unsigned int)((heading + 0x400) & GH_ANGLE_MASK));
    int forwardY = -SinScaled((unsigned int)heading);
    int rightX = SinScaled((unsigned int)heading);
    int rightY = SinScaled((unsigned int)((heading + 0x400) & GH_ANGLE_MASK));
    int forwardZ = pitchSin;

    forwardX = (forwardX * pitchCos) / GH_TRIG_SCALE;
    forwardY = (forwardY * pitchCos) / GH_TRIG_SCALE;

    // 6 vertices of the octahedron
    Vec3 wTop = { cx, cy, cz + GH_HALF_HEIGHT };
    Vec3 wBot = { cx, cy, cz - GH_HALF_HEIGHT };
    Vec3 wLeft = { cx - ((rightX * GH_HALF_WIDTH) / GH_TRIG_SCALE),  cy - ((rightY * GH_HALF_WIDTH) / GH_TRIG_SCALE),  cz };
    Vec3 wRight = { cx + ((rightX * GH_HALF_WIDTH) / GH_TRIG_SCALE), cy + ((rightY * GH_HALF_WIDTH) / GH_TRIG_SCALE), cz };
    Vec3 wFront = { cx + ((forwardX * GH_FRONT_DEPTH) / GH_TRIG_SCALE), cy + ((forwardY * GH_FRONT_DEPTH) / GH_TRIG_SCALE), cz + ((forwardZ * GH_FRONT_DEPTH) / GH_TRIG_SCALE) };
    Vec3 wBack = { cx - ((forwardX * GH_BACK_DEPTH) / GH_TRIG_SCALE),  cy - ((forwardY * GH_BACK_DEPTH) / GH_TRIG_SCALE),  cz - ((forwardZ * GH_BACK_DEPTH) / GH_TRIG_SCALE) };

    Vec3 sTop, sBot, sLeft, sRight, sFront, sBack;
    WorldToScreen(&sTop, &wTop);
    WorldToScreen(&sBot, &wBot);
    WorldToScreen(&sLeft, &wLeft);
    WorldToScreen(&sRight, &wRight);
    WorldToScreen(&sFront, &wFront);
    WorldToScreen(&sBack, &wBack);

    // Top 4 edges
    DrawGhostPurpleLine(&sTop, &sLeft);
    DrawGhostPurpleLine(&sTop, &sRight);
    DrawGhostPurpleLine(&sTop, &sFront);
    DrawGhostPurpleLine(&sTop, &sBack);

    // Bottom 4 edges
    DrawGhostPurpleLine(&sBot, &sLeft);
    DrawGhostPurpleLine(&sBot, &sRight);
    DrawGhostPurpleLine(&sBot, &sFront);
    DrawGhostPurpleLine(&sBot, &sBack);

    // Equator 4 edges
    DrawGhostPurpleLine(&sLeft, &sFront);
    DrawGhostPurpleLine(&sFront, &sRight);
    DrawGhostPurpleLine(&sRight, &sBack);
    DrawGhostPurpleLine(&sBack, &sLeft);
}


// Save/restore ghost playback and recording positions alongside savestates
void GhostSaveState(int slot)
{
    GhostHeader* headerB = GET_GHOST_HEADER(GHOST_REGION_B);

    g_ghost_pos_snapshots[slot].playbackFrame = g_ghostA.currentFrame;
    g_ghost_pos_snapshots[slot].playbackPos = g_ghostA.currentPosition;
    g_ghost_pos_snapshots[slot].playbackDragonEventIndex = s_playback_dragon_event_index;
    g_ghost_pos_snapshots[slot].playbackDragonPauseRemaining = s_playback_dragon_pause_remaining;
    g_ghost_pos_snapshots[slot].recordFrame = g_ghostB.currentFrame;
    g_ghost_pos_snapshots[slot].recordPrevPos = g_ghostB.previousPosition;
    g_ghost_pos_snapshots[slot].recordingDragonEventCount =
        (g_ghostB.isActive && g_ghostB.isRecording && GhostHeaderIsV2(headerB)) ?
        GhostDragonEventCount(headerB) : 0;
    g_ghost_pos_snapshots[slot].recordingDragonEventIndex = s_recording_dragon_event_index;
    g_ghost_pos_snapshots[slot].recordingDragonPauseFrames = s_recording_dragon_pause_frames;
    g_ghost_pos_snapshots[slot].recordingWasInDragon = s_recording_was_in_dragon;
}

void GhostLoadState(int slot)
{
    // Seek playback ghost back to where it was when the state was saved
    if (g_ghostA.isActive && !g_ghostA.isRecording)
    {
        g_ghostA.currentFrame = g_ghost_pos_snapshots[slot].playbackFrame;
        g_ghostA.currentPosition = g_ghost_pos_snapshots[slot].playbackPos;
        s_playback_dragon_event_index = g_ghost_pos_snapshots[slot].playbackDragonEventIndex;
        s_playback_dragon_pause_remaining = g_ghost_pos_snapshots[slot].playbackDragonPauseRemaining;
    }

    // Trim the recording back to the save point so deltas stay continuous
    if (g_ghostB.isActive && g_ghostB.isRecording)
    {
        g_ghostB.currentFrame = g_ghost_pos_snapshots[slot].recordFrame;
        g_ghostB.previousPosition = g_ghost_pos_snapshots[slot].recordPrevPos;
        s_recording_dragon_event_index = g_ghost_pos_snapshots[slot].recordingDragonEventIndex;
        s_recording_dragon_pause_frames = g_ghost_pos_snapshots[slot].recordingDragonPauseFrames;
        s_recording_was_in_dragon = g_ghost_pos_snapshots[slot].recordingWasInDragon;

        // Sync the in-memory header so GhostOnRunComplete sees the right frame count
        GhostHeader* headerB = GET_GHOST_HEADER(GHOST_REGION_B);
        GhostHdrStoreI32(&headerB->frameCount, g_ghostB.currentFrame);
        if (GhostHeaderIsV2(headerB))
        {
            int i;
            int eventCount = g_ghost_pos_snapshots[slot].recordingDragonEventCount;
            if (eventCount < 0)
                eventCount = 0;
            if (eventCount > GHOST_MAX_DRAGON_EVENTS)
                eventCount = GHOST_MAX_DRAGON_EVENTS;

            GhostHdrStoreI32(&headerB->dragonEventCount, eventCount);
            if (s_recording_dragon_event_index >= 0 &&
                s_recording_dragon_event_index < eventCount)
            {
                GhostHdrStoreI32(
                    &headerB->dragonEvents[s_recording_dragon_event_index].durationFrames,
                    s_recording_dragon_pause_frames);
            }
            for (i = eventCount; i < GHOST_MAX_DRAGON_EVENTS; i++)
            {
                GhostHdrStoreI32(&headerB->dragonEvents[i].frameIndex, 0);
                GhostHdrStoreI32(&headerB->dragonEvents[i].durationFrames, 0);
            }
        }
    }
}


// Helpers

// Get recording stats for display
int GhostGetRecordedFrameCount(void)
{
    if (g_ghostB.isActive && g_ghostB.isRecording)
        return g_ghostB.currentFrame;
    return 0;
}

int GhostGetPlaybackFrameCount(void)
{
    if (g_ghostA.isActive && !g_ghostA.isRecording)
    {
        GhostHeader* header = GET_GHOST_HEADER(GHOST_REGION_A);
        return GhostHdrLoadI32(&header->frameCount);
    }
    return 0;
}

int GhostGetPlaybackCurrentFrame(void)
{
    if (g_ghostA.isActive && !g_ghostA.isRecording)
        return g_ghostA.currentFrame;
    return 0;
}

int GhostGetMaxFrames(void)
{
    return GHOST_RECORDING_HUD_MAX_FRAMES;
}

bool GhostIsRacing(void)
{
    return g_ghostA.isActive && !g_ghostA.isRecording && g_ghostB.isActive && g_ghostB.isRecording;
}

extern int savestated_level_ids[3];

void GhostInvalidateStoredGhosts(void)
{
    StopGhost(&g_ghostA);
    StopGhost(&g_ghostB);
    ResetGhostDragonRuntime();
    ClearGhostHeader(GHOST_REGION_A);
    ClearGhostHeader(GHOST_REGION_B);
    memset(g_ghost_pos_snapshots, 0, sizeof(g_ghost_pos_snapshots));
    g_ghost_new_best = false;
    s_recording_failed_too_long = false;
    s_recording_too_long_message_timer = 0;
}

void GhostResetAll(void)
{
    GhostsAutoplayResetExternalLoadState();
    GhostInvalidateStoredGhosts();

    #if BUILD == PS2_DECKARD
    {
        memset((void*)0x80A29000, 0, 0x46FE0);

        savestate_selection = 0; // Force savestate slot to 1

        // Clear savestate slots 2/3 level IDs
        savestated_level_ids[1] = 0;
        savestated_level_ids[2] = 0;

    }
    #endif
}

void GhostAutoEnableIfLoaded(void)
{
    GhostHeader* headerA = GET_GHOST_HEADER(GHOST_REGION_A);
    if (GhostHeaderHasCompletedRun(headerA))
    {
        ghost_menu.ghosts_enabled = true;
        il_menu.il_state = true;
    }
}
