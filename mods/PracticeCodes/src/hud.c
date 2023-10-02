#include <common.h>
#include <custom_text.h>

CapitalTextInfo text_info[3] = {{0}};

void HUDHook()
{
    
    //if(_gameState == GAMESTATE_INVENTORY)
    //{
        text_info[0].x = 0x1b8;
        text_info[0].y = 0x2c;
        text_info[0].size = 0x1200;

        DrawTextCapitals("TESTING", &text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);

        MobyRender();
    //}
}