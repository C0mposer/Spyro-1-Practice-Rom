#include <custom_types.h>
#include <common.h>
#include <sound.h>
#include <custom_text.h>

extern int loadstate_button_index;
extern const short LOADSTATE_BUTTONS[3];

int frame_timer_after_jump = -1;
char current_input_index = 0;
char input_frames[5];

// Stores what frame after the initial jump you pressed each subsequent input. Then, prints how early/late you were on each input compared to the Daze/Composer setup.
void NestorSkipUpdate()
{
    if (_levelID == ARTISANS_ID)
    {
        bool should_reset_frame_data = _currentButton == LOADSTATE_BUTTONS[loadstate_button_index] || _gameState == GAMESTATE_CUTSCENE;
        if (should_reset_frame_data)
        {
            frame_timer_after_jump = -1;
            current_input_index = 0;
        }

        if (_spyro.position.x == 0x14C00) // Exact X start position (stays the same throughout entire sequence)
        {
            if (_spyro.position.z == 0x2554) // Exact Z start position
            {
                if (frame_timer_after_jump == -1 && _currentButtonOneFrame == X_BUTTON) {
                    frame_timer_after_jump = 0;
                }
            }

            if (frame_timer_after_jump >= 0) {
                frame_timer_after_jump++;

                if ((_currentButtonOneFrame & CIRCLE_BUTTON && current_input_index == 0) ||
                    (_currentButtonOneFrame & SQUARE_BUTTON && current_input_index == 1) ||
                    (_currentButtonOneFrame & X_BUTTON && current_input_index == 2) ||
                    (_currentButtonOneFrame & SQUARE_BUTTON && current_input_index == 3) ||
                    (_currentButtonOneFrame & X_BUTTON && current_input_index == 4)) {
                    input_frames[current_input_index] = frame_timer_after_jump;
                    current_input_index++;
                }
            }

            if (current_input_index == 5) {
                // RAW Frame Data
                // CapitalTextInfo input_text_info = { 0 };
                // input_text_info.x = SCREEN_LEFT_EDGE + 0x10;
                // input_text_info.y = SCREEN_BOTTOM_EDGE - 0xA;
                // input_text_info.size = DEFAULT_SIZE + 1000;
                // char buffer[15];
                // sprintf(buffer, "%d %d %d %d %d", input_frames[0], input_frames[1], input_frames[2], input_frames[3], input_frames[4]);
                // DrawTextCapitals(buffer, &input_text_info, DEFAULT_SPACING, MOBY_COLOR_WHITE);

                CapitalTextInfo input_text_info_adjusted = { 0 };
                input_text_info_adjusted.x = SCREEN_LEFT_EDGE + 0x10;
                input_text_info_adjusted.y = SCREEN_BOTTOM_EDGE - 0x1A;
                input_text_info_adjusted.size = DEFAULT_SIZE;
                char buffer2[15];

                char flame_offset = 0;
                if (input_frames[0] > 10 && input_frames[0] < 15)
                {
                    flame_offset = 0;
                }
                else if (input_frames[0] <= 10)
                {
                    flame_offset = input_frames[0] - 11;
                }
                else if (input_frames[0] >= 15)
                {
                    flame_offset = input_frames[0] - 14;
                }

                sprintf(buffer2, "%d %d %d %d %d", flame_offset, (input_frames[1] - 17), (input_frames[2] - 36), (input_frames[3] - 51), (input_frames[4]) - 55);
                DrawTextCapitals(buffer2, &input_text_info_adjusted, DEFAULT_SPACING, MOBY_COLOR_GOLD);

                if (_gameState == GAMESTATE_GAMEPLAY)
                {
                    RenderShadedMobyQueue();
                }
            }
        }
    }
}