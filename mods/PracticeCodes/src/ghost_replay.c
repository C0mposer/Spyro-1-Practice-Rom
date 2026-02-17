#include <common.h>
#include <spyro.h>
#include <prims.h>
#include <custom_menu.h>
#include <deckard.h>

// GHOST REPLAY SYSTEM
// Memory Layout (0x80A29000 - 0x80A7FFFF = 356,352 bytes):
//   Ghost A (Playback): 0x80A29000 - 0x80A53FFF (~175 KB)
//   Ghost B (Recording): 0x80A54000 - 0x80A7FFFF (~175 KB)
// 
// Each ghost region:
//   - Header (24 bytes): metadata about the recording
//   - Frame Data: up to ~8,900 frames (~4 min 57 sec at 30 FPS)



// Stored ONCE at the start of each ghost's data region
typedef struct GhostHeader
{
    Vec3 initialPosition;    // 12 bytes - starting position for delta reconstruction
    int levelId;             // 4 bytes  - which level this ghost was recorded on
    int frameCount;          // 4 bytes  - total frames in this recording
    int finalTimeFrames;     // 4 bytes  - final time (for display purposes)
} GhostHeader; // 24 bytes

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

    unsigned char padding;          // alignment

    // Angle (not only sparx related it seems, its important)
    int relativeSparxAngle;            // 4 bytes
} ReplayFrameData; // 24 bytes

// State for a ghost
typedef struct GhostBufferState
{
    bool isActive;           // Is this buffer currently in use
    bool isRecording;        // true = recording, false = playing back
    int currentFrame;        // Current frame index
    Vec3 currentPosition;    // Reconstructed absolute position for playback
    Vec3 previousPosition;   // Previous position for recording deltas
} GhostBufferState;

// -----------------------------------------------------------------------------
// Memory Region Definitions
// -----------------------------------------------------------------------------

// For emu
#if BUILD == 0 || BUILD == 5
#define TEMP_SPYRO_REGION   0x80500000
#define GHOST_REGION_A      0x80600000
#define GHOST_REGION_B      0x80658000  // Halfway point
#define GHOST_REGION_SIZE   0x58000    
// For console
#elif BUILD == 2
#define TEMP_SPYRO_REGION   0x80A28000
#define GHOST_REGION_A      0x80A29000  // Playback ghost
#define GHOST_REGION_B      0x80A54000  // Recording ghost (midpoint)
#define GHOST_REGION_SIZE   0x2B000     // ~175 KB each
#endif

// Calculate max frames per ghost buffer
// (Region size - Header size) / Frame size
#define GHOST_HEADER_SIZE   sizeof(GhostHeader)
#define GHOST_FRAME_SIZE    sizeof(ReplayFrameData)
#define MAX_FRAMES_PER_GHOST ((GHOST_REGION_SIZE - GHOST_HEADER_SIZE) / GHOST_FRAME_SIZE)


#define GET_GHOST_HEADER(region) ((GhostHeader*)(region))
#define GET_GHOST_FRAMES(region) ((ReplayFrameData*)((region) + GHOST_HEADER_SIZE))


// Deckard region helpers: sub-32-bit accesses in 0x80Axxxxx are half-word swapped
#if BUILD == 2
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

// Two ghost buffers: A for playback, B for recording
GhostBufferState g_ghostA = { 0 };  // Playback (race against this)
GhostBufferState g_ghostB = { 0 };  // Recording (current attempt)

extern GhostMenu ghost_menu;
extern bool has_started_insta_fly_in;
extern bool has_started_reg_fly_in;


// Start recording a new ghost into a buffer
static void StartRecording(GhostBufferState* state, unsigned int region)
{
    state->isActive = true;
    state->isRecording = true;
    state->currentFrame = 0;
    state->previousPosition = _spyro.position;

    // Initialize header
    GhostHeader* header = GET_GHOST_HEADER(region);
    header->initialPosition = _spyro.position;
    header->levelId = _levelID;
    header->frameCount = 0;
    header->finalTimeFrames = 0;
}

