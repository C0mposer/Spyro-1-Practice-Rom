#include <common.h>
#include <custom_menu.h>
#include <collision_wireframe.h>
#include <prims.h>
#include <spin_wheel.h>

/*
 * Collision terrain viewer (Spyro 1):
 * - Runtime collision header + grid/cell walk
 * - Wireframe-first hybrid presentation: dark fills + bright outlines
 * - Near-first traversal and camera-biased culling
 * - Optional DrawWorld suppression with a dark clear color
 * - HOLD R3 spin wheel controls while the viewer is active
 */

extern VisualizerMenu visualizer_menu;
extern MenuState menu_state;

#define CW_FILL_BUDGET_NEAR 260
#define CW_FILL_BUDGET_FAR 220
#define CW_FILL_BUDGET_WIDE 160
#define CW_LINE_BUDGET_NEAR 900
#define CW_LINE_BUDGET_FAR 1600
#define CW_LINE_BUDGET_WIDE 2400
#define CW_MAX_TRIS_NEAR 1800
#define CW_MAX_TRIS_FAR 3600
#define CW_MAX_TRIS_WIDE 5600
#define CW_MAX_INDICES_PER_CELL 1024
#define CW_SEEN_WORDS 4096
#define CW_EDGE_BUCKETS 4096
#define CW_LINE_SCREEN_SKIP 8192
#define CW_SCREEN_MARGIN 64
#define CW_LINE_AREA_MIN 220
#define CW_FILL_AREA_MIN 1800
#define CW_CAMERA_BACK_DOT_CULL 0x200000
#define CW_CLEAR_R 58
#define CW_CLEAR_G 58
#define CW_CLEAR_B 58

typedef enum CwRenderMode
{
    CW_RENDER_HYBRID,
    CW_RENDER_WIREFRAME
} CwRenderMode;

typedef enum CwFilterMode
{
    CW_FILTER_ALL,
    CW_FILTER_GOOP
} CwFilterMode;

typedef enum CwRangeMode
{
    CW_RANGE_NEAR,
    CW_RANGE_FAR,
    CW_RANGE_WIDE
} CwRangeMode;

typedef struct CwScreenTri
{
    int x0;
    int y0;
    int x1;
    int y1;
    int x2;
    int y2;
    int area2;
    int area_abs;
} CwScreenTri;

static int s_fill_prims;
static int s_line_prims;
static int s_fill_budget;
static int s_line_budget;
static int s_tri_budget;
static int s_world_only_applied;
static int s_clear_color_saved;
static int s_spinwheel_registered;
static int s_spinwheel_signature;
static unsigned char s_saved_clear_color[3];
static unsigned int s_seen[CW_SEEN_WORDS];
static unsigned char s_edge_used[CW_EDGE_BUCKETS];
static short s_edge_table[CW_EDGE_BUCKETS][6];
static CwRenderMode s_render_mode = CW_RENDER_HYBRID;
static CwFilterMode s_filter_mode = CW_FILTER_ALL;
static CwRangeMode s_range_mode = CW_RANGE_NEAR;
static bool s_show_world = false;

static const u8RGB kCwLineColor = { 236, 236, 236 };
static const u8RGB kCwNormalFill = { 18, 18, 20 };
static const u8RGB kCwWaterFill = { 36, 88, 188 };
static const u8RGB kCwUnknownSpecialFill = { 92, 76, 38 };
static const u8RGB kCwInvalidSpecialFill = { 96, 34, 72 };
static const u8RGB kCwTypeFills[11] = {
    { 120, 108, 36 },  /* Sink */
    { 132, 42, 42 },   /* Hot */
    { 62, 110, 122 },  /* Spyro 1: Ice */
    { 122, 52, 108 },  /* Trigger */
    { 76, 76, 76 },    /* Spyro 1: Supercharge */
    { 76, 88, 124 },   /* Barrier */
    { 46, 104, 60 },   /* Portal */
    { 46, 72, 138 },   /* Electric */
    { 114, 82, 50 },   /* Ladder */
    { 82, 118, 56 },   /* Ramp */
    { 60, 60, 100 }    /* Slip */
};

