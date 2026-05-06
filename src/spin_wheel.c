#include <common.h>
#include <deckard_strings.h>
#include <font.h>
#include <prims.h>
#include <spin_wheel.h>

#define SPIN_WHEEL_MAX_OPTIONS 12
#define SPIN_WHEEL_HOLD_FRAMES SECONDS
#define SPIN_WHEEL_OPEN_FRAMES 14
#define SPIN_WHEEL_CLOSE_FRAMES 6
#define SPIN_WHEEL_SELECT_FRAMES 12
#define SPIN_WHEEL_SNAP_FRAMES 5
#define SPIN_WHEEL_HOLD_VISUAL_DELAY 5
#define SPIN_WHEEL_BASE_RADIUS 70
#define SPIN_WHEEL_INNER_RADIUS 17
#define SPIN_WHEEL_LABEL_RADIUS 82
#define SPIN_WHEEL_CENTER_X 0x100
#define SPIN_WHEEL_CENTER_Y 0x74
#define SPIN_WHEEL_STICK_DEADZONE 38
#define SPIN_WHEEL_TRIG_SCALE 4096
#define SPIN_WHEEL_FULL_CIRCLE 0x1000
#define SPIN_WHEEL_EASE_SCALE 4096

typedef enum SpinWheelState
{
    SPIN_WHEEL_CLOSED,
    SPIN_WHEEL_HOLDING,
    SPIN_WHEEL_OPENING,
    SPIN_WHEEL_OPEN,
    SPIN_WHEEL_SELECTING,
    SPIN_WHEEL_CANCELING
} SpinWheelState;

typedef struct SpinWheelOption
{
    const char* label;
    SpinWheelCallback callback;
} SpinWheelOption;

static SpinWheelOption spin_wheel_options[SPIN_WHEEL_MAX_OPTIONS] = { 0 };
static int spin_wheel_option_count = 0;
static SpinWheelState spin_wheel_state = SPIN_WHEEL_CLOSED;
static int spin_wheel_hold_frames = 0;
static int spin_wheel_anim_frame = 0;
static int spin_wheel_selected_index = -1;
static int spin_wheel_pending_index = -1;
static int spin_wheel_snap_frame = 0;
static bool spin_wheel_test_registered = false;
static const char* spin_wheel_message = NULL;
static int spin_wheel_message_timer = 0;

static int SpinWheelStringWidth(const char* str)
{
    int len = 0;
    while (str != NULL && str[len] != '\0')
    {
        len++;
    }
    return len * 7;
}

static int SpinWheelAbs(int value)
{
    return value < 0 ? -value : value;
}

static int SpinWheelSin(int angle)
{
    return SinScaled((unsigned int)(angle & (SPIN_WHEEL_FULL_CIRCLE - 1)));
}

static int SpinWheelCos(int angle)
{
    return SinScaled((unsigned int)((angle + 0x400) & (SPIN_WHEEL_FULL_CIRCLE - 1)));
}

static int SpinWheelEaseOut(int frame, int total)
{
    int t;
    int inv;
    if (frame <= 0) return 0;
    if (frame >= total) return SPIN_WHEEL_EASE_SCALE;

    t = (frame * SPIN_WHEEL_EASE_SCALE) / total;
    inv = SPIN_WHEEL_EASE_SCALE - t;
    return SPIN_WHEEL_EASE_SCALE - ((inv * inv) / SPIN_WHEEL_EASE_SCALE);
}

static int SpinWheelEaseIn(int frame, int total)
{
    int t;
    if (frame <= 0) return 0;
    if (frame >= total) return SPIN_WHEEL_EASE_SCALE;

    t = (frame * SPIN_WHEEL_EASE_SCALE) / total;
    return (t * t) / SPIN_WHEEL_EASE_SCALE;
}

