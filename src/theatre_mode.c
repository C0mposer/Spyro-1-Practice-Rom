#include <common.h>
#include <deckard_strings.h>
#include <custom_menu.h>
#include <shared_funcs.h>

// Font drawing from font_testing.c
extern void DrawTextSimple(const char* str, int x, int y);

// Free cam functions from custom_menu_2.c
extern void StopCameraFollowSpyro(void);
extern void RestartCameraFollowSpyro(void);
extern void StopUpdateSpyro(void);
extern void RestartUpdateSpyro(void);
extern void RestartDrawWorldAndObjects(void);

// Menu state from custom_menu.c
extern MenuState menu_state;
extern CurrentMenu current_menu;
extern VisualizerMenu visualizer_menu;

// ---- Constants ----
#define MAX_KEYFRAMES       16
#define DEFAULT_TRANSITION_FRAMES  60   // 2 seconds at 30fps
#define DEFAULT_WAIT_FRAMES        0

// ---- Interpolation Modes ----
typedef enum InterpMode
{
    INTERP_EASE_IN_OUT,
    INTERP_LINEAR,
    INTERP_CATMULL_ROM,
    INTERP_EASE_IN,
    INTERP_EASE_OUT,
    INTERP_MODE_COUNT
} InterpMode;

static const char* INTERP_NAMES[INTERP_MODE_COUNT] = {
    deckard_str_theatre_interp_ease_in_out,
    deckard_str_theatre_interp_linear,
    deckard_str_theatre_interp_catmull,
    deckard_str_theatre_interp_ease_in,
    deckard_str_theatre_interp_ease_out
};

// ---- Auto Play Direction ----
typedef enum AutoPlayMode
{
    AUTO_PLAY_OFF,
    AUTO_PLAY_FORWARD,
    AUTO_PLAY_BACKWARD,
    AUTO_PLAY_MODE_COUNT
} AutoPlayMode;

static const char* AUTO_PLAY_NAMES[AUTO_PLAY_MODE_COUNT] = {
    deckard_str_theatre_autoplay_off,
    deckard_str_theatre_autoplay_fwd,
    deckard_str_theatre_autoplay_back
};

#define DEFAULT_PLAYBACK_LOOP   true
#define DEFAULT_AUTO_PLAY       AUTO_PLAY_FORWARD
#define DEFAULT_INTERP_MODE     INTERP_EASE_IN_OUT
#define DEFAULT_GREEN_SCREEN    false

// ---- Keyframe Data ----
typedef struct CameraKeyframe
{
    int posX;
    int posY;
    int posZ;
    short yaw;
    short pitch;
    bool set;
} CameraKeyframe;

// ---- Theatre Mode State ----
typedef enum TheatreState
{
    THEATRE_INACTIVE,       // Not in theatre mode
    THEATRE_FREE_LOOK,      // Free cam, setting keyframes (menu closed)
    THEATRE_MENU,           // Menu is open
    THEATRE_PLAYBACK        // Playing back through keyframes
} TheatreState;

static CameraKeyframe keyframes[MAX_KEYFRAMES] = { 0 };
static int keyframe_count = 0;
static int current_keyframe_index = 0;

// Playback state
static int playback_frame = 0;              // Current frame within transition
static int transition_frames = DEFAULT_TRANSITION_FRAMES;
static bool playback_loop = DEFAULT_PLAYBACK_LOOP;
static AutoPlayMode auto_play = DEFAULT_AUTO_PLAY;
static InterpMode interp_mode = DEFAULT_INTERP_MODE;
static int wait_frames = DEFAULT_WAIT_FRAMES;
static int wait_counter = 0;
static bool hide_hud = true;
static bool green_screen_mode = DEFAULT_GREEN_SCREEN;
static bool green_screen_clear_saved = false;
static unsigned char saved_green_screen_clear[3] = { 0 };

// Theatre menu
static int theatre_menu_selection = 0;
static int theatre_input_lockout = 0;
#define THEATRE_MENU_ITEMS  11

// HUD text flash
static int flash_timer = 0;
static char flash_text[32] = { 0 };

