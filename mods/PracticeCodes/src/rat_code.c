#include <common.h>
#include <moby.h>
#include <rat_code.h>
#include <multitap.h>

typedef enum RatCodeState
{
    UNFROZEN,
    FROZEN
} RatCodeState;
RatCodeState rat_code_state = UNFROZEN;

bool has_saved_rat = false;

bool should_lock = false;
int frames_locked = 0;

// Saves doritos position and rotation in a free space in ram
void SaveRat(void)
{
    byte *free_space = (byte *)0x80073990;

    memcpy((byte *)free_space + 0x390, &dorito.position, sizeof(dorito.position));
    memcpy((byte *)free_space + 0x3A0, &dorito.rotation, sizeof(dorito.rotation));

    has_saved_rat = true;
}

// Lock rat for 5 frames, for loading when running
void TempLockRat(void)
{
    byte *free_space = (byte *)0x80073990;

    if (should_lock == true && frames_locked < 5)
    {
        // Loop loading pos/rot
        memcpy((byte *)&dorito.position, free_space + 0x380, sizeof(dorito.position));
        memcpy((byte *)&dorito.rotation, free_space + 0x390, sizeof(dorito.rotation));
        frames_locked++;
    }
    else if (frames_locked == 5)
    {
        should_lock = false;
        frames_locked = 0;
    }
}

// Load the saved position/rotation from ram
void LoadRat(void)
{
    byte *free_space = (byte *)0x80073990;

    if (has_saved_rat)
    {
        memcpy((byte *)&dorito.position, free_space + 0x380, sizeof(dorito.position));
        memcpy((byte *)&dorito.rotation, free_space + 0x390, sizeof(dorito.rotation));

        dorito_run_time_left = 0;

        if (dorito.state == RAT_IDLE)
        {
            dorito.nextKeyFrame = 10;
        }
        else if (dorito.state == RAT_RUNNING)
        {
            should_lock = true; // If the rat is running, lock the rat for 5 frames so it can finish moving
        }
    }
}

void RatCodeUpdate(void)
{
    if (_levelID == GNASTYS_WORLD_ID)
    {
        if (CheckButtonMultiTap(TRIANGLE_BUTTON, 2) && dorito.state == RAT_IDLE)
        {
            rat_code_state = !rat_code_state;
        }

        if (rat_code_state == FROZEN)
        {
            dorito.nextKeyFrame = 10;
        }

        if (_gameState == GAMESTATE_DEATH)
        {
            rat_code_state = UNFROZEN;
        }

        if (_currentButtonOneFrame == L3_BUTTON)
        {
            SaveRat();
        }
        else if (_currentButtonOneFrame == R3_BUTTON)
        {
            LoadRat();
        }

        if (should_lock)
        {
            TempLockRat();
        }
    }
}