static int CwMinInt(int a, int b) { return a < b ? a : b; }
static int CwMaxInt(int a, int b) { return a > b ? a : b; }
static int CwAbsInt(int v) { return v < 0 ? -v : v; }
static int CwMax3(int a, int b, int c) { return CwMaxInt(CwMaxInt(a, b), c); }
static int CwShellDistance(int a, int b, int c) { return CwMax3(CwAbsInt(a), CwAbsInt(b), CwAbsInt(c)); }

static int CwSin(int angle) { return SinScaled((unsigned int)angle); }
static int CwCos(int angle) { return SinScaled((unsigned int)(angle + 0x400)); }

static int CwSext9(unsigned int w, unsigned int sh)
{
    unsigned int t = (w >> sh) & 0x1ffu;
    return (int)(t << 23) >> 23;
}

static int CwIsViewerActive(void)
{
    return visualizer_menu.collision_wireframe &&
        _gameState == GAMESTATE_GAMEPLAY &&
        menu_state == MENU_HIDDEN &&
        !IsTheatreModeActive();
}

static unsigned char* CwGetClearColorPtr(void)
{
    return (unsigned char*)0x80078a50;
}

static void CwSetClearColorState(int enabled)
{
    unsigned char* clear_color = CwGetClearColorPtr();

    if (enabled) {
        if (!s_clear_color_saved) {
            s_saved_clear_color[0] = clear_color[0];
            s_saved_clear_color[1] = clear_color[1];
            s_saved_clear_color[2] = clear_color[2];
            s_clear_color_saved = 1;
        }
        clear_color[0] = CW_CLEAR_R;
        clear_color[1] = CW_CLEAR_G;
        clear_color[2] = CW_CLEAR_B;
    }
    else if (s_clear_color_saved) {
        clear_color[0] = s_saved_clear_color[0];
        clear_color[1] = s_saved_clear_color[1];
        clear_color[2] = s_saved_clear_color[2];
        s_clear_color_saved = 0;
    }
}

static void CwSetWorldOnlyState(int enabled)
{
    if (enabled) {
        if (!s_world_only_applied) {
            StopDrawWorldOnly();
            s_world_only_applied = 1;
        }
    }
    else if (s_world_only_applied) {
        RestartDrawWorldOnly();
        s_world_only_applied = 0;
    }
}

static int CwRangeCellRadius(void)
{
    switch (s_range_mode) {
        case CW_RANGE_NEAR: return 3;
        case CW_RANGE_WIDE: return 13;
        default: return 8;
    }
}

static int CwRangeWorldRadius(void)
{
    switch (s_range_mode) {
        case CW_RANGE_NEAR: return 0x4000;
        case CW_RANGE_WIDE: return 0xD000;
        default: return 0x7000;
    }
}

static int CwRangeFillRadius(void)
{
    switch (s_range_mode) {
        case CW_RANGE_NEAR: return 0x3400;
        case CW_RANGE_WIDE: return 0xA000;
        default: return 0x7000;
    }
}

static int CwFillBudget(void)
{
    if (s_render_mode == CW_RENDER_WIREFRAME) return 0;

    switch (s_range_mode) {
        case CW_RANGE_NEAR: return CW_FILL_BUDGET_NEAR;
        case CW_RANGE_WIDE: return CW_FILL_BUDGET_WIDE;
        default: return CW_FILL_BUDGET_FAR;
    }
}

static int CwLineBudget(void)
{
    int budget;

    switch (s_range_mode) {
        case CW_RANGE_NEAR: budget = CW_LINE_BUDGET_NEAR; break;
        case CW_RANGE_WIDE: budget = CW_LINE_BUDGET_WIDE; break;
        default: budget = CW_LINE_BUDGET_FAR; break;
    }

    if (s_render_mode == CW_RENDER_WIREFRAME) budget += 500;
    return budget;
}

static int CwTriBudget(void)
{
    int budget;

    switch (s_range_mode) {
        case CW_RANGE_NEAR: budget = CW_MAX_TRIS_NEAR; break;
        case CW_RANGE_WIDE: budget = CW_MAX_TRIS_WIDE; break;
        default: budget = CW_MAX_TRIS_FAR; break;
    }

    if (s_render_mode == CW_RENDER_WIREFRAME) budget += 1200;
    return budget;
}