static TheatreState theatre_state = THEATRE_INACTIVE;

// ---- Fixed-Point Easing Functions ----
// All return a value in [0, 4096] for t in [0, 4096]
// Uses the in-game SinScaled function (full circle = 0x1000, output [0, 4096])

static int EaseLinear(int t)
{
    return t; // Already in [0, 4096]
}

static int EaseIn(int t)
{
    // ease-in: 1 - cos(t * pi/2)
    // t is [0, 4096], pi/2 = 0x400 in angle units
    int angle = (t * 0x400) / 4096;
    int cosVal = SinScaled((unsigned int)(angle + 0x400)); // cos(x) = sin(x + 90deg)
    return 4096 - cosVal;
}

static int EaseOut(int t)
{
    // ease-out: sin(t * pi/2)
    int angle = (t * 0x400) / 4096;
    return SinScaled((unsigned int)angle);
}

static int EaseInOut(int t)
{
    // ease-in-out: (1 - cos(t * pi)) / 2
    int angle = (t * 0x800) / 4096; // t * pi (half circle = 0x800)
    int cosVal = SinScaled((unsigned int)(angle + 0x400));
    return (4096 - cosVal) / 2;
}

// ---- Interpolation ----
static int ApplyEasing(int t)
{
    switch (interp_mode)
    {
        case INTERP_EASE_IN:     return EaseIn(t);
        case INTERP_EASE_OUT:    return EaseOut(t);
        case INTERP_EASE_IN_OUT: return EaseInOut(t);
        default:                 return EaseLinear(t);
    }
}

// Lerp between two ints using fixed-point t [0, 4096]
static int Lerp(int a, int b, int t)
{
    return a + (((b - a) * t) / 4096);
}

// Lerp for shorts (yaw/pitch), handling angle wrapping
static short LerpAngle(short a, short b, int t)
{
    // Find shortest path around the circle
    int diff = (int)b - (int)a;

    // Wrap to [-0x800, 0x800) for shortest arc (half circle = 0x800)
    while (diff > 0x800) diff -= 0x1000;
    while (diff < -0x800) diff += 0x1000;

    return (short)(a + (diff * t) / 4096);
}

// Catmull-Rom spline for one component
// p0, p1, p2, p3 are control points; t is [0, 4096]
// Uses Horner's method to avoid integer overflow on PS1:
//   q(t) = p1 + 0.5*t*( a + t*( b + t*c ) )
//   where a = -p0+p2, b = 2p0-5p1+4p2-p3, c = -p0+3p1-3p2+p3
static int CatmullRom(int p0, int p1, int p2, int p3, int t)
{
    int a = -p0 + p2;
    int b = 2 * p0 - 5 * p1 + 4 * p2 - p3;
    int c = -p0 + 3 * p1 - 3 * p2 + p3;

    // Evaluate innermost first, dividing by 4096 at each step to stay in range
    int inner = b + (c * t) / 4096;
    int outer = a + (inner * t) / 4096;
    return p1 + (outer * t) / (2 * 4096);
}

// Get a keyframe index, wrapping for loop mode or clamping otherwise
static int GetKeyframeIndex(int idx)
{
    if (playback_loop)
    {
        while (idx < 0) idx += keyframe_count;
        return idx % keyframe_count;
    }
    if (idx < 0) return 0;
    if (idx >= keyframe_count) return keyframe_count - 1;
    return idx;
}

