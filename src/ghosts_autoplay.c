#include <common.h>
#include <custom_menu.h>
#include <ghost_boot_handshake.h>

#define LS_READY 0
#define LS_LEVEL_CHOSEN 1
#define LS_STARTING_LEVEL_LOAD 2

/* Require a short stable inventory window before forcing selection. */
#define INVENTORY_STABLE_FRAMES_REQUIRED 20
/* If inventory did not open in time, request it again from gameplay. */
#define INVENTORY_RETRY_TIMEOUT_FRAMES   180
/* If handoff to level load never starts, retry selection flow. */
#define HANDOFF_TIMEOUT_FRAMES           90

typedef enum GhostAutoplayPhase
{
    GHOST_AUTOPLAY_IDLE = 0,
    GHOST_AUTOPLAY_WAIT_GAMEPLAY,
    GHOST_AUTOPLAY_WAIT_INVENTORY_STABLE,
    GHOST_AUTOPLAY_APPLY_SELECTION,
    GHOST_AUTOPLAY_WAIT_HANDOFF,
    GHOST_AUTOPLAY_DONE
} GhostAutoplayPhase;

extern ILMenu il_menu;
extern GhostMenu ghost_menu;

/* levelselect.c globals */
extern int levelSelectState;
extern int buttonSelection;

static GhostAutoplayPhase s_autoplay_phase = GHOST_AUTOPLAY_IDLE;
static int s_target_world_menu = 0;
static int s_target_level_button = 0;
static int s_phase_timer_frames = 0;
static int s_inventory_stable_frames = 0;
static int s_request_is_active = 0;

static GhostBootHandshake* GhostBootRequestPtr(void)
{
    return (GhostBootHandshake*)GHOST_BOOT_HANDSHAKE_ADDR;
}

static int GhostBootReadInt(int* p)
{
    return GhostBootHandshakeLoadInt(p);
}

static void GhostBootWriteInt(int* p, int v)
{
    GhostBootHandshakeStoreInt(p, v);
}

static void GhostBootClearRequest(void)
{
    int i;
    int* words = (int*)GhostBootRequestPtr();
    for (i = 0; i < GHOST_BOOT_NUM_INTS; i++)
        GhostBootWriteInt(&words[i], 0);
}

/* Convert level ID (11, 12, ... 64) into inventory world/button indices. */
static int MapLevelIdToInventorySelection(int level_id, int* out_world_menu, int* out_level_button)
{
    int world = (level_id / 10) - 1;
    int button = level_id % 10;
    if (world < 0 || world > 5 || button < 0 || button > 5)
        return 0;
    *out_world_menu = world;
    *out_level_button = button;
    return 1;
}

/*
 * Validate handshake contents and return the requested level ID when valid.
 * A request is considered valid only for normal level-select destinations
 * (world 0..5, button 0..5).
 */
static int TryReadValidBootRequest(int* out_target_level_id)
{
    GhostBootHandshake* req = GhostBootRequestPtr();
    int magic = GhostBootReadInt(&req->magic);
    int version = GhostBootReadInt(&req->version);
    int flags = GhostBootReadInt(&req->flags);
    int level_id = GhostBootReadInt(&req->target_level_id);
    int world;
    int button;

    if (magic != (int)GHOST_BOOT_MAGIC)
        return 0;
    if (version != GHOST_BOOT_VERSION)
        return 0;
    if (!(flags & GHOST_BOOT_FLAG_FLYIN_REQUEST))
        return 0;

    world = (level_id / 10) - 1;
    button = level_id % 10;
    if (world < 0 || world > 5 || button < 0 || button > 5)
        return 0;

    *out_target_level_id = level_id;
    return 1;
}

/*
 * Arm autoplay after a valid request:
 * - map requested level to inventory indices
 * - force ghost/IL toggles on for deterministic behavior
 * - start state machine from gameplay wait
 */
