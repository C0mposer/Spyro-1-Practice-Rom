#include <common.h>
#include <custom_menu.h>
#include <shared_funcs.h>
#include <multitap.h>
#include <font.h>
#include <deckard_strings.h>

extern VisualizerMenu visualizer_menu;

extern int MCP_RaycastFloorHeightPrecise(Vec3* position);
extern volatile int _cameraRaycastRegionIndex;
extern volatile int _cameraRenderRegionIndex;
extern volatile int _cameraUnknownPositionVectorX;
extern volatile int _worldRegionCount;

extern int current_menu;

static void FreeCamSyncRenderRegion(void)
{
    if ((int)_levelLoadState >= 0)
        return;

    if (MCP_RaycastFloorHeightPrecise(&_cameraPosition) != 0)
        _cameraRenderRegionIndex = _cameraRaycastRegionIndex;

    if (_worldRegionCount <= _cameraRenderRegionIndex)
        _cameraRenderRegionIndex = -1;

    _cameraUnknownPositionVectorX = 0;
}

// Wrappers around the in-game SinScaled function.
// Full circle = 0x1000, output scaled [0, 4096].
static int FreeCamSin(int angle)
{
    return SinScaled((unsigned int)angle);
}

static int FreeCamCos(int angle)
{
    return SinScaled((unsigned int)(angle + 0x400)); // cos(x) = sin(x + 90deg)
}

// ---- Free Cam Speed Modes ----
typedef enum FreeCamSpeedMode
{
    SPEED_SLOWEST,
    SPEED_SLOW,
    SPEED_NORMAL,
    SPEED_FAST,
    SPEED_FASTEST,
    SPEED_MODE_COUNT
} FreeCamSpeedMode;

static const int freecam_speeds[SPEED_MODE_COUNT] = { 25, 100, 300, 500, 800 };
static const int freecam_turn_speeds[SPEED_MODE_COUNT] = { 10, 25, 35, 50, 75 };
static FreeCamSpeedMode freecam_speed_mode = SPEED_NORMAL;

#define FREECAM_DEADZONE     30

// ---- Free Cam Pitch Limits ----
// Clamp pitch so you can't flip upside-down.
// ~87 degrees up/down in Spyro's angle units.
#define FREECAM_PITCH_MAX    0x03E0
#define FREECAM_PITCH_MIN    (-0x03E0)

int free_cam_exit_warning_timer = -1;
void FreeCamUpdate()
{
    if (visualizer_menu.free_cam == true)
    {
        StopCameraFollowSpyro();
        StopUpdateSpyro();

        // ---- EXIT CONTROLS ----
        // Show exit warning
        if (_currentButtonOneFrame == CIRCLE_BUTTON && free_cam_exit_warning_timer == -1)
        {
            free_cam_exit_warning_timer = 0;
        }
        else if (free_cam_exit_warning_timer >= 0 && free_cam_exit_warning_timer < 60)
        {
            DrawTextSimple(FREE_CAM_EXIT_TEXT, 8, 11);

            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                visualizer_menu.free_cam = false;
                current_menu = MAIN_MENU;
                ResetAllMenuSelections();

            }

            free_cam_exit_warning_timer++;
        }
        else
        {
            free_cam_exit_warning_timer = -1;
        }

        // ----- FREE CAM -----
        // ---- Toggle Speed Mode with L2 / R2 ----
        if (_currentButtonOneFrame & L2_BUTTON)
        {
            if (freecam_speed_mode > SPEED_SLOWEST)
                freecam_speed_mode--;
        }
        if (_currentButtonOneFrame & R2_BUTTON)
        {
            if (freecam_speed_mode < SPEED_FASTEST)
                freecam_speed_mode++;
        }
        int speed = freecam_speeds[freecam_speed_mode];
        int turn_speed = freecam_turn_speeds[freecam_speed_mode];

        // ---- Right Stick: Camera Rotation ----
        {
            int rx = (int)_rightAnalogStick.x_axis - 128;
            int ry = (int)_rightAnalogStick.y_axis - 128;

            // Apply deadzone
            if (rx > -FREECAM_DEADZONE && rx < FREECAM_DEADZONE) rx = 0;
            if (ry > -FREECAM_DEADZONE && ry < FREECAM_DEADZONE) ry = 0;

            // Turn yaw (left/right) - analog stick deflection scales proportionally
            _cameraAngle.yaw -= (short)((rx * turn_speed) / 128);

            // Turn pitch (up/down) - stick-up = look-up (positive pitch = look down)
            short newPitch = _cameraAngle.pitch + (short)((ry * turn_speed) / 128);

            // Clamp pitch
            if (newPitch > FREECAM_PITCH_MAX)
                newPitch = FREECAM_PITCH_MAX;
            if (newPitch < FREECAM_PITCH_MIN)
                newPitch = FREECAM_PITCH_MIN;

            _cameraAngle.pitch = newPitch;
        }

        // ---- Left Stick: Camera Movement ----
        {
            int lx = (int)_leftAnalogStick.x_axis - 128;  // strafe (left/right)
            int ly = (int)_leftAnalogStick.y_axis - 128;  // forward/back

            // Apply deadzone
            if (lx > -FREECAM_DEADZONE && lx < FREECAM_DEADZONE) lx = 0;
            if (ly > -FREECAM_DEADZONE && ly < FREECAM_DEADZONE) ly = 0;

            if (lx != 0 || ly != 0)
            {
                int yaw = (int)_cameraAngle.yaw;
                int pitch = (int)_cameraAngle.pitch;

                // Forward vector (projected from yaw + pitch)
                int fwdX = FreeCamCos(yaw);    // forward on X axis
                int fwdY = FreeCamSin(yaw);    // forward on Y axis
                int fwdZ = -FreeCamSin(pitch); // forward on Z axis (negated: positive pitch = down)

                // Scale forward XY by cos(pitch) so looking straight up doesn't move horizontally
                int pitchCos = FreeCamCos(pitch);
                fwdX = (fwdX * pitchCos) / 4096;
                fwdY = (fwdY * pitchCos) / 4096;

                // Right vector (perpendicular to forward, on the horizontal plane)
                // right = yaw - 90 degrees = yaw - 0x400
                int rightX = FreeCamCos(yaw - 0x400);
                int rightY = FreeCamSin(yaw - 0x400);

                // Combine: forward/back (-ly because stick-up = forward) and strafe (lx)
                // Divide by 128 to normalize stick range, then multiply by speed
                int moveX = ((-ly * fwdX) + (lx * rightX)) * speed / (128 * 4096);
                int moveY = ((-ly * fwdY) + (lx * rightY)) * speed / (128 * 4096);
                int moveZ = ((-ly * fwdZ)) * speed / (128 * 4096);

                _cameraPosition.x += moveX;
                _cameraPosition.y += moveY;
                _cameraPosition.z += moveZ;
            }
        }

        // ---- Shoulder Buttons: Vertical Movement ----
        if (_currentButton & R1_BUTTON)
            _cameraPosition.z += speed;
        if (_currentButton & L1_BUTTON)
            _cameraPosition.z -= speed;

        FreeCamSyncRenderRegion();
    }
    else
    {
        RestartCameraFollowSpyro();
        RestartUpdateSpyro();
        freecam_speed_mode = SPEED_NORMAL;
    }
}
