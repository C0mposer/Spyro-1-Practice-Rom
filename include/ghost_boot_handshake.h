#ifndef GHOST_BOOT_HANDSHAKE_H
#define GHOST_BOOT_HANDSHAKE_H

/*
 * Ghost USB boot handshake: 8 ints immediately before Ghost A in RAM.
 *   Redux / DuckStation: 0x80600000 - 0x20 = 0x805FFFE0 (plain int load/store).
 *   PS2 Deckard:     0x80A28FE0 (full-word byte swap; see deckard_ghost_boot.h).
 */

#define GHOST_BOOT_MAGIC            0x464C5931u /* 'FLY1' — keep in sync with deckard_ghost_boot.h */
#define GHOST_BOOT_VERSION          1
#define GHOST_BOOT_FLAG_FLYIN_REQUEST 0x1

#define GHOST_BOOT_NUM_INTS         8

typedef struct GhostBootHandshake
{
    int magic;
    int version;
    int flags;
    int target_level_id;
    int reserved0;
    int reserved1;
    int reserved2;
    int reserved3;
} GhostBootHandshake;

#if BUILD == PS2_DECKARD

#include "deckard_ghost_boot.h"

#define GHOST_BOOT_HANDSHAKE_ADDR   DECKARD_GHOST_BOOT_ADDR

/*
 * PS2 ghost-loader may place logical int values in EE order at this address; the
 * same bytes read as 32-bit words in Deckard may match GHOST_BOOT_MAGIC in RAM
 * without the usual per-word storage swap. Ghost data from ghostrd/ghostwr
 * still uses deckard_ghost_boot_* / GhostHdrStoreI32 layout.
 * If the raw first word is 'FLY1' but byte-swapping it does not yield 'FLY1',
 * the block is in native logical layout — use plain int load/store.
 */
static inline int GhostBootHandshakeDeckardUsesNativeLeWords(void)
{
    int* p = (int*)(void*)GHOST_BOOT_HANDSHAKE_ADDR;
    unsigned raw = (unsigned)(*(volatile unsigned*)p);
    if (raw != (unsigned)GHOST_BOOT_MAGIC) return 0;
    {
        int via_swap = deckard_ghost_boot_load_int(p);
        return via_swap != (int)GHOST_BOOT_MAGIC;
    }
}

static inline int GhostBootHandshakeLoadInt(int* p)
{
    if (GhostBootHandshakeDeckardUsesNativeLeWords()) return *p;
    return deckard_ghost_boot_load_int(p);
}

static inline void GhostBootHandshakeStoreInt(int* p, int v)
{
    if (GhostBootHandshakeDeckardUsesNativeLeWords()) *p = v;
    else
        deckard_ghost_boot_store_int(p, v);
}

#else

/* Ghost A @ 0x80600000 on Redux / DuckStation extra build (ghost_replay.c) */
#define GHOST_BOOT_HANDSHAKE_ADDR   0x805FFFE0u

static inline int GhostBootHandshakeLoadInt(int* p)
{
    return *p;
}

static inline void GhostBootHandshakeStoreInt(int* p, int v)
{
    *p = v;
}

#endif

#endif /* GHOST_BOOT_HANDSHAKE_H */
