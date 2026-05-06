#include <common.h>
#include <ghost_debug.h>
#include <ghost_replay_api.h>
#include <custom_menu.h>
#include <deckard_strings.h>
#include <font.h>

#if GHOST_DEBUG && (BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION)

#if BUILD == PS2_DECKARD
#include <deckard.h>
#endif

extern GhostMenu ghost_menu;
extern ILMenu il_menu;
extern bool has_started_insta_fly_in;
extern bool has_started_reg_fly_in;
extern char _levelID;

#if BUILD == REDUX || BUILD == DUCKSTATION
#define GDBG_RA 0x80600000u
#define GDBG_RB 0x80658000u
#elif BUILD == PS2_DECKARD
#define GDBG_RA 0x80A29000u
#define GDBG_RB 0x80A4C7F0u
#else
#define GDBG_RA 0u
#define GDBG_RB 0u
#endif

#define GDBG_HDR_BYTES 24u
#define GDBG_FRAME_BYTES 24u

typedef struct GhostDbgHdr
{
    Vec3 initialPosition;
    int levelId;
    int frameCount;
    int finalTimeFrames;
} GhostDbgHdr;

static char HexNibble(int v)
{
    v &= 0xF;
    if (v < 10)
        return (char)('0' + v);
    return (char)('A' + (v - 10));
}

static void U32Hex(char* out, unsigned u)
{
    int i;
    out[0] = '0';
    out[1] = 'x';
    for (i = 0; i < 8; i++)
        out[2 + i] = HexNibble((int)(u >> (28 - i * 4)));
    out[10] = '\0';
}

static void I32Dec(char* out, int v)
{
    sprintf(out, deckard_str_fmt_d, v);
}

static unsigned WordSwap32(unsigned w)
{
    return (w << 24) | ((w << 8) & 0xFF0000u) | ((w >> 8) & 0xFF00u) | (w >> 24);
}

static int LogicalHdrI32(const int* p)
{
#if BUILD == PS2_DECKARD
    unsigned u = *(const unsigned*)p;
    return (int)WordSwap32(u);
#else
    return *p;
#endif
}

static void FmtHdrLine(char* line, const char* tag, unsigned base)
{
    unsigned w0 = *(unsigned*)(base + 0);
    unsigned w1 = *(unsigned*)(base + 4);
    unsigned w2 = *(unsigned*)(base + 8);
    char h0[11];
    char h1[11];
    char h2[11];
    int pos = 0;
    int i;
    const char* s;

    U32Hex(h0, w0);
    U32Hex(h1, w1);
    U32Hex(h2, w2);

    for (s = tag; *s != '\0'; s++)
        line[pos++] = *s;
    line[pos++] = ' ';
    for (i = 0; h0[i]; i++)
        line[pos++] = h0[i];
    line[pos++] = ' ';
    for (i = 0; h1[i]; i++)
        line[pos++] = h1[i];
    line[pos++] = ' ';
    for (i = 0; h2[i]; i++)
        line[pos++] = h2[i];
    line[pos] = '\0';
}

static void FmtHdrLine2(char* line, const char* tag, unsigned base)
{
    unsigned w0 = *(unsigned*)(base + 12);
    unsigned w1 = *(unsigned*)(base + 16);
    unsigned w2 = *(unsigned*)(base + 20);
    char h0[11];
    char h1[11];
    char h2[11];
    int pos = 0;
    int i;
    const char* s;

    U32Hex(h0, w0);
    U32Hex(h1, w1);
    U32Hex(h2, w2);

    for (s = tag; *s != '\0'; s++)
        line[pos++] = *s;
    line[pos++] = ' ';
    for (i = 0; h0[i]; i++)
        line[pos++] = h0[i];
    line[pos++] = ' ';
    for (i = 0; h1[i]; i++)
        line[pos++] = h1[i];
    line[pos++] = ' ';
    for (i = 0; h2[i]; i++)
        line[pos++] = h2[i];
    line[pos] = '\0';
}

