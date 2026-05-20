#include <types.h>
#include <deckard_strings.h>

typedef struct VisualizerMenu
{
    int selection;
    char* show_sparx_range_text;
    int free_cam;
    char* free_cam_text;
    char* theatre_mode_text;
    char* memory_watch_text;
    int hitbox_viewer;
    char* hitbox_viewer_text;
    int collision_wireframe;
    char* collision_wireframe_text;
} VisualizerMenu;

extern VisualizerMenu visualizer_menu;
extern int _currentButtonOneFrame;
void DrawMenuItem(const char* text, int menu_number, int currently_selected_menu, int init_size);

#define HITBOX_LEFT_BUTTON 0x8000
#define HITBOX_RIGHT_BUTTON 0x2000

typedef struct {
    int x, y, z;
} Vec3;

typedef struct {
    void* ptr_mobyVarData;      // 0x00
    int   nextCollisionLink;    // 0x04
    void* ptr_colision;         // 0x08
    Vec3  position;             // 0x0C
    int   interactState;        // 0x18
    int   UNK2;                 // 0x1C
    int   UNK3;                 // 0x20
    int   UNK4;                 // 0x24
    int   UNK5;                 // 0x28
    int   UNK6;                 // 0x2C
    int   UNK7;                 // 0x30
    short subType;              // 0x34
    short type;                 // 0x36
    short UNK10;                // 0x38
    byte  canDropObject;        // 0x3A
    byte  UNK21;                // 0x3B
    byte  currentAnimation;     // 0x3C
    byte  nextAnimation;        // 0x3D
    byte  currentKeyFrame;      // 0x3E
    byte  nextKeyFrame;         // 0x3F
    byte  speedRelated;         // 0x40
    byte  animationSpeed;       // 0x41
    byte  UNK13;                // 0x42
    byte  active;               // 0x43
    byte  rotX;                 // 0x44
    byte  rotY;                 // 0x45
    byte  rotZ;                 // 0x46
    byte  requiredHUD1;         // 0x47
    byte  state;                // 0x48
    byte  subState;             // 0x49
    byte  visible;              // 0x4A
    byte  shadowRelated;        // 0x4B
    byte  r;                    // 0x4C
    byte  g;                    // 0x4D
    byte  b;                    // 0x4E
    byte  color;                // 0x4F
    short requiredHUD2;         // 0x50
    byte  UNK19;                // 0x52
    byte  heldValue;            // 0x53
    short UNK20;                // 0x54
    short scale;                // 0x56
} Moby_Copy;

extern Moby_Copy* ptr_startOfCurrentLevelMobys;
extern byte* ptrPrimitiveAlloc;
extern Vec3  _spyro;
extern int   maybe_ptr_models[];

void UpdateMobyBins(void);
void WorldToScreen(int* screenOut, int* worldIn);
void DrawPrimitive(void* prim);

#define COLLISION_FLAG_LAST       0x01
#define COLLISION_FLAG_DONT_PUSH  0x02
#define COLLISION_TYPE_SPHERE     0
#define COLLISION_TYPE_SPHERE_Z   1
#define COLLISION_TYPE_PILL       2
#define COLLISION_TYPE_XFORM      3

#define CIRCLE_SEGS      16
#define MAX_HITBOXES     48
#define MAX_MOBY_SCAN    300
#define DRAW_DIST        10000
#define SPYRO_RADIUS     0x164
#define PRIM_BUDGET      8192

/* Instead of storing as shorts, Store as ints. The Deckard build
whole-file byteswap for the .text section, corrupts the shorts.
Q12 (4096=1.0). */
static const int g_cos16[CIRCLE_SEGS] = {
    4096, 3784, 2896, 1567, 0, -1567, -2896, -3784,
    -4096, -3784, -2896, -1567, 0, 1567, 2896, 3784
};
static const int g_sin16[CIRCLE_SEGS] = {
    0, 1567, 2896, 3784, 4096, 3784, 2896, 1567,
    0, -1567, -2896, -3784, -4096, -3784, -2896, -1567
};

static byte* g_primStart;