static int CwIsBudgetExhausted(int tris_considered)
{
    if (tris_considered >= s_tri_budget) return 1;
    if (s_line_prims >= s_line_budget) return 1;
    if (s_render_mode == CW_RENDER_HYBRID && s_fill_prims >= s_fill_budget && s_line_prims >= (s_line_budget * 3) / 4) return 1;
    return 0;
}

static void CwDecodeTri(const unsigned int* t, Vec3* a, Vec3* b, Vec3* c)
{
    unsigned int xw;
    unsigned int yw;
    unsigned int zw;
    int b0;
    int b1;
    int b2;
    int m1x;
    int m1y;
    int m1z;
    int m2x;
    int m2y;
    int m2z;

    xw = t[0];
    yw = t[1];
    zw = t[2];

    b0 = (int)(xw & 0x3fffU);
    b1 = (int)(yw & 0x3fffU);
    b2 = (int)(zw & 0x3fffU);

    m1x = CwSext9(xw, 14);
    m2x = CwSext9(xw, 23);
    m1y = CwSext9(yw, 14);
    m2y = CwSext9(yw, 23);
    m1z = (int)((zw >> 16) & 0xffU);
    m2z = (int)((zw >> 24) & 0xffU);

    a->x = b0 * 0x10;
    a->y = b1 * 0x10;
    a->z = b2 * 0x10;
    b->x = (b0 + m1x) * 0x10;
    b->y = (b1 + m1y) * 0x10;
    b->z = (b2 + m1z) * 0x10;
    c->x = (b0 + m2x) * 0x10;
    c->y = (b1 + m2y) * 0x10;
    c->z = (b2 + m2z) * 0x10;
}

static unsigned short CwU16FromScreen(int v)
{
    if (v < 0) return 0;
    if (v > 65535) return 65535;
    return (unsigned short)(unsigned int)v;
}

static int CwGetSpecialType(const int* hdr, int tri_index)
{
    unsigned char* flags;
    unsigned int flag_info;
    unsigned int flag_index;

    if (hdr == 0 || tri_index < 0 || tri_index >= hdr[1]) return -1;

    flags = (unsigned char*)(unsigned)hdr[5];
    if (flags == 0) return -2;

    flag_info = flags[tri_index];
    flag_index = flag_info & 0x3fu;
    if (flag_index == 0x3fu) return -1;
    return (int)flag_index;
}

static int CwIsWaterTriangle(const unsigned int* tri_words, int tri_index)
{
    if (tri_words == 0 || tri_index < 0) return 0;
    return (tri_words[tri_index * 3 + 2] & 0x4000u) != 0;
}

static int CwPassesFilter(const int* hdr, const unsigned int* tri_words, int tri_index)
{
    int special_type;

    if (s_filter_mode == CW_FILTER_ALL) return 1;

    special_type = CwGetSpecialType(hdr, tri_index);
    return CwIsWaterTriangle(tri_words, tri_index) || special_type == 0;
}

static const u8RGB* CwGetFillColor(const int* hdr, const unsigned int* tri_words, int tri_index)
{
    int special_type;

    if (hdr == 0 || tri_words == 0 || tri_index < 0) return &kCwNormalFill;

    if (CwIsWaterTriangle(tri_words, tri_index)) return &kCwWaterFill;

    special_type = CwGetSpecialType(hdr, tri_index);
    if (special_type >= 0 && special_type < 11) return &kCwTypeFills[special_type];
    if (special_type >= 11) return &kCwUnknownSpecialFill;
    if (special_type == -2) return &kCwInvalidSpecialFill;

    return &kCwNormalFill;
}

static void CwGetCameraForward(Vec3* forward)
{
    int yaw;
    int pitch;
    int pitch_cos;

    yaw = (int)_cameraAngle.yaw;
    pitch = (int)_cameraAngle.pitch;

    forward->x = CwCos(yaw);
    forward->y = CwSin(yaw);
    forward->z = -CwSin(pitch);

    pitch_cos = CwCos(pitch);
    forward->x = (forward->x * pitch_cos) / 4096;
    forward->y = (forward->y * pitch_cos) / 4096;
}

static void CwTriangleCenter(const Vec3* p0, const Vec3* p1, const Vec3* p2, Vec3* center)
{
    center->x = (p0->x + p1->x + p2->x) / 3;
    center->y = (p0->y + p1->y + p2->y) / 3;
    center->z = (p0->z + p1->z + p2->z) / 3;
}