// Start playing back a ghost from a buffer
static void StartPlayback(GhostBufferState* state, unsigned int region)
{
    GhostHeader* header = GET_GHOST_HEADER(region);

    // Only start if there's actually data to play
    if (header->frameCount > 0)
    {
        state->isActive = true;
        state->isRecording = false;
        state->currentFrame = 0;
        state->currentPosition = header->initialPosition;
    }
}

// Stop a ghost buffer
static void StopGhost(GhostBufferState* state)
{
    state->isActive = false;
    state->isRecording = false;
    state->currentFrame = 0;
}

// Copy ghost data from one region to another (for promoting recording to playback)
static void CopyGhostData(unsigned int srcRegion, unsigned int dstRegion)
{
    GhostHeader* srcHeader = GET_GHOST_HEADER(srcRegion);
    GhostHeader* dstHeader = GET_GHOST_HEADER(dstRegion);

    // Copy header
    *dstHeader = *srcHeader;

    // Copy frame data
    ReplayFrameData* srcFrames = GET_GHOST_FRAMES(srcRegion);
    ReplayFrameData* dstFrames = GET_GHOST_FRAMES(dstRegion);

    int bytesToCopy = srcHeader->frameCount * GHOST_FRAME_SIZE;
    DECK_MEMCPY(dstFrames, srcFrames, bytesToCopy);
}

// Record a single frame
static void RecordFrame(GhostBufferState* state, unsigned int region)
{
    if (state->currentFrame >= MAX_FRAMES_PER_GHOST)
    {
        // Buffer full - stop recording
        state->isActive = false;
        return;
    }

    GhostHeader* header = GET_GHOST_HEADER(region);
    ReplayFrameData* frames = GET_GHOST_FRAMES(region);
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
    DECK_WRITE_INT(&frame->relativeSparxAngle, _spyro.relativeSparxAngle);
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
    DECK_WRITE_U8(&frame->padding, 0);

    // Update state for next frame
    state->previousPosition = _spyro.position;
    state->currentFrame++;
    header->frameCount = state->currentFrame;
}

// Play back a single frame to temp_spyro
static void PlaybackFrame(GhostBufferState* state, unsigned int region, Spyro* temp_spyro)
{
    GhostHeader* header = GET_GHOST_HEADER(region);

    if (state->currentFrame >= header->frameCount)
    {
        // ghost just stops at final position right now with testing.
        return;
    }

    ReplayFrameData* frames = GET_GHOST_FRAMES(region);
    ReplayFrameData* frame = &frames[state->currentFrame];

    // Reconstruct position from delta (shorts need deckard read)
    state->currentPosition.x += DECK_READ_SHORT(&frame->deltaX);
    state->currentPosition.y += DECK_READ_SHORT(&frame->deltaY);
    state->currentPosition.z += DECK_READ_SHORT(&frame->deltaZ);

    // Apply to temp spyro — direct writes here because DrawSpyro reads
    // through the same hardware swap, so normal writes cancel out correctly
    temp_spyro->position = state->currentPosition;
    temp_spyro->relativeSparxAngle = DECK_READ_INT(&frame->relativeSparxAngle);
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

    state->currentFrame++;
}

// API
void GhostButtonCheck(void)
{
    if (!ghost_menu.ghosts_enabled)
        return;

    // Copy Spyro data to temp region (plain memcpy — DrawSpyro reads through
    // the same hardware swap, so normal writes cancel out correctly)
    memcpy((void*)TEMP_SPYRO_REGION, &_spyro, sizeof(_spyro));

    // L3 or insta fly-in: Start fresh recording (no playback ghost)
    if (_currentButtonOneFrame == L3_BUTTON || has_started_insta_fly_in)
    {
        // Stop any existing ghosts
        StopGhost(&g_ghostA);
        StopGhost(&g_ghostB);

        // Start recording into buffer A
        StartRecording(&g_ghostA, GHOST_REGION_A);
    }
    // R3 or regular fly-in: Start racing against last ghost
    else if (_currentButtonOneFrame == R3_BUTTON || has_started_reg_fly_in)
    {
        GhostHeader* headerA = GET_GHOST_HEADER(GHOST_REGION_A);

        // Only start race mode if we have a ghost to race against
        if (headerA->frameCount > 0)
        {
            // Move current ghost A data to "safe" playback position
            // and start recording new attempt into B

            // Stop current states
            StopGhost(&g_ghostA);
            StopGhost(&g_ghostB);

            // Start playback of ghost A
            StartPlayback(&g_ghostA, GHOST_REGION_A);

            // Start recording into ghost B
            StartRecording(&g_ghostB, GHOST_REGION_B);
        }
    }
    // R1: Cancel/reset everything (just for testing)
    else if (_currentButtonOneFrame == R1_BUTTON)
    {
        StopGhost(&g_ghostA);
        StopGhost(&g_ghostB);
    }
}