// Interpolate between keyframes
static void InterpolateCamera(int fromIdx, int toIdx, int t_raw)
{
    int t;
    CameraKeyframe* from = &keyframes[fromIdx];
    CameraKeyframe* to = &keyframes[toIdx];

    if (interp_mode == INTERP_CATMULL_ROM && keyframe_count >= 2)
    {
        // For Catmull-Rom we need 4 points (wraps properly in loop mode)
        int i0 = GetKeyframeIndex(fromIdx - 1);
        int i3 = GetKeyframeIndex(toIdx + 1);
        CameraKeyframe* p0 = &keyframes[i0];
        CameraKeyframe* p3 = &keyframes[i3];

        // t_raw is [0, transition_frames], normalize to [0, 4096]
        t = (t_raw * 4096) / transition_frames;

        _cameraPosition.x = CatmullRom(p0->posX, from->posX, to->posX, p3->posX, t);
        _cameraPosition.y = CatmullRom(p0->posY, from->posY, to->posY, p3->posY, t);
        _cameraPosition.z = CatmullRom(p0->posZ, from->posZ, to->posZ, p3->posZ, t);

        // For angles, use simple eased lerp even in Catmull-Rom mode
        // (spline on angles can cause weird orbits)
        int t_eased = EaseInOut(t);
        _cameraAngle.yaw = LerpAngle(from->yaw, to->yaw, t_eased);
        _cameraAngle.pitch = LerpAngle(from->pitch, to->pitch, t_eased);
    }
    else
    {
        // Linear/Eased modes
        t = (t_raw * 4096) / transition_frames;
        int t_eased = ApplyEasing(t);

        _cameraPosition.x = Lerp(from->posX, to->posX, t_eased);
        _cameraPosition.y = Lerp(from->posY, to->posY, t_eased);
        _cameraPosition.z = Lerp(from->posZ, to->posZ, t_eased);
        _cameraAngle.yaw = LerpAngle(from->yaw, to->yaw, t_eased);
        _cameraAngle.pitch = LerpAngle(from->pitch, to->pitch, t_eased);
    }
}

// ---- Flash Text Helper ----
static void SetFlashText(const char* text, int duration)
{
    int i = 0;
    while (text[i] != '\0' && i < 31)
    {
        flash_text[i] = text[i];
        i++;
    }
    flash_text[i] = '\0';
    flash_timer = duration;
}

// ---- Snap camera to a keyframe ----
static void SnapToKeyframe(int idx)
{
    CameraKeyframe* kf = &keyframes[idx];
    _cameraPosition.x = kf->posX;
    _cameraPosition.y = kf->posY;
    _cameraPosition.z = kf->posZ;
    _cameraAngle.yaw = kf->yaw;
    _cameraAngle.pitch = kf->pitch;
}

// ---- Store current camera as a keyframe ----
static void SetKeyframe(void)
{
    if (keyframe_count >= MAX_KEYFRAMES) return;

    CameraKeyframe* kf = &keyframes[keyframe_count];
    kf->posX = _cameraPosition.x;
    kf->posY = _cameraPosition.y;
    kf->posZ = _cameraPosition.z;
    kf->yaw = _cameraAngle.yaw;
    kf->pitch = _cameraAngle.pitch;
    kf->set = true;

    keyframe_count++;

    sprintf(flash_text, deckard_str_fmt_theatre_keyframe_set, keyframe_count);
    flash_timer = 45; // 1.5 seconds
}

// ---- Clear all keyframes ----
static void ClearKeyframes(void)
{
    for (int i = 0; i < MAX_KEYFRAMES; i++)
    {
        keyframes[i].set = false;
    }
    keyframe_count = 0;
    current_keyframe_index = 0;
    playback_frame = 0;
    wait_counter = 0;
    SetFlashText(deckard_str_theatre_keyframes_cleared, 45);
}

static void ReturnToTheatreMenu(int selection)
{
    theatre_state = THEATRE_MENU;
    visualizer_menu.free_cam = false;
    theatre_menu_selection = selection;
    theatre_input_lockout = 1;
}

// ---- Start Playback ----
static void StartPlayback(void)
{
    if (keyframe_count < 2) return;

    theatre_state = THEATRE_PLAYBACK;
    wait_counter = 0;

    if (auto_play == AUTO_PLAY_BACKWARD)
    {
        // Start from the last segment going backward
        int total_segs = playback_loop ? keyframe_count : (keyframe_count - 1);
        current_keyframe_index = total_segs - 1;
        playback_frame = transition_frames - 1;
        SnapToKeyframe(keyframe_count - 1);
    }
    else
    {
        // Start from the beginning going forward
        current_keyframe_index = 0;
        playback_frame = 0;
        SnapToKeyframe(0);
    }
}