static int CwRangeMetric(const Vec3* point, int fx, int fy, int fz)
{
    return CwMax3(
        CwAbsInt(point->x - fx),
        CwAbsInt(point->y - fy),
        CwAbsInt(point->z - fz)
    );
}

static int CwTriangleInRange(const Vec3* center, int fx, int fy, int fz, int radius)
{
    if (radius <= 0) return 1;
    return CwRangeMetric(center, fx, fy, fz) <= radius;
}

static int CwTriangleInFrontOfCamera(const Vec3* center, const Vec3* forward)
{
    int dx;
    int dy;
    int dz;
    int dot;

    dx = center->x - _cameraPosition.x;
    dy = center->y - _cameraPosition.y;
    dz = center->z - _cameraPosition.z;
    dot = dx * forward->x + dy * forward->y + dz * forward->z;

    if (dot < -CW_CAMERA_BACK_DOT_CULL) return 0;
    return 1;
}

static int CwProjectTri(const Vec3* p0, const Vec3* p1, const Vec3* p2, CwScreenTri* out)
{
    Vec3 s0;
    Vec3 s1;
    Vec3 s2;

    WorldToScreen(&s0, (Vec3*)p0);
    WorldToScreen(&s1, (Vec3*)p1);
    WorldToScreen(&s2, (Vec3*)p2);
    if (s0.z <= 0 || s1.z <= 0 || s2.z <= 0) return 0;

    out->x0 = s0.x;
    out->y0 = s0.y;
    out->x1 = s1.x;
    out->y1 = s1.y;
    out->x2 = s2.x;
    out->y2 = s2.y;
    out->area2 = (out->x1 - out->x0) * (out->y2 - out->y0) - (out->y1 - out->y0) * (out->x2 - out->x0);
    out->area_abs = CwAbsInt(out->area2);

    if (out->area2 == 0) return 0;
    if (out->area_abs < CW_LINE_AREA_MIN) return 0;
    if (out->x0 < -CW_SCREEN_MARGIN && out->x1 < -CW_SCREEN_MARGIN && out->x2 < -CW_SCREEN_MARGIN) return 0;
    if (out->x0 > 0x200 + CW_SCREEN_MARGIN && out->x1 > 0x200 + CW_SCREEN_MARGIN && out->x2 > 0x200 + CW_SCREEN_MARGIN) return 0;
    if (out->y0 < -CW_SCREEN_MARGIN && out->y1 < -CW_SCREEN_MARGIN && out->y2 < -CW_SCREEN_MARGIN) return 0;
    if (out->y0 > 0xE8 + CW_SCREEN_MARGIN && out->y1 > 0xE8 + CW_SCREEN_MARGIN && out->y2 > 0xE8 + CW_SCREEN_MARGIN) return 0;

    return 1;
}

static void CwDrawLineScreen(int x0, int y0, int x1, int y1, const u8RGB* color)
{
    ShadedLine line;

    if (s_line_prims >= s_line_budget) return;
    if (x0 < -CW_LINE_SCREEN_SKIP && x1 < -CW_LINE_SCREEN_SKIP) return;
    if (x0 > CW_LINE_SCREEN_SKIP && x1 > CW_LINE_SCREEN_SKIP) return;
    if (y0 < -CW_LINE_SCREEN_SKIP && y1 < -CW_LINE_SCREEN_SKIP) return;
    if (y0 > CW_LINE_SCREEN_SKIP && y1 > CW_LINE_SCREEN_SKIP) return;

    line.point1Pos.x = CwU16FromScreen(x0);
    line.point1Pos.y = CwU16FromScreen(y0);
    line.point2Pos.x = CwU16FromScreen(x1);
    line.point2Pos.y = CwU16FromScreen(y1);
    line.colorP1 = *color;
    line.colorP2 = line.colorP1;
    CustomDrawLineShaded(line);
    s_line_prims++;
}

static int CwShouldDrawFill(const CwScreenTri* tri, int range_metric, int fill_radius)
{
    if (s_render_mode != CW_RENDER_HYBRID) return 0;
    if (tri->area_abs < CW_FILL_AREA_MIN) return 0;
    if (range_metric > fill_radius) return 0;
    return 1;
}