// Call this when the player finishes a run
void GhostOnRunComplete(int finalTimeFrames)
{
    // If we were recording in B (racing mode), check if it's a PB
    if (g_ghostB.isActive && g_ghostB.isRecording)
    {
        GhostHeader* headerA = GET_GHOST_HEADER(GHOST_REGION_A);
        GhostHeader* headerB = GET_GHOST_HEADER(GHOST_REGION_B);

        headerB->finalTimeFrames = finalTimeFrames;

        // If this is a new PB (or first completion), promote B to A
        if (headerA->frameCount == 0 || finalTimeFrames < headerA->finalTimeFrames)
        {
            CopyGhostData(GHOST_REGION_B, GHOST_REGION_A);
        }

        StopGhost(&g_ghostB);
    }
    // If recording directly into A (solo mode)
    else if (g_ghostA.isActive && g_ghostA.isRecording)
    {
        GhostHeader* headerA = GET_GHOST_HEADER(GHOST_REGION_A);
        headerA->finalTimeFrames = finalTimeFrames;
        StopGhost(&g_ghostA);
    }
}

void GhostUpdate(void)
{
    if (!ghost_menu.ghosts_enabled)
        return;

    Spyro* temp_spyro = (Spyro*)TEMP_SPYRO_REGION;

    // Patch DrawSpyro to use temp_spyro region
    #if BUILD == 0 || BUILD == 5
    * (int*)0x80023afc = 0x3C1F8050; // lui $ra, 0x8050
    *(int*)0x80023b00 = 0x27FF0000; // addiu $ra, $ra, 0x0000
    #elif BUILD == 2
    * (int*)0x80023afc = 0x3C1F80A3; // lui $ra, 0x80A3
    *(int*)0x80023b00 = 0x27FF8000; // addiu $ra, $ra, 0x8000
    #endif

    // Disable flame rotation matrix changes
    * (int*)0x80024110 = NOP;
    *(int*)0x80024114 = NOP;
    *(int*)0x80024118 = NOP;
    *(int*)0x8002411C = NOP;
    *(int*)0x80024120 = NOP;

    // Skip during dragon cutscenes
    if (_gameState == GAMESTATE_DRAGON_STATE)
        goto restore;

    // Handle recording for ghost A (solo recording mode)
    if (g_ghostA.isActive && g_ghostA.isRecording)
    {
        RecordFrame(&g_ghostA, GHOST_REGION_A);
    }

    // Handle recording for ghost B (racing mode - new attempt)
    if (g_ghostB.isActive && g_ghostB.isRecording)
    {
        RecordFrame(&g_ghostB, GHOST_REGION_B);
    }

    // Handle playback for ghost A (racing mode - ghost to beat)
    if (g_ghostA.isActive && !g_ghostA.isRecording)
    {
        GhostHeader* headerA = GET_GHOST_HEADER(GHOST_REGION_A);

        if (g_ghostA.currentFrame < headerA->frameCount)
        {
            PlaybackFrame(&g_ghostA, GHOST_REGION_A, temp_spyro);

            // Apply ghost visual effect
            _spyro.colorFilter.B = 0xFF;
            _spyro.colorFilter.opacity = 0xCC;

            // Draw the ghost
            if (_gameState == GAMESTATE_GAMEPLAY)
            {
                if (ghost_menu.ghosts_visual == VISUAL_SPYRO)
                    DrawSpyro();
                else
                    DrawSquareOverTempSpyro(temp_spyro);
            }

            // Reset color filter
            _spyro.colorFilter.B = 0x0;
            _spyro.colorFilter.opacity = 0x0;
        }
    }

restore:
    // Restore DrawSpyro to use normal _spyro
    *(int*)0x80023afc = 0x3C1F8008; // lui $ra, 0x8008
    *(int*)0x80023b00 = 0x27FF8A58; // addiu $ra, $ra, 0x8A58

    // Re-enable flame rotation matrix
    *(int*)0x80024110 = 0xAC2700B8;
    *(int*)0x80024114 = 0xAC2800BC;
    *(int*)0x80024118 = 0xAC2900C0;
    *(int*)0x8002411C = 0xAC2A00C4;
    *(int*)0x80024120 = 0xAC2B00C8;
}