static void DrawWorldRing(Vec3* center, int radius, int axis, u8 cr, u8 cg, u8 cb) {
    int px[CIRCLE_SEGS], py[CIRCLE_SEGS], ok[CIRCLE_SEGS];

    for (int i = 0; i < CIRCLE_SEGS; i++) {
        int c = (radius * g_cos16[i]) >> 12;
        int s = (radius * g_sin16[i]) >> 12;
        Vec3 p = *center;

        if (axis == 0) { p.x += c; p.y += s; }
        else if (axis == 1) { p.x += c; p.z += s; }
        else { p.y += c; p.z += s; }

        int scr[3];
        WorldToScreen(scr, (int*)&p);
        px[i] = scr[0];
        py[i] = scr[1];
        ok[i] = (scr[2] > 64);
    }

    for (int i = 0; i < CIRCLE_SEGS; i++) {
        int ni = (i + 1) & (CIRCLE_SEGS - 1);
        if (!ok[i] || !ok[ni]) continue;

        int x0 = px[i], y0 = py[i];
        int x1 = px[ni], y1 = py[ni];

        if (x0 < -512 || x0 > 512 || y0 < -512 || y0 > 512) continue;
        if (x1 < -512 || x1 > 512 || y1 < -512 || y1 > 512) continue;

        if (ptrPrimitiveAlloc - g_primStart >= PRIM_BUDGET) return;

        u32* prim = (u32*)ptrPrimitiveAlloc;
        ptrPrimitiveAlloc += 16;

        prim[0] = (3u << 24);
        prim[1] = (u32)cr | ((u32)cg << 8) | ((u32)cb << 16) | (0x40u << 24);
        prim[2] = (u32)(u16)x0 | ((u32)(u16)y0 << 16);
        prim[3] = (u32)(u16)x1 | ((u32)(u16)y1 << 16);

        DrawPrimitive(prim);
    }
}

static void DrawSphere(Vec3* center, int radius, u8 cr, u8 cg, u8 cb) {
    DrawWorldRing(center, radius, 0, cr, cg, cb);
    DrawWorldRing(center, radius, 1, cr, cg, cb);
    DrawWorldRing(center, radius, 2, cr, cg, cb);
}

static void DrawVerticalHalfRing(Vec3* center, int radius, int axis, int top, u8 cr, u8 cg, u8 cb) {
    int half = CIRCLE_SEGS / 2;
    int base_i = top ? 0 : half;
    int px[CIRCLE_SEGS / 2 + 1], py[CIRCLE_SEGS / 2 + 1], ok[CIRCLE_SEGS / 2 + 1];

    for (int i = 0; i <= half; i++) {
        int idx = (base_i + i) & (CIRCLE_SEGS - 1);
        int c = (radius * g_cos16[idx]) >> 12;
        int s = (radius * g_sin16[idx]) >> 12;
        Vec3 p = *center;

        if (axis == 1) { p.x += c; p.z += s; }
        else { p.y += c; p.z += s; }

        int scr[3];
        WorldToScreen(scr, (int*)&p);
        px[i] = scr[0]; py[i] = scr[1]; ok[i] = (scr[2] > 64);
    }

    for (int i = 0; i < half; i++) {
        if (!ok[i] || !ok[i + 1]) continue;
        int x0 = px[i], y0 = py[i], x1 = px[i + 1], y1 = py[i + 1];
        if (x0 < -512 || x0 > 512 || y0 < -512 || y0 > 512) continue;
        if (x1 < -512 || x1 > 512 || y1 < -512 || y1 > 512) continue;
        if (ptrPrimitiveAlloc - g_primStart >= PRIM_BUDGET) return;

        u32* prim = (u32*)ptrPrimitiveAlloc;
        ptrPrimitiveAlloc += 16;
        prim[0] = (3u << 24);
        prim[1] = (u32)cr | ((u32)cg << 8) | ((u32)cb << 16) | (0x40u << 24);
        prim[2] = (u32)(u16)x0 | ((u32)(u16)y0 << 16);
        prim[3] = (u32)(u16)x1 | ((u32)(u16)y1 << 16);
        DrawPrimitive(prim);
    }
}

