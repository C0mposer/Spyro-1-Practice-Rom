#include <common.h>
#include <sound.h>
#include <custom_text.h>

extern int loadstate_button_index;
extern const short LOADSTATE_BUTTONS[3];

int ns_timer = -1;
int count = 0;
char inputs[5];

void NestorSkipUpdate()
{
    if (_levelID == ARTISANS_ID)
    {
        if (_currentButton == LOADSTATE_BUTTONS[loadstate_button_index])
        {
            ns_timer = -1;
            count = 0;
        }

        if (ns_timer == -1 && _currentButtonOneFrame == X_BUTTON) {
            ns_timer = 0;
        }

        if (ns_timer >= 0) {
            ns_timer++;

            if ((_currentButtonOneFrame & CIRCLE_BUTTON && count == 0) ||
                (_currentButtonOneFrame & SQUARE_BUTTON && count == 1) ||
                (_currentButtonOneFrame & X_BUTTON && count == 2) ||
                (_currentButtonOneFrame & SQUARE_BUTTON && count == 3) ||
                (_currentButtonOneFrame & X_BUTTON && count == 4)) {
                inputs[count] = ns_timer;
                count++;
            }
        }

        if (count == 5) {
            // CapitalTextInfo input_text_info = { 0 };
            // input_text_info.x = SCREEN_LEFT_EDGE + 0x10;
            // input_text_info.y = SCREEN_BOTTOM_EDGE - 0xA;
            // input_text_info.size = DEFAULT_SIZE + 1000;
            // char buffer[15];
            // sprintf(buffer, "%d %d %d %d %d", inputs[0], inputs[1], inputs[2], inputs[3], inputs[4]);
            // DrawTextCapitals(buffer, &input_text_info, DEFAULT_SPACING, MOBY_COLOR_WHITE);

            CapitalTextInfo input_text_info_adjusted = { 0 };
            input_text_info_adjusted.x = SCREEN_LEFT_EDGE + 0x10;
            input_text_info_adjusted.y = SCREEN_BOTTOM_EDGE - 0x1A;
            input_text_info_adjusted.size = DEFAULT_SIZE;
            char buffer2[15];

            char flame_offset = 0;
            if (inputs[0] > 10 && inputs[0] < 15)
            {
                flame_offset = 0;
            }
            else if (inputs[0] <= 10)
            {
                flame_offset = inputs[0] - 11;
            }
            else if (inputs[0] >= 15)
            {
                flame_offset = inputs[0] - 14;
            }

            sprintf(buffer2, "%d %d %d %d %d", flame_offset, (inputs[1] - 17), (inputs[2] - 36), (inputs[3] - 51), (inputs[4]) - 55);
            DrawTextCapitals(buffer2, &input_text_info_adjusted, DEFAULT_SPACING, MOBY_COLOR_GOLD);

            if (_gameState == GAMESTATE_GAMEPLAY)
            {
                RenderShadedMobyQueue();
            }
        }
    }
}