static void SpinWheelDrawLine(int x1, int y1, int x2, int y2, u8 r, u8 g, u8 b)
{
    ShadedLine line = { 0 };
    line.point1Pos.x = (u16)x1;
    line.point1Pos.y = (u16)y1;
    line.point2Pos.x = (u16)x2;
    line.point2Pos.y = (u16)y2;
    line.colorP1.R = r;
    line.colorP1.G = g;
    line.colorP1.B = b;
    line.colorP2.R = r;
    line.colorP2.G = g;
    line.colorP2.B = b;
    CustomDrawLineShaded(line);
}

static void SpinWheelDrawCircle(int radius, u8 r, u8 g, u8 b)
{
    int segments = 32;
    int last_x = SPIN_WHEEL_CENTER_X;
    int last_y = SPIN_WHEEL_CENTER_Y - radius;

    for (int i = 1; i <= segments; i++)
    {
        int angle = (i * SPIN_WHEEL_FULL_CIRCLE) / segments;
        int x = SPIN_WHEEL_CENTER_X + ((SpinWheelSin(angle) * radius) / SPIN_WHEEL_TRIG_SCALE);
        int y = SPIN_WHEEL_CENTER_Y - ((SpinWheelCos(angle) * radius) / SPIN_WHEEL_TRIG_SCALE);
        SpinWheelDrawLine(last_x, last_y, x, y, r, g, b);
        last_x = x;
        last_y = y;
    }
}

static void SpinWheelDrawCircleAt(int cx, int cy, int radius, u8 r, u8 g, u8 b)
{
    int segments = 16;
    int last_x = cx;
    int last_y = cy - radius;

    for (int i = 1; i <= segments; i++)
    {
        int angle = (i * SPIN_WHEEL_FULL_CIRCLE) / segments;
        int x = cx + ((SpinWheelSin(angle) * radius) / SPIN_WHEEL_TRIG_SCALE);
        int y = cy - ((SpinWheelCos(angle) * radius) / SPIN_WHEEL_TRIG_SCALE);
        SpinWheelDrawLine(last_x, last_y, x, y, r, g, b);
        last_x = x;
        last_y = y;
    }
}

static void SpinWheelDrawArc(int radius, int start_angle, int end_angle, u8 r, u8 g, u8 b)
{
    int segments = 10;
    int last_x = SPIN_WHEEL_CENTER_X + ((SpinWheelSin(start_angle) * radius) / SPIN_WHEEL_TRIG_SCALE);
    int last_y = SPIN_WHEEL_CENTER_Y - ((SpinWheelCos(start_angle) * radius) / SPIN_WHEEL_TRIG_SCALE);

    for (int i = 1; i <= segments; i++)
    {
        int angle = start_angle + (((end_angle - start_angle) * i) / segments);
        int x = SPIN_WHEEL_CENTER_X + ((SpinWheelSin(angle) * radius) / SPIN_WHEEL_TRIG_SCALE);
        int y = SPIN_WHEEL_CENTER_Y - ((SpinWheelCos(angle) * radius) / SPIN_WHEEL_TRIG_SCALE);
        SpinWheelDrawLine(last_x, last_y, x, y, r, g, b);
        last_x = x;
        last_y = y;
    }
}

