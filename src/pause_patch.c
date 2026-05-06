#include <common.h>
#include <custom_text.h>
#include <playtime_timer.h>

extern PlaytimeTimer playtime_timer;
extern char global_playtime_string[15];

// Display version number in pause menu by hooking into pause function
void PauseHook(void)
{
    if (_gameState == GAMESTATE_PAUSED)
    {
        CalculatePlaytimeUpdate();

        // Mod Version Number
        DrawTextCapitals(MOD_VERSION_STRING, &(CapitalTextInfo){.x = (SCREEN_RIGHT_EDGE - 70), .y = (SCREEN_BOTTOM_EDGE - 10), .size = DEFAULT_SIZE}, DEFAULT_SPACING, MOBY_COLOR_PURPLE);

        // Playtime
        DrawTextCapitals(global_playtime_string, &(CapitalTextInfo){.x = (SCREEN_LEFT_EDGE + 15), .y = (SCREEN_BOTTOM_EDGE - 10), .size = DEFAULT_SIZE}, DEFAULT_SPACING, MOBY_COLOR_GOLD);

    }

    CopyHudToShaded();
}
