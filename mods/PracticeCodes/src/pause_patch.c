#include <common.h>
#include <custom_text.h>

// Display version number in pause menu
void PausePatch(void)
{
    if (_gameState == GAMESTATE_PAUSED)
    {
        DrawTextCapitals(MOD_VERSION_STRING, &(CapitalTextInfo){.x=(SCREEN_RIGHT_EDGE-70), .y=(SCREEN_BOTTOM_EDGE-10), .size=DEFAULT_SIZE}, DEFAULT_SPACING, MOBY_COLOR_PURPLE);
    }
    CopyHudToShaded();
}