static void SpinWheelDrawBandPiece(int a1, int a2, int inner_radius, int outer_radius, u8 r, u8 g, u8 b)
{
    Poly4F poly = { 0 };

    poly.color.R = r;
    poly.color.G = g;
    poly.color.B = b;

    poly.point1Pos.x = (u16)(SPIN_WHEEL_CENTER_X + ((SpinWheelSin(a1) * inner_radius) / SPIN_WHEEL_TRIG_SCALE));
    poly.point1Pos.y = (u16)(SPIN_WHEEL_CENTER_Y - ((SpinWheelCos(a1) * inner_radius) / SPIN_WHEEL_TRIG_SCALE));
    poly.point2Pos.x = (u16)(SPIN_WHEEL_CENTER_X + ((SpinWheelSin(a1) * outer_radius) / SPIN_WHEEL_TRIG_SCALE));
    poly.point2Pos.y = (u16)(SPIN_WHEEL_CENTER_Y - ((SpinWheelCos(a1) * outer_radius) / SPIN_WHEEL_TRIG_SCALE));
    poly.point3Pos.x = (u16)(SPIN_WHEEL_CENTER_X + ((SpinWheelSin(a2) * outer_radius) / SPIN_WHEEL_TRIG_SCALE));
    poly.point3Pos.y = (u16)(SPIN_WHEEL_CENTER_Y - ((SpinWheelCos(a2) * outer_radius) / SPIN_WHEEL_TRIG_SCALE));
    poly.point4Pos.x = (u16)(SPIN_WHEEL_CENTER_X + ((SpinWheelSin(a2) * inner_radius) / SPIN_WHEEL_TRIG_SCALE));
    poly.point4Pos.y = (u16)(SPIN_WHEEL_CENTER_Y - ((SpinWheelCos(a2) * inner_radius) / SPIN_WHEEL_TRIG_SCALE));

    CustomDrawPolygon(poly, POLY4F_TRANSPARENT);
}

static void SpinWheelDrawBandSegment(int center_angle, int half_step, int inner_radius, int outer_radius, u8 r, u8 g, u8 b)
{
    int segments = 5;
    int start_angle = center_angle - half_step;
    int end_angle = center_angle + half_step;

    for (int i = 0; i < segments; i++)
    {
        int a1 = start_angle + (((end_angle - start_angle) * i) / segments);
        int a2 = start_angle + (((end_angle - start_angle) * (i + 1)) / segments);
        SpinWheelDrawBandPiece(a1, a2, inner_radius, outer_radius, r, g, b);
    }
}

static int SpinWheelSelectedPop(void)
{
    int frame;
    int pop;

    if (spin_wheel_state != SPIN_WHEEL_SELECTING) return 0;

    frame = spin_wheel_anim_frame;
    if (frame < 4) return frame * 5;
    if (frame < 8) return 20 - ((frame - 4) * 3);

    pop = 10 - ((frame - 8) * 3);
    return pop > 0 ? pop : 0;
}

static int SpinWheelSelectionSnap(void)
{
    int inv;

    if (spin_wheel_snap_frame <= 0) return 0;

    inv = SPIN_WHEEL_SNAP_FRAMES - spin_wheel_snap_frame;
    if (inv < 0) inv = 0;
    return 8 - (inv * 2);
}