// ---- Advance to next keyframe transition ----
static void AdvancePlayback(void)
{
    // With looping: N segments (including last->first wrap)
    // Without looping: N-1 segments
    int total_segs = playback_loop ? keyframe_count : (keyframe_count - 1);

    current_keyframe_index++;
    if (current_keyframe_index >= total_segs)
    {
        if (playback_loop)
        {
            current_keyframe_index = 0;
        }
        else
        {
            // Stay at the end
            current_keyframe_index = total_segs - 1;
        }
    }
    playback_frame = 0;
}

// ---- Go to previous keyframe transition ----
static void ReversePlayback(void)
{
    current_keyframe_index--;
    if (current_keyframe_index < 0)
    {
        if (playback_loop)
        {
            current_keyframe_index = keyframe_count - 1;
        }
        else
        {
            current_keyframe_index = 0;
        }
    }
    playback_frame = transition_frames - 1;
}

// ---- Int to string helper (for menu display) ----
static int IntToStr(char* buf, int val)
{
    int pos = 0;
    if (val >= 100) { buf[pos++] = '0' + (val / 100); }
    if (val >= 10) { buf[pos++] = '0' + ((val / 10) % 10); }
    buf[pos++] = '0' + (val % 10);
    buf[pos] = '\0';
    return pos;
}

static void ApplyGreenScreenMode(bool turn_on)
{
    volatile unsigned char* clear_color = (volatile unsigned char*)0x80078a50;

    if (turn_on)
    {
        // Skip world and skybox, leaving objects/Spyro over a solid green fill.
        *((int*)0x8002B9CC) = 0x03E00008;
        *((int*)0x8002B9D0) = 0x00000000;
        *((int*)0x8004EBA8) = 0x03E00008;
        *((int*)0x8004EBAC) = 0x00000000;

        if (!green_screen_clear_saved)
        {
            saved_green_screen_clear[0] = clear_color[0];
            saved_green_screen_clear[1] = clear_color[1];
            saved_green_screen_clear[2] = clear_color[2];
            green_screen_clear_saved = true;
        }

        clear_color[0] = 0x00;
        clear_color[1] = 0xFF;
        clear_color[2] = 0x00;
    }
    else
    {
        *((int*)0x8002B9CC) = 0x27BDFFE8;
        *((int*)0x8002B9D0) = 0x3C048007;
        *((int*)0x8004EBA8) = 0x3C018007;
        *((int*)0x8004EBAC) = 0x24217DD8;

        if (green_screen_clear_saved)
        {
            clear_color[0] = saved_green_screen_clear[0];
            clear_color[1] = saved_green_screen_clear[1];
            clear_color[2] = saved_green_screen_clear[2];
            green_screen_clear_saved = false;
        }
    }
}

