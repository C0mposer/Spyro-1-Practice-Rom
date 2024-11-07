#include <custom_text.h>
#include <common.h>


int* hblank_timer = 0x1F801110;
int last_frame_timer_val;

int hblank_differences[320] = { 0 };

int average_lag = 0;

extern int frames_tested; //from lag_tests.c

void HBlankUpdate(void)
{
    short hblank_dif = 1;

    if (_gameState == GAMESTATE_GAMEPLAY)
    {
        if (hblank_dif > 0)
        {
            hblank_dif = *hblank_timer - last_frame_timer_val;

            hblank_differences[frames_tested] = hblank_dif;

            printf("frame: %d\tdifference: %d\n", frames_tested, hblank_dif);


            char timer_text[10] = { 0 };
            sprintf(&timer_text, "%d", hblank_dif);
            //printf("%d\n", hblank_dif);

            last_frame_timer_val = *hblank_timer;

            CapitalTextInfo hblank_text_info = { 0 };
            hblank_text_info.x = SCREEN_LEFT_EDGE + 15;
            hblank_text_info.y = SCREEN_BOTTOM_EDGE - 10;
            hblank_text_info.size = DEFAULT_SIZE;
            DrawTextCapitals(timer_text, &hblank_text_info, DEFAULT_SPACING, MOBY_COLOR_GOLD);

            RenderShadedMobyQueue();
        }


        if (frames_tested == 310)
        {
            average_lag = CalculateAverageLag();
        }

        else if (frames_tested > 310)
        {
            char average_lag_text[10] = { 0 };

            sprintf(&average_lag_text, "%d", average_lag);

            CapitalTextInfo average_text_info = { 0 };
            average_text_info.x = SCREEN_LEFT_EDGE + 15;
            average_text_info.y = SCREEN_BOTTOM_EDGE - 30;
            average_text_info.size = DEFAULT_SIZE;
            DrawTextCapitals(average_lag_text, &average_text_info, DEFAULT_SPACING, MOBY_COLOR_BLUE);
            RenderShadedMobyQueue();
            printf("text: %s\n", average_lag_text);
        }
    }
}

int CalculateAverageLag(void)
{
    int sum;
    for (int i = 0; i < 300; i++)
    {
        sum += hblank_differences[(i + 10)]; //skip the first 10 for accuracy (loading in, flying in, etc)
    }

    int average = sum / 300;

    printf("average: %d\n", average);

    return average;
}