static void DrawPill(Vec3* base, int radius, int z_bot, int z_top, u8 cr, u8 cg, u8 cb) {
    Vec3 bot = *base; bot.z += z_bot;
    Vec3 top = *base; top.z += z_top;

    DrawWorldRing(&bot, radius, 0, cr, cg, cb);
    DrawWorldRing(&top, radius, 0, cr, cg, cb);

    DrawVerticalHalfRing(&bot, radius, 1, 0, cr, cg, cb);
    DrawVerticalHalfRing(&bot, radius, 2, 0, cr, cg, cb);
    DrawVerticalHalfRing(&top, radius, 1, 1, cr, cg, cb);
    DrawVerticalHalfRing(&top, radius, 2, 1, cr, cg, cb);

    for (int i = 0; i < CIRCLE_SEGS; i += 2) {
        int c = (radius * g_cos16[i]) >> 12;
        int s = (radius * g_sin16[i]) >> 12;

        Vec3 pb = bot; pb.x += c; pb.y += s;
        Vec3 pt = top; pt.x += c; pt.y += s;

        int sb[3], st[3];
        WorldToScreen(sb, (int*)&pb);
        WorldToScreen(st, (int*)&pt);

        if (sb[2] <= 64 || st[2] <= 64) continue;
        if (sb[0] < -512 || sb[0] > 512 || sb[1] < -512 || sb[1] > 512) continue;
        if (st[0] < -512 || st[0] > 512 || st[1] < -512 || st[1] > 512) continue;

        if (ptrPrimitiveAlloc - g_primStart >= PRIM_BUDGET) return;

        u32* prim = (u32*)ptrPrimitiveAlloc;
        ptrPrimitiveAlloc += 16;

        prim[0] = (3u << 24);
        prim[1] = (u32)cr | ((u32)cg << 8) | ((u32)cb << 16) | (0x40u << 24);
        prim[2] = (u32)(u16)sb[0] | ((u32)(u16)sb[1] << 16);
        prim[3] = (u32)(u16)st[0] | ((u32)(u16)st[1] << 16);

        DrawPrimitive(prim);
    }
}

static void GetXformCenter(Moby_Copy* m, Vec3* out, int vertex_id) {
    int model_base = maybe_ptr_models[(u16)m->type];
    if (model_base == 0) { *out = m->position; return; }

    int frame_data = *(int*)(model_base + (int)m->currentAnimation * 4 + 0x38);
    if (frame_data == 0) { *out = m->position; return; }

    int packed_ptr_raw = *(int*)(frame_data + (int)m->currentKeyFrame * 8 + 0x24);
    int scale = *((byte*)(frame_data + 5));
    int* vert_array = (int*)(((u32)packed_ptr_raw & 0x001FFFFFu) | 0x80000000u);
    int packed = vert_array[vertex_id];

    int ca = packed >> 21;
    int cb = (packed << 10) >> 21;
    int cc = (packed << 20) >> 19;
    ca <<= scale;
    cb <<= scale;
    cc <<= scale;

    short R11 = (short)m->UNK3;        short R12 = (short)(m->UNK3 >> 16);
    short R13 = (short)m->UNK4;        short R21 = (short)(m->UNK4 >> 16);
    short R22 = (short)m->UNK5;        short R23 = (short)(m->UNK5 >> 16);
    short R31 = (short)m->UNK6;        short R32 = (short)(m->UNK6 >> 16);
    short R33 = (short)m->UNK7;

    int ir1 = ((int)R11 * cb + (int)R12 * cc + (int)R13 * ca) >> 12;
    int ir2 = ((int)R21 * cb + (int)R22 * cc + (int)R23 * ca) >> 12;
    int ir3 = ((int)R31 * cb + (int)R32 * cc + (int)R33 * ca) >> 12;

    out->x = m->position.x + ir3;
    out->y = m->position.y - ir1;
    out->z = m->position.z - ir2;
}