// ---- Theatre Menu Drawing & Input ----
static void DrawTheatreMenu(void)
{
    int y_start = 40;
    int x = 10;
    int spacing = 9;

    // Title
    DrawTextSimple(deckard_str_theatre_title, x, y_start);
    y_start += spacing + 4;

    // Menu items
    const char* items[THEATRE_MENU_ITEMS];
    items[0] = deckard_str_theatre_place_keyframes;
    items[1] = deckard_str_theatre_start_playback;
    items[2] = AUTO_PLAY_NAMES[auto_play];
    items[3] = playback_loop ? deckard_str_theatre_loop_on : deckard_str_theatre_loop_off;

    // Transition duration text
    char dur_buf[28];
    sprintf(dur_buf, deckard_str_fmt_duration, transition_frames);
    items[4] = dur_buf;

    // Wait frames text
    char wait_buf[28];
    sprintf(wait_buf, deckard_str_fmt_wait_frames, wait_frames);
    items[5] = wait_buf;

    // Interp mode
    items[6] = INTERP_NAMES[interp_mode];

    items[7] = hide_hud ? deckard_str_theatre_hide_hud_on : deckard_str_theatre_hide_hud_off;
    items[8] = green_screen_mode ? deckard_str_theatre_green_on : deckard_str_theatre_green_off;
    items[9] = deckard_str_theatre_clear_keyframes;
    items[10] = deckard_str_theatre_exit;

    // Draw each item
    for (int i = 0; i < THEATRE_MENU_ITEMS; i++)
    {
        // Selection indicator
        if (i == theatre_menu_selection)
        {
            DrawTextSimple(deckard_str_theatre_ui_dash, x, y_start + (i * spacing));
        }
        DrawTextSimple(items[i], x + 8, y_start + (i * spacing));
    }

    // Keyframe counter at bottom
    {
        char kf_buf[16];
        sprintf(kf_buf, deckard_str_fmt_keyframes_count, keyframe_count);
        DrawTextSimple(kf_buf, x, y_start + (THEATRE_MENU_ITEMS * spacing) + 6);
    }

    // Controls hint
    DrawTextSimple(deckard_str_theatre_x_select, x, y_start + (THEATRE_MENU_ITEMS * spacing) + 18);

    if (theatre_input_lockout > 0)
    {
        theatre_input_lockout--;
        return;
    }

    if (_currentButtonOneFrame == CIRCLE_BUTTON)
    {
        PlayMenuSound();
        TheatreModeExit();
        return;
    }

    // ---- Menu Input ----
    if (_currentButtonOneFrame == DOWN_BUTTON)
    {
        theatre_menu_selection = (theatre_menu_selection + 1) % THEATRE_MENU_ITEMS;
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == UP_BUTTON)
    {
        theatre_menu_selection = (theatre_menu_selection + THEATRE_MENU_ITEMS - 1) % THEATRE_MENU_ITEMS;
        PlayMenuSound();
    }

    // Handle selection actions
    if (_currentButtonOneFrame == X_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
    {
        PlayMenuSound();

        switch (theatre_menu_selection)
        {
            case 0: // Place keyframes
            if (_currentButtonOneFrame == X_BUTTON)
            {
                theatre_state = THEATRE_FREE_LOOK;
                visualizer_menu.free_cam = true;
                theatre_input_lockout = 1;
            }
            break;

            case 1: // Start Playback
            if (keyframe_count >= 2)
            {
                visualizer_menu.free_cam = false;
                StartPlayback();
            }
            break;

            case 2: // Auto play mode (Off / Forward / Backward)
            if (_currentButtonOneFrame == LEFT_BUTTON)
            {
                if (auto_play > 0) auto_play--;
                else auto_play = AUTO_PLAY_MODE_COUNT - 1;
            }
            else
            {
                auto_play = (auto_play + 1) % AUTO_PLAY_MODE_COUNT;
            }
            break;

            case 3: // Loop toggle
            playback_loop = !playback_loop;
            break;

            case 4: // Transition duration
            if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == X_BUTTON)
            {
                transition_frames += 30;
                if (transition_frames > 600) transition_frames = 600;
            }
            else if (_currentButtonOneFrame == LEFT_BUTTON)
            {
                transition_frames -= 30;
                if (transition_frames < 15) transition_frames = 15;
            }
            break;

            case 5: // Wait frames
            if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == X_BUTTON)
            {
                wait_frames += 15;
                if (wait_frames > 300) wait_frames = 300;
            }
            else if (_currentButtonOneFrame == LEFT_BUTTON)
            {
                wait_frames -= 15;
                if (wait_frames < 0) wait_frames = 0;
            }
            break;

            case 6: // Interp mode
            if (_currentButtonOneFrame == LEFT_BUTTON)
            {
                if (interp_mode > 0) interp_mode--;
                else interp_mode = INTERP_MODE_COUNT - 1;
            }
            else
            {
                interp_mode = (interp_mode + 1) % INTERP_MODE_COUNT;
            }
            break;

            case 7: // Hide HUD toggle
            hide_hud = !hide_hud;
            break;

            case 8: // Green screen mode toggle
            green_screen_mode = !green_screen_mode;
            ApplyGreenScreenMode(green_screen_mode);
            break;

            case 9: // Clear keyframes
            if (_currentButtonOneFrame == X_BUTTON)
            {
                ClearKeyframes();
            }
            break;

            case 10: // Exit Theatre Mode
            if (_currentButtonOneFrame == X_BUTTON)
            {
                TheatreModeExit();
                return;
            }
            break;
        }
    }
}