void DrawSquareOverTempSpyro(Spyro* temp_spyro)
{
    #define WORLD_SQUARE_SIZE 150

    Vec3 spyroCenter = temp_spyro->position;
    Vec3 worldPoint1, worldPoint2, worldPoint3, worldPoint4;
    Vec3 screenPoint1, screenPoint2, screenPoint3, screenPoint4;

    worldPoint1.x = spyroCenter.x - WORLD_SQUARE_SIZE;
    worldPoint1.y = spyroCenter.y + WORLD_SQUARE_SIZE;
    worldPoint1.z = spyroCenter.z;

    worldPoint2.x = spyroCenter.x - WORLD_SQUARE_SIZE;
    worldPoint2.y = spyroCenter.y - WORLD_SQUARE_SIZE;
    worldPoint2.z = spyroCenter.z;

    worldPoint3.x = spyroCenter.x + WORLD_SQUARE_SIZE;
    worldPoint3.y = spyroCenter.y + WORLD_SQUARE_SIZE;
    worldPoint3.z = spyroCenter.z;

    worldPoint4.x = spyroCenter.x + WORLD_SQUARE_SIZE;
    worldPoint4.y = spyroCenter.y - WORLD_SQUARE_SIZE;
    worldPoint4.z = spyroCenter.z;

    Poly4F poly = { .code = POLY4F_TRANSPARENT, .color.B = 0xFF };

    WorldToScreen(&screenPoint1, &worldPoint1);
    WorldToScreen(&screenPoint2, &worldPoint2);
    WorldToScreen(&screenPoint3, &worldPoint3);
    WorldToScreen(&screenPoint4, &worldPoint4);

    poly.point1Pos.x = screenPoint1.x;
    poly.point1Pos.y = screenPoint1.y;
    poly.point2Pos.x = screenPoint2.x;
    poly.point2Pos.y = screenPoint2.y;
    poly.point3Pos.x = screenPoint3.x;
    poly.point3Pos.y = screenPoint3.y;
    poly.point4Pos.x = screenPoint4.x;
    poly.point4Pos.y = screenPoint4.y;

    CustomDrawPolygon(poly, POLY4F_OPAQUE);
}


// Helpers

// Get recording stats for display
int GhostGetRecordedFrameCount(void)
{
    if (g_ghostB.isActive && g_ghostB.isRecording)
        return g_ghostB.currentFrame;
    if (g_ghostA.isActive && g_ghostA.isRecording)
        return g_ghostA.currentFrame;
    return 0;
}

int GhostGetPlaybackFrameCount(void)
{
    if (g_ghostA.isActive && !g_ghostA.isRecording)
    {
        GhostHeader* header = GET_GHOST_HEADER(GHOST_REGION_A);
        return header->frameCount;
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
    return MAX_FRAMES_PER_GHOST;
}

bool GhostIsRacing(void)
{
    return g_ghostA.isActive && !g_ghostA.isRecording && g_ghostB.isActive && g_ghostB.isRecording;
}