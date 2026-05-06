#ifndef DECKARD_GHOST_BOOT_H
#define DECKARD_GHOST_BOOT_H

/*
 * USB/network ghost-loader handshake (PS2 Deckard only).
 * Placed 0x20 bytes before Ghost A so ghost-loader and IOP ghostwr can pack
 * it without overlapping 0x80A29000; use only 32-bit ints (same storage rules
 * as GhostHeader in 0x80Axxxx: full words are byte-swapped vs normal EE).
 *
 * Game-side readers should use deckard_ghost_boot_load_int / _store_int below.
 *
 * Why not the inventory "exit level" path?
 *   LevelSelectUpdate() + MOVEMENT_SUBSTATE_LOADING is tied to
 *   _selectMenuOption + buttonSelection from the *inventory* (pause) flow. That
 *   is a different code path from touching a world portal.
 *
 * Vanilla world-portal entry (homeworld -> sublevel) is driven by
 *   ChangeLevelIDRelated (Ghidra @ 0x80057150), case 6: it sets levelID and
 *   portalNumber from the portal's moby table, canFlyIn = 1, and either
 *   kicks a simple load (levelLoadState = 0, gameState = LOADING, ...) or
 *   the branch that adjusts portal-side moby state.
 *   ReturnHomeParamSet (0x8002c6d0) is only for *returning to homeworld*
 *   (levelID = floor(homeworld) from levelIDPortalExit), not for entering
 *   a new sublevel.
 *
 * For a cold start at Artisan HW with no portal moby, the same *effect* as
 * a portal+fly-in is already approximated by InstaLoadUpdate(): set
 *   _levelID (and _levelIDIndex to match), then _gameState = LOADING,
 *   _levelLoadState = 0xB, _canFlyIn = 1, _portalNumber = -1, etc. No
 *   inventory, no _movementSubState from "fake exit" — only those globals.
 */

#define DECKARD_GHOST_BOOT_ADDR       0x80A28FE0u
#define DECKARD_GHOST_BOOT_NUM_INTS   8
#define DECKARD_GHOST_BOOT_NUM_BYTES  (DECKARD_GHOST_BOOT_NUM_INTS * 4)

/* Magic: 'FLY1' in ASCII — bump version if the int layout ever changes. */
#define DECKARD_GHOST_BOOT_MAGIC      0x464C5931u
#define DECKARD_GHOST_BOOT_VERSION    1

/* bit 0: PS2 loader requests auto fly-in to target_level_id (clear after handling) */
#define DECKARD_GHOST_BOOT_FLAG_FLYIN_REQUEST  0x1

typedef struct DeckardGhostBootRequest
{
    int magic;
    int version;
    int flags;
    int target_level_id; /* e.g. STONE_HILL_ID; may mirror GhostHeader.levelId */
    int reserved0;
    int reserved1;
    int reserved2;
    int reserved3;
} DeckardGhostBootRequest;

/* Same 32-bit pattern as GhostHeader in this region (see ghost_replay.c). */
static inline unsigned deckard_ghost_boot_word_from_storage(unsigned w)
{
    return (w << 24) | ((w << 8) & 0xFF0000u) | ((w >> 8) & 0xFF00u) | (w >> 24);
}

static inline int deckard_ghost_boot_load_int(int* p)
{
    unsigned u = *(unsigned*)p;
    return (int)deckard_ghost_boot_word_from_storage(u);
}

static inline void deckard_ghost_boot_store_int(int* p, int v)
{
    *(unsigned*)p = deckard_ghost_boot_word_from_storage((unsigned)v);
}

#endif /* DECKARD_GHOST_BOOT_H */