static void CwDrawFilledTriScreen(const CwScreenTri* tri, const u8RGB* color)
{
    Poly4F poly;
    PrimitiveCode shape_code;

    if (s_fill_prims >= s_fill_budget) return;

    poly.color = *color;
    poly.point1Pos.x = CwU16FromScreen(tri->x0);
    poly.point1Pos.y = CwU16FromScreen(tri->y0);
    poly.point2Pos.x = CwU16FromScreen(tri->x1);
    poly.point2Pos.y = CwU16FromScreen(tri->y1);
    poly.point3Pos.x = CwU16FromScreen(tri->x2);
    poly.point3Pos.y = CwU16FromScreen(tri->y2);
    poly.point4Pos.x = 0;
    poly.point4Pos.y = 0;

    shape_code = s_show_world ? POLY3F_TRANSPARENT : POLY3F_OPAQUE;
    CustomDrawPolygon(poly, shape_code);
    s_fill_prims++;
}

static int CwGatherCell(const int* hdr, int cx, int cy, int cz, short* out, int cap)
{
    short* ztab;
    short* ytab;
    short* xtab;
    short zoff;
    short yoff;
    unsigned short start;
    unsigned short* cells;
    int zi;
    int yi;
    int xi;
    int outn;
    unsigned int k;

    if (hdr == 0 || out == 0 || cap <= 0) return 0;
    ztab = (short*)(unsigned)hdr[2];
    if (ztab == 0) return 0;
    zi = (int)ztab[0];
    if (cz < 0 || cz >= zi) return 0;
    zoff = ztab[1 + cz];
    if (zoff < 0) return 0;
    ytab = (short*)((int)ztab + (int)zoff);
    yi = (int)ytab[0];
    if (cy < 0 || cy >= yi) return 0;
    yoff = ytab[1 + cy];
    if (yoff < 0) return 0;
    xtab = (short*)((int)ztab + (int)yoff);
    xi = (int)xtab[0];
    if (cx < 0 || cx >= xi) return 0;
    start = xtab[1 + cx];
    if ((short)start < 0) return 0;
    cells = (unsigned short*)(unsigned)hdr[3];
    if (cells == 0) return 0;
    k = (unsigned int)(unsigned short)start;
    outn = 0;
    if (outn < cap) {
        out[outn++] = (short)(cells[k++] & 0x7fff);
    }
    while (outn < cap) {
        short w = (short)cells[k];
        if (w < 0) break;
        out[outn++] = w;
        k++;
    }
    return outn;
}

static int CwSeenTry(int tri)
{
    unsigned int u;
    unsigned int w;
    unsigned int bit;

    if (tri < 0) return 0;
    u = (unsigned int)tri;
    if (u >= (unsigned int)CW_SEEN_WORDS * 32u) return 1;
    w = u >> 5;
    bit = 1u << (u & 31u);
    if (s_seen[w] & bit) return 0;
    s_seen[w] |= bit;
    return 1;
}

static void CwClearSeen(void)
{
    int i;
    for (i = 0; i < CW_SEEN_WORDS; i++) s_seen[i] = 0;
}

static int CwEdgeLess(const Vec3* a, const Vec3* b)
{
    if (a->x != b->x) return a->x < b->x;
    if (a->y != b->y) return a->y < b->y;
    return a->z < b->z;
}

static unsigned int CwHashEdgeKey(const short* key)
{
    unsigned int h;
    int i;

    h = 2166136261u;
    for (i = 0; i < 6; i++) {
        h ^= (unsigned int)(unsigned short)key[i];
        h *= 16777619u;
    }
    return h;
}

static int CwEdgeTry(const Vec3* a, const Vec3* b)
{
    const Vec3* p0;
    const Vec3* p1;
    short key[6];
    unsigned int slot;
    int probe;
    int i;

    p0 = a;
    p1 = b;
    if (CwEdgeLess(p1, p0)) {
        const Vec3* t = p0;
        p0 = p1;
        p1 = t;
    }

    key[0] = (short)(p0->x >> 4);
    key[1] = (short)(p0->y >> 4);
    key[2] = (short)(p0->z >> 4);
    key[3] = (short)(p1->x >> 4);
    key[4] = (short)(p1->y >> 4);
    key[5] = (short)(p1->z >> 4);

    slot = CwHashEdgeKey(key) & (CW_EDGE_BUCKETS - 1u);
    for (probe = 0; probe < CW_EDGE_BUCKETS; probe++) {
        if (!s_edge_used[slot]) {
            s_edge_used[slot] = 1;
            for (i = 0; i < 6; i++) s_edge_table[slot][i] = key[i];
            return 1;
        }
        for (i = 0; i < 6; i++) {
            if (s_edge_table[slot][i] != key[i]) break;
        }
        if (i == 6) return 0;
        slot = (slot + 1u) & (CW_EDGE_BUCKETS - 1u);
    }

    return 1;
}