static void SpinWheelDrawStickCursor(int wheel_radius)
{
    int rx;
    int ry;
    int abs_rx;
    int abs_ry;
    int max_axis;
    int min_axis;
    int approx_magnitude;
    int track_radius;
    int cursor_x;
    int cursor_y;
    int cursor_radius;
    int pulse;

    if (spin_wheel_state != SPIN_WHEEL_OPENING && spin_wheel_state != SPIN_WHEEL_OPEN) return;

    rx = (int)_rightAnalogStick.x_axis - 128;
    ry = (int)_rightAnalogStick.y_axis - 128;
    abs_rx = SpinWheelAbs(rx);
    abs_ry = SpinWheelAbs(ry);
    max_axis = abs_rx > abs_ry ? abs_rx : abs_ry;
    min_axis = abs_rx > abs_ry ? abs_ry : abs_rx;
    approx_magnitude = max_axis + (min_axis / 2);

    if (approx_magnitude > 128 && approx_magnitude > 0)
    {
        rx = (rx * 128) / approx_magnitude;
        ry = (ry * 128) / approx_magnitude;
        approx_magnitude = 128;
    }

    track_radius = wheel_radius - 10;
    if (track_radius < SPIN_WHEEL_INNER_RADIUS + 4) track_radius = SPIN_WHEEL_INNER_RADIUS + 4;

    cursor_x = SPIN_WHEEL_CENTER_X + ((rx * track_radius) / 128);
    cursor_y = SPIN_WHEEL_CENTER_Y + ((ry * track_radius) / 128);
    pulse = (spin_wheel_anim_frame % 10) < 5 ? 1 : 0;
    cursor_radius = 3 + ((approx_magnitude * 3) / 128) + pulse;

    if ((rx * rx) + (ry * ry) < (SPIN_WHEEL_STICK_DEADZONE * SPIN_WHEEL_STICK_DEADZONE))
    {
        SpinWheelDrawCircleAt(cursor_x, cursor_y, cursor_radius + 2, 0x30, 0x18, 0x58);
        SpinWheelDrawCircleAt(cursor_x, cursor_y, cursor_radius, 0x68, 0x28, 0x88);
        return;
    }

    SpinWheelDrawCircleAt(cursor_x, cursor_y, cursor_radius + 4, 0x18, 0x08, 0x28);
    SpinWheelDrawCircleAt(cursor_x, cursor_y, cursor_radius + 2, 0x80, 0x58, 0xA0);
    SpinWheelDrawCircleAt(cursor_x, cursor_y, cursor_radius, 0x80, 0x80, 0x00);
    SpinWheelDrawLine(cursor_x - 3, cursor_y, cursor_x + 3, cursor_y, 0x80, 0x80, 0x00);
    SpinWheelDrawLine(cursor_x, cursor_y - 3, cursor_x, cursor_y + 3, 0x80, 0x80, 0x00);
}

static int SpinWheelScaledRadius(void)
{
    if (spin_wheel_state == SPIN_WHEEL_OPENING)
    {
        int ease = SpinWheelEaseOut(spin_wheel_anim_frame, SPIN_WHEEL_OPEN_FRAMES);
        int pop = spin_wheel_anim_frame > (SPIN_WHEEL_OPEN_FRAMES / 2) ? 5 - ((spin_wheel_anim_frame - (SPIN_WHEEL_OPEN_FRAMES / 2)) / 2) : 0;
        if (pop < 0) pop = 0;
        return ((SPIN_WHEEL_BASE_RADIUS * ease) / SPIN_WHEEL_EASE_SCALE) + pop;
    }
    if (spin_wheel_state == SPIN_WHEEL_CANCELING)
    {
        int ease = SpinWheelEaseIn(spin_wheel_anim_frame, SPIN_WHEEL_CLOSE_FRAMES);
        return (SPIN_WHEEL_BASE_RADIUS * (SPIN_WHEEL_EASE_SCALE - ease)) / SPIN_WHEEL_EASE_SCALE;
    }
    if (spin_wheel_state == SPIN_WHEEL_SELECTING)
    {
        int pulse;
        if (spin_wheel_anim_frame < 4)
        {
            pulse = spin_wheel_anim_frame * 3;
        }
        else if (spin_wheel_anim_frame < 8)
        {
            pulse = 12 - ((spin_wheel_anim_frame - 4) * 2);
        }
        else
        {
            pulse = 6 - ((spin_wheel_anim_frame - 8) * 2);
        }
        if (pulse < 0) pulse = 0;
        return SPIN_WHEEL_BASE_RADIUS + pulse;
    }
    return SPIN_WHEEL_BASE_RADIUS;
}

static int SpinWheelFindSelection(void)
{
    int rx = (int)_rightAnalogStick.x_axis - 128;
    int ry = (int)_rightAnalogStick.y_axis - 128;
    int best_index = -1;
    int best_dot = 0;

    if (spin_wheel_option_count <= 0) return -1;
    if ((rx * rx) + (ry * ry) < (SPIN_WHEEL_STICK_DEADZONE * SPIN_WHEEL_STICK_DEADZONE)) return -1;

    for (int i = 0; i < spin_wheel_option_count; i++)
    {
        int angle = (i * SPIN_WHEEL_FULL_CIRCLE) / spin_wheel_option_count;
        int dir_x = SpinWheelSin(angle);
        int dir_y = -SpinWheelCos(angle);
        int dot = (rx * dir_x) + (ry * dir_y);

        if (best_index < 0 || dot > best_dot)
        {
            best_index = i;
            best_dot = dot;
        }
    }

    return best_index;
}

