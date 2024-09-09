#include <common.h>
#include <custom_text.h>

PausePatch(){
    DrawTextCapitals("V3.0", &(CapitalTextInfo){.x=(SCREEN_RIGHT_EDGE-70), .y=(SCREEN_BOTTOM_EDGE-10), .size=DEFAULT_SIZE}, DEFAULT_SPACING, MOBY_COLOR_GOLD);

    CopyHudToShaded();
}