static void CwClearEdges(void)
{
    int i;
    for (i = 0; i < CW_EDGE_BUCKETS; i++) s_edge_used[i] = 0;
}

static const char* CwSpinFilterLabel(void)
{
    return s_filter_mode == CW_FILTER_ALL ? "SHOW GOOP" : "SHOW ALL COLL";
}

static const char* CwSpinWorldLabel(void)
{
    return s_show_world ? "HIDE WORLD" : "SHOW WORLD";
}

static const char* CwSpinRenderLabel(void)
{
    return s_render_mode == CW_RENDER_HYBRID ? "VIEW WIREFR" : "VIEW HYBRID";
}

static const char* CwSpinRangeLabel(void)
{
    switch (s_range_mode) {
        case CW_RANGE_NEAR: return "RANGE NEAR";
        case CW_RANGE_WIDE: return "RANGE WIDE";
        default: return "RANGE FAR";
    }
}

static void CwSpinToggleFilter(void)
{
    s_filter_mode = (s_filter_mode == CW_FILTER_ALL) ? CW_FILTER_GOOP : CW_FILTER_ALL;
}

static void CwSpinToggleWorld(void)
{
    s_show_world = !s_show_world;
}

static void CwSpinToggleRender(void)
{
    s_render_mode = (s_render_mode == CW_RENDER_HYBRID) ? CW_RENDER_WIREFRAME : CW_RENDER_HYBRID;
}

static void CwSpinCycleRange(void)
{
    if (s_range_mode == CW_RANGE_WIDE)
        s_range_mode = CW_RANGE_NEAR;
    else
        s_range_mode = (CwRangeMode)(s_range_mode + 1);
}

void CollisionWireframeSpinWheelSync(void)
{
    int active;
    int signature;

    active = CwIsViewerActive();
    if (!active) {
        if (s_spinwheel_registered) {
            SpinWheelClearOptions();
            s_spinwheel_registered = 0;
            s_spinwheel_signature = -1;
        }
        return;
    }

    signature = (int)s_filter_mode |
        ((int)s_render_mode << 4) |
        ((int)s_show_world << 8) |
        ((int)s_range_mode << 12);

    if (s_spinwheel_registered && signature == s_spinwheel_signature) return;

    SpinWheelClearOptions();
    SpinWheelAddOption(CwSpinRangeLabel(), CwSpinCycleRange);
    SpinWheelAddOption(CwSpinFilterLabel(), CwSpinToggleFilter);
    SpinWheelAddOption(CwSpinWorldLabel(), CwSpinToggleWorld);
    SpinWheelAddOption(CwSpinRenderLabel(), CwSpinToggleRender);
    s_spinwheel_registered = 1;
    s_spinwheel_signature = signature;
}