static void SpinWheelDraw(void)
{
    int radius = SpinWheelScaledRadius();
    int label_radius = SPIN_WHEEL_LABEL_RADIUS;
    int step;
    int selected_for_display = spin_wheel_pending_index >= 0 ? spin_wheel_pending_index : spin_wheel_selected_index;

    if (spin_wheel_option_count <= 0 || radius <= 0) return;

    if (spin_wheel_state == SPIN_WHEEL_OPENING)
    {
        int ease = SpinWheelEaseOut(spin_wheel_anim_frame, SPIN_WHEEL_OPEN_FRAMES);
        label_radius = (SPIN_WHEEL_LABEL_RADIUS * ease) / SPIN_WHEEL_EASE_SCALE;
    }
    else if (spin_wheel_state == SPIN_WHEEL_CANCELING)
    {
        int ease = SpinWheelEaseIn(spin_wheel_anim_frame, SPIN_WHEEL_CLOSE_FRAMES);
        label_radius = (SPIN_WHEEL_LABEL_RADIUS * (SPIN_WHEEL_EASE_SCALE - ease)) / SPIN_WHEEL_EASE_SCALE;
    }
    else if (spin_wheel_state == SPIN_WHEEL_SELECTING)
    {
        label_radius = SPIN_WHEEL_LABEL_RADIUS + (spin_wheel_anim_frame / 2);
    }

    step = SPIN_WHEEL_FULL_CIRCLE / spin_wheel_option_count;

    if (selected_for_display >= 0)
    {
        int selected_angle = selected_for_display * step;
        int selected_pop = SpinWheelSelectedPop();
        int selected_snap = SpinWheelSelectionSnap();
        u8 selected_r = selected_snap > 0 ? 0x58 : 0x44;
        u8 selected_g = selected_snap > 0 ? 0x28 : 0x20;
        u8 selected_b = selected_snap > 0 ? 0x78 : 0x68;
        int selected_outer = radius + 8 + selected_pop + selected_snap;
        int selected_inner = radius - 10 - ((selected_pop + selected_snap) / 3);
        if (selected_inner < SPIN_WHEEL_INNER_RADIUS + 6) selected_inner = SPIN_WHEEL_INNER_RADIUS + 6;
        if (selected_outer < selected_inner + 5) selected_outer = selected_inner + 5;

        SpinWheelDrawBandSegment(selected_angle, (step / 2) - 0x18, selected_inner, selected_outer, selected_r, selected_g, selected_b);
        SpinWheelDrawArc(selected_outer + 1, selected_angle - (step / 2) + 0x18, selected_angle + (step / 2) - 0x18, 0x80, 0x80, 0x00);
        if (selected_snap > 0)
        {
            SpinWheelDrawArc(selected_outer + 4, selected_angle - (step / 2) + 0x28, selected_angle + (step / 2) - 0x28, 0x80, 0x58, 0xA0);
        }
        if (spin_wheel_state == SPIN_WHEEL_SELECTING)
        {
            SpinWheelDrawArc(selected_inner - 1, selected_angle - (step / 2) + 0x28, selected_angle + (step / 2) - 0x28, 0x80, 0x58, 0xA0);
        }
    }

    SpinWheelDrawCircle(radius + 3, 0x18, 0x08, 0x28);
    SpinWheelDrawCircle(radius, 0x70, 0x30, 0x90);
    SpinWheelDrawCircle(radius - 3, 0x30, 0x18, 0x58);
    SpinWheelDrawCircle(SPIN_WHEEL_INNER_RADIUS + 4, 0x14, 0x08, 0x24);
    SpinWheelDrawCircle(SPIN_WHEEL_INNER_RADIUS, 0x80, 0x80, 0x00);
    SpinWheelDrawStickCursor(radius);

    for (int i = 0; i < spin_wheel_option_count; i++)
    {
        int boundary_angle = (i * step) - (step / 2);
        int label_angle = i * step;
        int option_label_radius = label_radius;
        int boundary_x = SPIN_WHEEL_CENTER_X + ((SpinWheelSin(boundary_angle) * radius) / SPIN_WHEEL_TRIG_SCALE);
        int boundary_y = SPIN_WHEEL_CENTER_Y - ((SpinWheelCos(boundary_angle) * radius) / SPIN_WHEEL_TRIG_SCALE);
        int label_x;
        int label_y;
        bool is_selected = i == selected_for_display;

        SpinWheelDrawLine(SPIN_WHEEL_CENTER_X, SPIN_WHEEL_CENTER_Y, boundary_x, boundary_y, 0x28, 0x10, 0x48);
        SpinWheelDrawLine(boundary_x, boundary_y, boundary_x + ((SpinWheelSin(boundary_angle) * 5) / SPIN_WHEEL_TRIG_SCALE), boundary_y - ((SpinWheelCos(boundary_angle) * 5) / SPIN_WHEEL_TRIG_SCALE), 0x68, 0x28, 0x88);

        if (is_selected)
        {
            option_label_radius += SpinWheelSelectedPop() / 2;
            option_label_radius += SpinWheelSelectionSnap() / 2;
        }

        label_x = SPIN_WHEEL_CENTER_X + ((SpinWheelSin(label_angle) * option_label_radius) / SPIN_WHEEL_TRIG_SCALE);
        label_y = SPIN_WHEEL_CENTER_Y - ((SpinWheelCos(label_angle) * option_label_radius) / SPIN_WHEEL_TRIG_SCALE);
        label_x -= SpinWheelStringWidth(spin_wheel_options[i].label) / 2;
        label_y -= 3;
        DrawTextSelected(spin_wheel_options[i].label, label_x, label_y, is_selected);
    }

    DrawTextSelected(deckard_str_spin_r3, SPIN_WHEEL_CENTER_X - 7, SPIN_WHEEL_CENTER_Y - 9, true);
    if (selected_for_display >= 0)
    {
        char selected_line[48];
        sprintf(selected_line, deckard_str_fmt_s, spin_wheel_options[selected_for_display].label);
        DrawTextSelected(selected_line, SPIN_WHEEL_CENTER_X - (SpinWheelStringWidth(selected_line) / 2), SPIN_WHEEL_CENTER_Y + 5, true);

        if (spin_wheel_state == SPIN_WHEEL_SELECTING)
        {
            DrawTextSelected(deckard_str_spin_selected, SPIN_WHEEL_CENTER_X - 28, SPIN_WHEEL_CENTER_Y + 88, true);
        }
        else
        {
            DrawTextSimple(deckard_str_spin_release, SPIN_WHEEL_CENTER_X - 56, SPIN_WHEEL_CENTER_Y + 88);
        }
    }
    else
    {
        DrawTextSimple(deckard_str_spin_move_stick, SPIN_WHEEL_CENTER_X - 32, SPIN_WHEEL_CENTER_Y + 88);
    }
}