// ---- Playback Update ----
static void UpdatePlayback(void)
{
    StopCameraFollowSpyro();
    StopUpdateSpyro();

    // Handle wait at keyframe
    if (wait_counter > 0)
    {
        wait_counter--;
        if (_currentButtonOneFrame == CIRCLE_BUTTON || _currentButtonOneFrame == TRIANGLE_BUTTON)
        {
            ReturnToTheatreMenu(1);
        }
        return;
    }

    int fromIdx = current_keyframe_index;
    int toIdx;
    int total_segs = playback_loop ? keyframe_count : (keyframe_count - 1);

    if (playback_loop)
    {
        toIdx = (fromIdx + 1) % keyframe_count;
    }
    else
    {
        toIdx = fromIdx + 1;
        if (toIdx >= keyframe_count)
        {
            // Reached the end (non-looping)
            SnapToKeyframe(keyframe_count - 1);

            if (!hide_hud)
            {
                DrawTextSimple(deckard_str_theatre_end_square_back, 10, 10);
            }

            if (_currentButton & SQUARE_BUTTON)
            {
                playback_frame--;
                if (playback_frame < 0)
                {
                    ReversePlayback();
                }
                else
                {
                    // Still in last segment going backward
                    fromIdx = current_keyframe_index;
                    toIdx = current_keyframe_index + 1;
                    if (toIdx < keyframe_count)
                        InterpolateCamera(fromIdx, toIdx, playback_frame);
                }
            }
            if (_currentButtonOneFrame == CIRCLE_BUTTON || _currentButtonOneFrame == TRIANGLE_BUTTON)
            {
                ReturnToTheatreMenu(1);
            }
            return;
        }
    }

    // Interpolate
    InterpolateCamera(fromIdx, toIdx, playback_frame);

    // Advance frame based on auto play mode
    if (auto_play == AUTO_PLAY_FORWARD)
    {
        playback_frame++;
        if (playback_frame >= transition_frames)
        {
            SnapToKeyframe(toIdx);
            wait_counter = wait_frames;
            AdvancePlayback();
        }
    }
    else if (auto_play == AUTO_PLAY_BACKWARD)
    {
        playback_frame--;
        if (playback_frame < 0)
        {
            SnapToKeyframe(fromIdx);
            wait_counter = wait_frames;
            ReversePlayback();
        }
    }
    else
    {
        // Manual: X = advance, Square = reverse. Circle returns to the menu.
        if (_currentButton & X_BUTTON)
        {
            playback_frame++;
            if (playback_frame >= transition_frames)
            {
                SnapToKeyframe(toIdx);
                wait_counter = wait_frames;
                AdvancePlayback();
            }
        }
        if (_currentButton & SQUARE_BUTTON)
        {
            playback_frame--;
            if (playback_frame < 0)
            {
                SnapToKeyframe(fromIdx);
                wait_counter = wait_frames;
                ReversePlayback();
            }
        }
    }

    // Show playback position (respects hide_hud)
    if (!hide_hud)
    {
        char pos_buf[12];
        pos_buf[0] = '0' + ((fromIdx + 1) % 10);
        pos_buf[1] = '-';
        pos_buf[2] = '>';
        pos_buf[3] = '0' + ((toIdx + 1) % 10);
        pos_buf[4] = '\0';
        DrawTextSimple(pos_buf, 10, 10);
    }

    if (_currentButtonOneFrame == CIRCLE_BUTTON || _currentButtonOneFrame == TRIANGLE_BUTTON)
    {
        ReturnToTheatreMenu(1);
    }
}