void GhostDebugDraw(void)
{
    char line[72];
    int x = 6;
    int y = 6;
    int row = 0;
    int inst0;
    int inst1;
    const int line_h = 9;
    const int rows_per_col = 18;
    const int col2_x = 156;

#define GDBG_NEXT_LINE()                           \
    do                                             \
    {                                              \
        row++;                                     \
        if (row >= rows_per_col)                   \
        {                                          \
            x = col2_x;                            \
            y = 6;                                 \
        }                                          \
        else                                       \
        {                                          \
            y += line_h;                           \
        }                                          \
    } while (0)

    if (!ghost_menu.ghosts_enabled)
        return;

    DrawTextSimple(deckard_str_ghost_dbg_title, x, y);
    GDBG_NEXT_LINE();

    DrawTextSimple(g_ghost_new_best ? deckard_str_ghost_dbg_new_best_1 : deckard_str_ghost_dbg_new_best_0, x, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_ghostA.isActive ? 1 : 0);
    DrawTextSimple(deckard_str_ghost_dbg_ga_act, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_ghostA.isRecording ? 1 : 0);
    DrawTextSimple(deckard_str_ghost_dbg_ga_rec, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_ghostA.currentFrame);
    DrawTextSimple(deckard_str_ghost_dbg_ga_curfrm, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_ghostB.isActive ? 1 : 0);
    DrawTextSimple(deckard_str_ghost_dbg_gb_act, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_ghostB.isRecording ? 1 : 0);
    DrawTextSimple(deckard_str_ghost_dbg_gb_rec, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_ghostB.currentFrame);
    DrawTextSimple(deckard_str_ghost_dbg_gb_curfrm, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    DrawTextSimple(il_menu.il_state ? deckard_str_ghost_dbg_il_1 : deckard_str_ghost_dbg_il_0, x, y);
    GDBG_NEXT_LINE();

    DrawTextSimple(has_started_insta_fly_in ? deckard_str_ghost_dbg_instafly_1 : deckard_str_ghost_dbg_instafly_0, x, y);
    GDBG_NEXT_LINE();

    DrawTextSimple(has_started_reg_fly_in ? deckard_str_ghost_dbg_regfly_1 : deckard_str_ghost_dbg_regfly_0, x, y);
    GDBG_NEXT_LINE();

    I32Dec(line, (int)(signed char)_levelID);
    DrawTextSimple(deckard_str_ghost_dbg_levelid, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_dbg_ghost_update_calls);
    DrawTextSimple(deckard_str_ghost_dbg_upd_calls, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_dbg_ghost_update_dragon_skips);
    DrawTextSimple(deckard_str_ghost_dbg_upd_drgskip, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_dbg_ghost_record_calls);
    DrawTextSimple(deckard_str_ghost_dbg_rec_calls, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_dbg_ghost_start_calls);
    DrawTextSimple(deckard_str_ghost_dbg_start_calls, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_dbg_ghost_start_insta_calls);
    DrawTextSimple(deckard_str_ghost_dbg_start_insta, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_dbg_ghost_start_reg_calls);
    DrawTextSimple(deckard_str_ghost_dbg_start_reg, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    FmtHdrLine(line, deckard_str_ghost_dbg_hdr_a_w02, GDBG_RA);
    DrawTextSimple(line, x, y);
    GDBG_NEXT_LINE();

    FmtHdrLine2(line, deckard_str_ghost_dbg_hdr_a_w35, GDBG_RA);
    DrawTextSimple(line, x, y);
    GDBG_NEXT_LINE();

    FmtHdrLine(line, deckard_str_ghost_dbg_hdr_b_w02, GDBG_RB);
    DrawTextSimple(line, x, y);
    GDBG_NEXT_LINE();

    FmtHdrLine2(line, deckard_str_ghost_dbg_hdr_b_w35, GDBG_RB);
    DrawTextSimple(line, x, y);
    GDBG_NEXT_LINE();

    {
        GhostDbgHdr* ha = (GhostDbgHdr*)GDBG_RA;
        int fc = LogicalHdrI32(&ha->frameCount);
        int ft = LogicalHdrI32(&ha->finalTimeFrames);
        int lv = LogicalHdrI32(&ha->levelId);
        I32Dec(line, fc);
        DrawTextSimple(deckard_str_ghost_dbg_a_log_fc, x, y);
        DrawTextSimple(line, x + 64, y);
        GDBG_NEXT_LINE();
        I32Dec(line, ft);
        DrawTextSimple(deckard_str_ghost_dbg_a_log_ft, x, y);
        DrawTextSimple(line, x + 64, y);
        GDBG_NEXT_LINE();
        I32Dec(line, lv);
        DrawTextSimple(deckard_str_ghost_dbg_a_log_lv, x, y);
        DrawTextSimple(line, x + 64, y);
        GDBG_NEXT_LINE();
    }

    {
        GhostDbgHdr* hb = (GhostDbgHdr*)GDBG_RB;
        int fc = LogicalHdrI32(&hb->frameCount);
        int ft = LogicalHdrI32(&hb->finalTimeFrames);
        int lv = LogicalHdrI32(&hb->levelId);
        I32Dec(line, fc);
        DrawTextSimple(deckard_str_ghost_dbg_b_log_fc, x, y);
        DrawTextSimple(line, x + 64, y);
        GDBG_NEXT_LINE();
        I32Dec(line, ft);
        DrawTextSimple(deckard_str_ghost_dbg_b_log_ft, x, y);
        DrawTextSimple(line, x + 64, y);
        GDBG_NEXT_LINE();
        I32Dec(line, lv);
        DrawTextSimple(deckard_str_ghost_dbg_b_log_lv, x, y);
        DrawTextSimple(line, x + 64, y);
        GDBG_NEXT_LINE();
    }

    {
        unsigned f0 = GDBG_RA + GDBG_HDR_BYTES;
#if BUILD == PS2_DECKARD
        short dx = (short)deckard_read_u16((u32*)f0);
        short dy = (short)deckard_read_u16((u32*)(f0 + 2));
        short dz = (short)deckard_read_u16((u32*)(f0 + 4));
#else
        short dx = *(short*)f0;
        short dy = *(short*)(f0 + 2);
        short dz = *(short*)(f0 + 4);
#endif
        I32Dec(line, dx);
        DrawTextSimple(deckard_str_ghost_dbg_f0_dx, x, y);
        DrawTextSimple(line, x + 64, y);
        GDBG_NEXT_LINE();
        I32Dec(line, dy);
        DrawTextSimple(deckard_str_ghost_dbg_f0_dy, x, y);
        DrawTextSimple(line, x + 64, y);
        GDBG_NEXT_LINE();
        I32Dec(line, dz);
        DrawTextSimple(deckard_str_ghost_dbg_f0_dz, x, y);
        DrawTextSimple(line, x + 64, y);
        GDBG_NEXT_LINE();
    }

    inst0 = *(int*)0x80023afc;
    inst1 = *(int*)0x80023b00;
    U32Hex(line, (unsigned)inst0);
    DrawTextSimple(deckard_str_ghost_dbg_ds_23afc, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();
    U32Hex(line, (unsigned)inst1);
    DrawTextSimple(deckard_str_ghost_dbg_ds_23b00, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

    I32Dec(line, g_ghostA.currentPosition.x);
    DrawTextSimple(deckard_str_ghost_dbg_ga_posx, x, y);
    DrawTextSimple(line, x + 64, y);
    GDBG_NEXT_LINE();

#undef GDBG_NEXT_LINE
}

#else

void GhostDebugDraw(void)
{
}

#endif