static void DrawMobyHitSpheres(Moby_Copy* m) {
    u16* col = (u16*)m->ptr_colision;
    if (col == 0) return;
    if (((u32)col & 0x80000000) == 0) return;

    s16 model_radius = (s16)col[0];
    if (model_radius <= 0) return;

    byte* entry = (byte*)col + 4;

    for (int e = 0; e < 8; e++) {
        u8 flags = entry[0];
        u8 type = entry[1];
        s16 col_radius = *(s16*)(entry + 2);

        int entry_size = 4;
        if (type == COLLISION_TYPE_SPHERE_Z ||
            type == COLLISION_TYPE_XFORM)
            entry_size = 12;
        else if (type == COLLISION_TYPE_PILL)
            entry_size = 16;

        if (col_radius <= 0) goto skip_draw;

        {
            Vec3 sphere_center = m->position;

            switch (type) {
                case COLLISION_TYPE_SPHERE:
                break;

                case COLLISION_TYPE_SPHERE_Z:
                sphere_center.z += *(s16*)(entry + 6);
                break;

                case COLLISION_TYPE_PILL: {
                    s16 tube_bot = *(s16*)(entry + 4);
                    s16 tube_top = *(s16*)(entry + 6);
                    u8 pr, pg, pb;
                    if (flags & COLLISION_FLAG_DONT_PUSH) { pr = 50; pg = 50; pb = 160; }
                    else { pr = 255; pg = 50; pb = 50; }
                    DrawPill(&m->position, col_radius, tube_bot, tube_top, pr, pg, pb);
                    goto skip_draw;
                }

                case COLLISION_TYPE_XFORM: {
                    u16 vid = *(u16*)(entry + 4);
                    GetXformCenter(m, &sphere_center, vid);
                    break;
                }
            }

            u8 rc, gc, bc;
            if (flags & COLLISION_FLAG_DONT_PUSH) { rc = 50; gc = 50; bc = 160; }
            else { rc = 255; gc = 50; bc = 50; }
            DrawSphere(&sphere_center, col_radius, rc, gc, bc);
        }

    skip_draw:
        if (flags & COLLISION_FLAG_LAST) break;
        entry += entry_size;
    }
}

void DrawSpyroHitSphere(void)
{
    DrawSphere((Vec3*)&_spyro, SPYRO_RADIUS, 50, 220, 50);
}

void HitboxViewerMenuUpdate(void)
{
    if (visualizer_menu.hitbox_viewer_text == 0)
    {
        visualizer_menu.hitbox_viewer_text = (char*)deckard_str_hitbox_menu_off;
    }

    DrawMenuItem(visualizer_menu.hitbox_viewer_text, 3, visualizer_menu.selection, 42);
    if (visualizer_menu.selection == 3 && (_currentButtonOneFrame == HITBOX_RIGHT_BUTTON || _currentButtonOneFrame == HITBOX_LEFT_BUTTON))
    {
        visualizer_menu.hitbox_viewer = !visualizer_menu.hitbox_viewer;
    }

    visualizer_menu.hitbox_viewer_text = (char*)(visualizer_menu.hitbox_viewer ? deckard_str_hitbox_menu_on : deckard_str_hitbox_menu_off);
}

void HitboxViewerUpdate(void) {
    if (ptr_startOfCurrentLevelMobys == 0) return;

    g_primStart = ptrPrimitiveAlloc;

    DrawSpyroHitSphere();

    Moby_Copy* arr = ptr_startOfCurrentLevelMobys;
    int drawn = 0;

    for (int i = 0; i < MAX_MOBY_SCAN && drawn < MAX_HITBOXES; i++) {
        Moby_Copy* m = &arr[i];
        if (m->state == 0xFD) continue;
        if (m->ptr_colision == 0) continue;
        if (m->visible == 0) continue;

        int dx = m->position.x - _spyro.x;
        int dy = m->position.y - _spyro.y;
        int dz = m->position.z - _spyro.z;
        if (dx > DRAW_DIST || dx < -DRAW_DIST) continue;
        if (dy > DRAW_DIST || dy < -DRAW_DIST) continue;
        if (dz > DRAW_DIST || dz < -DRAW_DIST) continue;

        DrawMobyHitSpheres(m);
        drawn++;
    }
}

// void Hook_GameUpdateLoop(void) { // we need a specific hook point for the hitbox's to look good. Maybe 
//     UpdateMobyBins();
//     if (true)
//         HitboxViewerUpdate();
// }
