#include <common.h>
#include <custom_text.h>
#include <right_stick.h>
#include <multitap.h>

extern const short LOADSTATE_BUTTONS[3];
extern int loadstate_button_index;

int consistency_tracker_render_time = 0; // Render time when you press a button, or turn it on. Currently 1 seconds (30 frames)

int loadstate_render_delay = 0; // Used to fix VRAM corruption


typedef struct Consistency
{
    int successAmount;
    int totalAmount;
}Consistency;
Consistency consistency_tracker = {0};

extern bool consistency_tracker_mode;

void DrawConsistencyInfo(void)
{
    // Text Position Info
    CapitalTextInfo success_text_info = {0};
    success_text_info.x = SCREEN_LEFT_EDGE + 15;
    success_text_info.y = SCREEN_BOTTOM_EDGE - 27;
    success_text_info.size = DEFAULT_SIZE;

    CapitalTextInfo total_text_info = {0};
    total_text_info.x = SCREEN_LEFT_EDGE + 15;
    total_text_info.y = SCREEN_BOTTOM_EDGE - 10;
    total_text_info.size = DEFAULT_SIZE;

    // Convert Success/Total's into ASCII
    char success_amount_text[4] = {0};
    char total_amount_text[4] = {0};
    NumberToAscii(consistency_tracker.successAmount, &success_amount_text);
    NumberToAscii(consistency_tracker.totalAmount, &total_amount_text);

    // Draw the text
    DrawTextCapitals(success_amount_text, &success_text_info, DEFAULT_SPACING, MOBY_COLOR_GOLD);
    DrawTextCapitals(total_amount_text, &total_text_info, DEFAULT_SPACING, MOBY_COLOR_GOLD);

    RenderShadedMobyQueue();
}

//! Every Frame Update
void TrackConsistencyUpdate(void)
{
    if (_gameState == GAMESTATE_GAMEPLAY && consistency_tracker_mode == true)
    {
        int right_stick_direction = GetVerticalRightStickDirection(); // Get right stick direction this frame

        // Increase total counter by 1 if loaded a state
        if (_currentButtonOneFrame == LOADSTATE_BUTTONS[loadstate_button_index])
        {
            consistency_tracker.totalAmount++;

            consistency_tracker_render_time = 30;           // Set the time to render the text to be 1 seconds
            loadstate_render_delay = 3; // Set the render delay to 3 frames
            //printf("what\n");
        }
        // Increase total counter by 1 if L2 + R2 + Right Stick Up
        else if (_currentButton == L2_BUTTON + R2_BUTTON && right_stick_direction == UP)
        {
            consistency_tracker.totalAmount++;
            consistency_tracker_render_time = 30;           // Set the time to render the text to be 1 seconds
        }
        // Decrease total counter by 1 if L2 + R2 + Right Stick Down
        else if (_currentButton == L2_BUTTON + R2_BUTTON && right_stick_direction == DOWN)
        {
            consistency_tracker.totalAmount--;
            consistency_tracker_render_time = 30;           // Set the time to render the text to be 1 seconds
        }
        // Increase success counter by 1 if Right Stick UP
        else if (right_stick_direction == UP)
        {
            consistency_tracker.successAmount++;
            consistency_tracker_render_time = 30;           // Set the time to render the text to be 1 seconds
        } 
        // Increase success counter by 1 if Right Stick DOWN
        else if (right_stick_direction == DOWN)
        {
            consistency_tracker.successAmount--;
            consistency_tracker_render_time = 30;           // Set the time to render the text to be 1 seconds
        }

        if(CheckButtonMultiTap(LOADSTATE_BUTTONS[loadstate_button_index], 3))
        {
            consistency_tracker.successAmount = 0;
            consistency_tracker.totalAmount = 0;
        }

        // Render while holding L2 + R2
        if (_currentButton == L2_BUTTON + R2_BUTTON)
        {
            consistency_tracker_render_time = 30; // Set the time to render the text to be 1 seconds
        }

        // Decrement render delay
        if (loadstate_render_delay > 0)
            loadstate_render_delay--;
        
        // Render while the render timer isn't over, and when the render delay is up
        if (loadstate_render_delay == 0 && consistency_tracker_render_time > 0)
        {
            DrawConsistencyInfo();  // Draw the streak info on screen
            consistency_tracker_render_time--;          // Decrement render time
        }
    }
}