static void ArmGhostAutoplay(int target_level_id)
{
    if (!MapLevelIdToInventorySelection(target_level_id, &s_target_world_menu, &s_target_level_button))
    {
        GhostBootClearRequest();
        s_autoplay_phase = GHOST_AUTOPLAY_DONE;
        return;
    }

    /* Goal is immediate ghost playback/IL state after boot. */
    ghost_menu.ghosts_enabled = true;
    il_menu.il_state = true;

    s_autoplay_phase = GHOST_AUTOPLAY_WAIT_GAMEPLAY;
    s_phase_timer_frames = 0;
    s_inventory_stable_frames = 0;
    s_request_is_active = 1;
}

void GhostsAutoplayUpdate(void)
{
#if BUILD == REDUX || BUILD == DUCKSTATION || BUILD == PS2_DECKARD
    int requested_level_id;

    /* Stay dormant until a valid handshake request appears. */
    if (!s_request_is_active)
    {
        if (TryReadValidBootRequest(&requested_level_id))
            ArmGhostAutoplay(requested_level_id);
        else
            return;
    }

    /* Keep ghost/IL toggles forced on while autoplay request is in flight. */
    il_menu.il_state = true;
    ghost_menu.ghosts_enabled = true;

    switch (s_autoplay_phase)
    {
    case GHOST_AUTOPLAY_WAIT_GAMEPLAY:
        /* Wait until gameplay is active before forcing inventory open. */
        if (_gameState == GAMESTATE_GAMEPLAY && !_isInInGameCutscene)
        {
            PrepareInventoryGamestate(true);
            s_autoplay_phase = GHOST_AUTOPLAY_WAIT_INVENTORY_STABLE;
            s_phase_timer_frames = 0;
            s_inventory_stable_frames = 0;
        }
        break;

    case GHOST_AUTOPLAY_WAIT_INVENTORY_STABLE:
        /*
         * Inventory can flicker during transition. Require a short stable
         * window before applying selection.
         */
        s_phase_timer_frames++;
        if (_gameState == GAMESTATE_INVENTORY && _pausedTimer > 7 && levelSelectState == LS_READY)
        {
            s_inventory_stable_frames++;
            if (s_inventory_stable_frames >= INVENTORY_STABLE_FRAMES_REQUIRED)
            {
                s_autoplay_phase = GHOST_AUTOPLAY_APPLY_SELECTION;
                s_phase_timer_frames = 0;
            }
        }
        else
        {
            s_inventory_stable_frames = 0;
            if (s_phase_timer_frames > INVENTORY_RETRY_TIMEOUT_FRAMES && _gameState == GAMESTATE_GAMEPLAY)
            {
                PrepareInventoryGamestate(true);
                s_phase_timer_frames = 0;
            }
        }
        break;

    case GHOST_AUTOPLAY_APPLY_SELECTION:
        /*
         * Bypass DetermineButton() completely: its multitap fallback can rewrite
         * selection to L3 during inventory transitions. We set menu/button
         * directly, then mark LEVEL_CHOSEN so LevelSelectUpdate handles load.
         */
        _selectMenuOption = s_target_world_menu;
        buttonSelection = s_target_level_button;
        levelSelectState = LS_LEVEL_CHOSEN;
        _currentButton = 0;

        s_autoplay_phase = GHOST_AUTOPLAY_WAIT_HANDOFF;
        s_phase_timer_frames = 0;
        break;

    case GHOST_AUTOPLAY_WAIT_HANDOFF:
        /*
         * Successful handoff means level select already started exiting/loading.
         * Once seen, clear handshake so next boot starts clean.
         */
        s_phase_timer_frames++;
        if (levelSelectState == LS_STARTING_LEVEL_LOAD ||
            _gameState == GAMESTATE_EXITING_LEVEL ||
            _gameState == GAMESTATE_LOADING)
        {
            GhostBootClearRequest();
            s_autoplay_phase = GHOST_AUTOPLAY_DONE;
            break;
        }

        /* If nothing happened for too long, retry from inventory-stable step. */
        if (s_phase_timer_frames > HANDOFF_TIMEOUT_FRAMES)
        {
            s_autoplay_phase = GHOST_AUTOPLAY_WAIT_INVENTORY_STABLE;
            s_phase_timer_frames = 0;
            s_inventory_stable_frames = 0;
        }
        break;

    case GHOST_AUTOPLAY_DONE:
    default:
        break;
    }
#else
    (void)0;
#endif
}
