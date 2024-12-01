#include <common.h>
#include <custom_menu.h>
#include <custom_text.h>
#include <sound.h>
#include <shared_funcs.h>
#include <main_updates.h>
#include <cosmetic.h>
#include <igt.h>
#include <moby.h>
#include <cd.h>

extern ILMenu il_menu;

// Loop Level selection in HW fly in
void UpdateLoopLevel()
{
    if (_currentButtonOneFrame == RIGHT_BUTTON)
    {
        il_menu.dont_loop_level = false;
        //PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
    }
    if (_currentButtonOneFrame == LEFT_BUTTON)
    {
        il_menu.dont_loop_level = true;
        //PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
    }
}

void LoopLevelChoiceFlyIn(void)
{
    UpdateLoopLevel();

    CapitalTextInfo loop_level_text_info = { 0 };
    loop_level_text_info.x = SCREEN_LEFT_EDGE + 10;
    loop_level_text_info.y = SCREEN_BOTTOM_EDGE - 6;
    loop_level_text_info.size = DEFAULT_SIZE + 1500;

    if (il_menu.dont_loop_level == false)
        DrawTextCapitals("LOOP LEVEL ON", &loop_level_text_info, DEFAULT_SPACING - 5, MOBY_COLOR_BLUE);
    if (il_menu.dont_loop_level == true)
        DrawTextCapitals("LOOP LEVEL OFF", &loop_level_text_info, DEFAULT_SPACING - 5, MOBY_COLOR_BLUE);

    // Render Text
    RenderShadedMobyQueue();
}