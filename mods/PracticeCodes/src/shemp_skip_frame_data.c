#include <common.h>
#include <moby.h>
#include <custom_text.h>

int IntToAscii(int number) {
   return '0' + number;
}

const char* EARLY_TEXT = "FRAMES EARLY";
const char* LATE_TEXT = "FRAMES LATE";
const char* PERFECT_TEXT = "PERFECT";

int frames_early_trick = 0;  
int frames_late_trick = 0; 
int frames_to_display_text = 0;

#define IDLE 0
#define KNIFE_SWING 5
extern Moby _shemplet;


ShempSkipFrameDataUpdate()
{
    if (_levelID == SHEMP_ID)
    {
        printf("anim %X\tnext anim %X\tcur kf %X\tnext kf %X\tspeed %X\n", _shemplet.currentAnimation, _shemplet.nextAnimation, _shemplet.currentKeyFrame, _shemplet.nextKeyFrame, _shemplet.speedRelated);

        if (_shemplet.currentAnimation == KNIFE_SWING && _shemplet.nextAnimation == KNIFE_SWING)
        {
            if (_currentButtonOneFrame == X_BUTTON)
            {
                printf("PRESSED X\n");
                if (_shemplet.currentKeyFrame == 0x0 && _shemplet.nextKeyFrame == 0x1 && (_shemplet.speedRelated == 0x00 || _shemplet.speedRelated == 0x10))
                {
                    frames_early_trick = 3;

                    frames_to_display_text = 60;
                }
                else if (_shemplet.currentKeyFrame == 0x0 && _shemplet.nextKeyFrame == 0x1 && (_shemplet.speedRelated == 0x20 || _shemplet.speedRelated == 0x30))
                {
                    frames_early_trick = 2;

                    frames_to_display_text = 60;
                }
                else if (_shemplet.currentKeyFrame == 0x1 && _shemplet.nextKeyFrame == 0x2 && (_shemplet.speedRelated == 0x00 || _shemplet.speedRelated == 0x10))
                {
                    frames_early_trick = 1;

                    frames_to_display_text = 60;
                }
                else if (_shemplet.currentKeyFrame == 0x1 && _shemplet.nextKeyFrame == 0x2 && (_shemplet.speedRelated == 0x20 || _shemplet.speedRelated == 0x30))
                {

                    frames_to_display_text = 60;
                }
            }
        }

        if (frames_to_display_text != 0)
        {
            printf("EARLY %d\n", frames_early_trick);
            printf("LATE %d\n", frames_late_trick);

            CapitalTextInfo frames_off_text_info = {.x=SCREEN_RIGHT_EDGE - 250, .y=SCREEN_BOTTOM_EDGE - 10, .size=DEFAULT_SIZE};
            CapitalTextInfo early_late_text_info = {.x=SCREEN_RIGHT_EDGE - 220, .y=SCREEN_BOTTOM_EDGE - 10, .size=DEFAULT_SIZE};
            CapitalTextInfo peftect_text_info = {.x=SCREEN_RIGHT_EDGE - 120, .y=SCREEN_BOTTOM_EDGE - 10, .size=DEFAULT_SIZE};

            // Draw FRAMES EARLY
            if (frames_early_trick > 0)
            {
                DrawTextCapitals(IntToAscii(frames_early_trick), &frames_off_text_info, DEFAULT_SPACING, MOBY_COLOR_GOLD);
                DrawTextCapitals(EARLY_TEXT, &early_late_text_info, DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            // Draw FRAMES LATE
            else if (frames_late_trick > 0)
            {
                DrawTextCapitals(IntToAscii(frames_late_trick), &frames_off_text_info, DEFAULT_SPACING, MOBY_COLOR_GOLD);
                DrawTextCapitals(LATE_TEXT, &early_late_text_info, DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            // Draw "PERFECT"
            else
            {
                DrawTextCapitals(PERFECT_TEXT, &frames_off_text_info, DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }


            // CapitalTextInfo version_text_info = {0};
            // version_text_info.x = SCREEN_RIGHT_EDGE - 70;
            // version_text_info.y = SCREEN_BOTTOM_EDGE - 10;
            // version_text_info.size = DEFAULT_SIZE;
            
            // DrawTextCapitals(MOD_VERSION_STRING, &version_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);

            RenderShadedMobyQueue();

            frames_to_display_text--;
        }

    }

}