// ---- Free Look Mode (setting keyframes) ----
static void UpdateFreeLook(void)
{
    // Free cam is handled by FreeCamUpdate() since visualizer_menu.free_cam is ON
    // We just handle keyframe setting and menu opening here
    bool input_locked = false;
    if (theatre_input_lockout > 0)
    {
        theatre_input_lockout--;
        input_locked = true;
    }

    // X button: set keyframe
    if (!input_locked && _currentButtonOneFrame == X_BUTTON)
    {
        SetKeyframe();
    }

    if (!input_locked &&
        (_currentButtonOneFrame == CIRCLE_BUTTON || _currentButtonOneFrame == TRIANGLE_BUTTON))
    {
        ReturnToTheatreMenu(0);
    }

    DrawTextSimple(deckard_str_theatre_triangle_save, 10, 10);

    // Draw flash text
    if (flash_timer > 0)
    {
        DrawTextSimple(flash_text, 10, 20);
        flash_timer--;
    }

    // Show keyframe count
    if (keyframe_count > 0 && !hide_hud)
    {
        char kf_buf[6];
        kf_buf[0] = '0' + keyframe_count;
        kf_buf[1] = '/';
        kf_buf[2] = '1';
        kf_buf[3] = '6';
        kf_buf[4] = '\0';
        DrawTextSimple(kf_buf, 200, 10);
    }
}

// ==========================
// Main Theatre Mode Update
// ==========================
void TheatreModeUpdate(void)
{
    // Entering theatre mode: from the visualizer menu, when user presses X on "Theatre Mode"
    // This is handled externally.
    if (theatre_state == THEATRE_INACTIVE) return;

    RestartDrawWorldAndObjects();
    ApplyGreenScreenMode(green_screen_mode);

    // Free cam controls are only active while placing keyframes.
    visualizer_menu.free_cam = (theatre_state == THEATRE_FREE_LOOK);

    switch (theatre_state)
    {
        case THEATRE_FREE_LOOK:
        UpdateFreeLook();
        break;

        case THEATRE_MENU:
        StopCameraFollowSpyro();
        StopUpdateSpyro();
        DrawTheatreMenu();
        break;

        case THEATRE_PLAYBACK:
        UpdatePlayback();
        break;

        default:
        break;
    }
}

// Called when exiting theatre mode entirely
void TheatreModeExit(void)
{
    theatre_state = THEATRE_INACTIVE;
    ClearKeyframes();
    auto_play = DEFAULT_AUTO_PLAY;
    playback_loop = DEFAULT_PLAYBACK_LOOP;
    interp_mode = DEFAULT_INTERP_MODE;
    transition_frames = DEFAULT_TRANSITION_FRAMES;
    wait_frames = DEFAULT_WAIT_FRAMES;
    wait_counter = 0;
    hide_hud = true;
    green_screen_mode = DEFAULT_GREEN_SCREEN;
    ApplyGreenScreenMode(false);
    theatre_menu_selection = 0;
    theatre_input_lockout = 0;
    visualizer_menu.free_cam = false;
    RestartCameraFollowSpyro();
    RestartUpdateSpyro();
    RestartDrawWorldAndObjects();
    _spyro.isMovementLocked = FALSE;
}

// Check if theatre mode is active (for use by other files)
bool IsTheatreModeActive(void)
{
    return theatre_state != THEATRE_INACTIVE;
}

// Enter theatre mode
void TheatreModeEnter(void)
{
    theatre_state = THEATRE_MENU;
    visualizer_menu.free_cam = false;
    keyframe_count = 0;
    current_keyframe_index = 0;
    playback_frame = 0;
    auto_play = DEFAULT_AUTO_PLAY;
    playback_loop = DEFAULT_PLAYBACK_LOOP;
    interp_mode = DEFAULT_INTERP_MODE;
    transition_frames = DEFAULT_TRANSITION_FRAMES;
    wait_frames = DEFAULT_WAIT_FRAMES;
    wait_counter = 0;
    hide_hud = true;
    green_screen_mode = DEFAULT_GREEN_SCREEN;
    ApplyGreenScreenMode(false);
    theatre_menu_selection = 0;
    theatre_input_lockout = 1;
    flash_timer = 0;
}