static void SpinWheelDrawHoldProgress(void)
{
    int progress_radius;
    int progress;
    int segments;

    if (spin_wheel_hold_frames <= SPIN_WHEEL_HOLD_VISUAL_DELAY) return;

    progress = ((spin_wheel_hold_frames - SPIN_WHEEL_HOLD_VISUAL_DELAY) * SPIN_WHEEL_EASE_SCALE) / (SPIN_WHEEL_HOLD_FRAMES - SPIN_WHEEL_HOLD_VISUAL_DELAY);
    if (progress > SPIN_WHEEL_EASE_SCALE) progress = SPIN_WHEEL_EASE_SCALE;

    progress_radius = 10 + ((progress * 8) / SPIN_WHEEL_EASE_SCALE);
    segments = (progress * 24) / SPIN_WHEEL_EASE_SCALE;

    SpinWheelDrawCircle(progress_radius + 3, 0x18, 0x08, 0x28);
    SpinWheelDrawCircle(progress_radius, 0x50, 0x20, 0x80);

    for (int i = 0; i < segments; i++)
    {
        int angle = (i * SPIN_WHEEL_FULL_CIRCLE) / 24;
        int x1 = SPIN_WHEEL_CENTER_X + ((SpinWheelSin(angle) * (progress_radius + 3)) / SPIN_WHEEL_TRIG_SCALE);
        int y1 = SPIN_WHEEL_CENTER_Y - ((SpinWheelCos(angle) * (progress_radius + 3)) / SPIN_WHEEL_TRIG_SCALE);
        int x2 = SPIN_WHEEL_CENTER_X + ((SpinWheelSin(angle) * (progress_radius + 9)) / SPIN_WHEEL_TRIG_SCALE);
        int y2 = SPIN_WHEEL_CENTER_Y - ((SpinWheelCos(angle) * (progress_radius + 9)) / SPIN_WHEEL_TRIG_SCALE);
        SpinWheelDrawLine(x1, y1, x2, y2, 0x80, 0x80, 0x00);
    }

    DrawTextSelected(deckard_str_spin_r3, SPIN_WHEEL_CENTER_X - 7, SPIN_WHEEL_CENTER_Y - 3, true);
}