void CollisionWireframeUpdate(void)
{
    int* hdr;
    unsigned int* twords;
    int active;
    int hide_world;
    short* ztab;
    unsigned int tmax;
    int zcount;
    int focus_x;
    int focus_y;
    int focus_z;
    int focus_cell_x;
    int focus_cell_y;
    int focus_cell_z;
    int range_cells;
    int range_world;
    int range_fill;
    Vec3 camera_forward;
    int shell;
    int dx;
    int dy;
    int dz;
    int tris_considered;
    short idxbuf[CW_MAX_INDICES_PER_CELL];

    active = CwIsViewerActive();
    hide_world = active && !s_show_world;
    CwSetWorldOnlyState(hide_world);
    CwSetClearColorState(hide_world);
    if (!active) return;

    hdr = _ptr_ptr_moving_collision;
    if (hdr == 0) return;
    twords = (unsigned int*)(unsigned)hdr[4];
    ztab = (short*)(unsigned)hdr[2];
    if (twords == 0 || ztab == 0) return;
    zcount = (int)ztab[0];
    if (zcount <= 0) return;
    tmax = (unsigned int)hdr[0];
    if (tmax == 0u || tmax > 200000u) return;

    focus_x = _cameraPosition.x;
    focus_y = _cameraPosition.y;
    focus_z = _cameraPosition.z;
    focus_cell_x = focus_x >> 12;
    focus_cell_y = focus_y >> 12;
    focus_cell_z = focus_z >> 12;
    range_cells = CwRangeCellRadius();
    range_world = CwRangeWorldRadius();
    range_fill = CwRangeFillRadius();
    CwGetCameraForward(&camera_forward);

    s_fill_prims = 0;
    s_line_prims = 0;
    s_fill_budget = CwFillBudget();
    s_line_budget = CwLineBudget();
    s_tri_budget = CwTriBudget();
    tris_considered = 0;
    CwClearSeen();
    CwClearEdges();

    for (shell = 0; shell <= range_cells; shell++) {
        for (dz = -shell; dz <= shell; dz++) {
            int uz = focus_cell_z + dz;
            short zoff;
            short* ytab;
            int ycount;

            if (uz < 0 || uz >= zcount) continue;
            zoff = ztab[1 + uz];
            if (zoff < 0) continue;

            ytab = (short*)((int)ztab + (int)zoff);
            ycount = (int)ytab[0];

            for (dy = -shell; dy <= shell; dy++) {
                int uy = focus_cell_y + dy;
                short yoff2;
                int xcount;
                short* xtab;

                if (uy < 0 || uy >= ycount) continue;
                yoff2 = ytab[1 + uy];
                if (yoff2 < 0) continue;

                xtab = (short*)((int)ztab + (int)yoff2);
                xcount = (int)xtab[0];

                for (dx = -shell; dx <= shell; dx++) {
                    int ux = focus_cell_x + dx;
                    int n;
                    int j;

                    if (CwShellDistance(dx, dy, dz) != shell) continue;
                    if ((dx * dx) + (dy * dy) + (dz * dz) > (range_cells * range_cells)) continue;
                    if (ux < 0 || ux >= xcount) continue;
                    if (CwIsBudgetExhausted(tris_considered)) return;

                    n = CwGatherCell(hdr, ux, uy, uz, idxbuf, CW_MAX_INDICES_PER_CELL);
                    for (j = 0; j < n; j++) {
                        int ti = idxbuf[j];
                        CwScreenTri tri;
                        Vec3 p0;
                        Vec3 p1;
                        Vec3 p2;
                        Vec3 center;
                        int range_metric;
                        const u8RGB* fill_color;

                        if (CwIsBudgetExhausted(tris_considered)) return;
                        if (ti < 0) continue;
                        if ((unsigned int)ti >= tmax) continue;
                        if (!CwSeenTry(ti)) continue;

                        CwDecodeTri(&twords[ti * 3u], &p0, &p1, &p2);
                        if (!CwPassesFilter(hdr, twords, ti)) continue;

                        CwTriangleCenter(&p0, &p1, &p2, &center);
                        range_metric = CwRangeMetric(&center, focus_x, focus_y, focus_z);
                        if (!CwTriangleInRange(&center, focus_x, focus_y, focus_z, range_world)) continue;
                        if (!CwTriangleInFrontOfCamera(&center, &camera_forward)) continue;
                        if (!CwProjectTri(&p0, &p1, &p2, &tri)) continue;

                        tris_considered++;
                        fill_color = CwGetFillColor(hdr, twords, ti);

                        if (CwShouldDrawFill(&tri, range_metric, range_fill)) {
                            CwDrawFilledTriScreen(&tri, fill_color);
                        }

                        if (CwEdgeTry(&p0, &p1)) CwDrawLineScreen(tri.x0, tri.y0, tri.x1, tri.y1, &kCwLineColor);
                        if (CwEdgeTry(&p1, &p2)) CwDrawLineScreen(tri.x1, tri.y1, tri.x2, tri.y2, &kCwLineColor);
                        if (CwEdgeTry(&p2, &p0)) CwDrawLineScreen(tri.x2, tri.y2, tri.x0, tri.y0, &kCwLineColor);
                    }
                }
            }
        }
    }
}