static void SpinWheelReset(void)
{
    spin_wheel_state = SPIN_WHEEL_CLOSED;
    spin_wheel_hold_frames = 0;
    spin_wheel_anim_frame = 0;
    spin_wheel_selected_index = -1;
    spin_wheel_pending_index = -1;
    spin_wheel_snap_frame = 0;
}

static void SpinWheelSetSelection(int new_selection)
{
    if (new_selection >= 0 && new_selection != spin_wheel_selected_index)
    {
        spin_wheel_snap_frame = SPIN_WHEEL_SNAP_FRAMES;
    }

    spin_wheel_selected_index = new_selection;
}

static void SpinWheelBeginClose(bool should_select)
{
    spin_wheel_snap_frame = 0;

    if (should_select)
    {
        spin_wheel_pending_index = spin_wheel_selected_index;
        spin_wheel_state = SPIN_WHEEL_SELECTING;
    }
    else
    {
        spin_wheel_pending_index = -1;
        spin_wheel_state = SPIN_WHEEL_CANCELING;
    }
    spin_wheel_anim_frame = 0;
}

bool SpinWheelAddOption(const char* label, SpinWheelCallback callback)
{
    if (spin_wheel_option_count >= SPIN_WHEEL_MAX_OPTIONS || label == NULL || callback == NULL) return false;

    spin_wheel_options[spin_wheel_option_count].label = label;
    spin_wheel_options[spin_wheel_option_count].callback = callback;
    spin_wheel_option_count++;
    return true;
}

void SpinWheelClearOptions(void)
{
    spin_wheel_option_count = 0;
    SpinWheelReset();
}

static void SpinWheelShowTestMessage(const char* message)
{
    spin_wheel_message = message;
    spin_wheel_message_timer = 60;
}

static void SpinWheelTestOptionOne(void)
{
    SpinWheelShowTestMessage(deckard_str_spin_test_opt1);
}

static void SpinWheelTestOptionTwo(void)
{
    SpinWheelShowTestMessage(deckard_str_spin_test_opt2);
}

static void SpinWheelTestOptionThree(void)
{
    SpinWheelShowTestMessage(deckard_str_spin_test_opt3);
}

static void SpinWheelTestOptionFour(void)
{
    SpinWheelShowTestMessage(deckard_str_spin_test_opt4);
}

void SpinWheelRegisterTestOptions(void)
{
    if (spin_wheel_test_registered) return;

    SpinWheelAddOption(deckard_str_spin_opt_one, SpinWheelTestOptionOne);
    SpinWheelAddOption(deckard_str_spin_opt_two, SpinWheelTestOptionTwo);
    SpinWheelAddOption(deckard_str_spin_opt_three, SpinWheelTestOptionThree);
    SpinWheelAddOption(deckard_str_spin_opt_four, SpinWheelTestOptionFour);
    spin_wheel_test_registered = true;
}

void SpinWheelUpdate(void)
{
    bool is_holding_r3;

    if (spin_wheel_message_timer > 0 && _gameState == GAMESTATE_GAMEPLAY)
    {
        DrawTextSimple(spin_wheel_message, SPIN_WHEEL_CENTER_X - (SpinWheelStringWidth(spin_wheel_message) / 2), 24);
        spin_wheel_message_timer--;
    }

    if (spin_wheel_option_count <= 0) return;

    if (_gameState != GAMESTATE_GAMEPLAY)
    {
        SpinWheelReset();
        return;
    }

    is_holding_r3 = (_currentButton & R3_BUTTON) != 0;

    if (spin_wheel_state == SPIN_WHEEL_CLOSED)
    {
        if (is_holding_r3)
        {
            spin_wheel_state = SPIN_WHEEL_HOLDING;
            spin_wheel_hold_frames = 1;
        }
        return;
    }

    if (spin_wheel_state == SPIN_WHEEL_HOLDING)
    {
        SpinWheelDrawHoldProgress();

        if (!is_holding_r3)
        {
            SpinWheelReset();
            return;
        }

        spin_wheel_hold_frames++;
        if (spin_wheel_hold_frames >= SPIN_WHEEL_HOLD_FRAMES)
        {
            spin_wheel_state = SPIN_WHEEL_OPENING;
            spin_wheel_anim_frame = 1;
        }
        return;
    }

    if (spin_wheel_state == SPIN_WHEEL_OPENING)
    {
        SpinWheelSetSelection(SpinWheelFindSelection());
        SpinWheelDraw();
        if (spin_wheel_snap_frame > 0) spin_wheel_snap_frame--;

        if (!is_holding_r3)
        {
            SpinWheelBeginClose(spin_wheel_selected_index >= 0);
            return;
        }

        spin_wheel_anim_frame++;
        if (spin_wheel_anim_frame >= SPIN_WHEEL_OPEN_FRAMES)
        {
            spin_wheel_state = SPIN_WHEEL_OPEN;
        }
        return;
    }

    if (spin_wheel_state == SPIN_WHEEL_OPEN)
    {
        SpinWheelSetSelection(SpinWheelFindSelection());
        SpinWheelDraw();
        if (spin_wheel_snap_frame > 0) spin_wheel_snap_frame--;

        if (!is_holding_r3)
        {
            SpinWheelBeginClose(spin_wheel_selected_index >= 0);
        }
        return;
    }

    if (spin_wheel_state == SPIN_WHEEL_SELECTING)
    {
        SpinWheelDraw();
        spin_wheel_anim_frame++;

        if (spin_wheel_anim_frame >= SPIN_WHEEL_SELECT_FRAMES)
        {
            if (spin_wheel_pending_index >= 0 && spin_wheel_pending_index < spin_wheel_option_count)
            {
                spin_wheel_options[spin_wheel_pending_index].callback();
            }
            SpinWheelReset();
        }
        return;
    }

    if (spin_wheel_state == SPIN_WHEEL_CANCELING)
    {
        SpinWheelDraw();
        spin_wheel_anim_frame++;

        if (spin_wheel_anim_frame >= SPIN_WHEEL_CLOSE_FRAMES)
        {
            SpinWheelReset();
        }
    }
}
