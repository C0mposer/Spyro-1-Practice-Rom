#include <common.h>
#include <right_stick.h>
#include <custom_menu.h>
#include <custom_text.h>
#include <sound.h>

int held_timer_l2 = 0;
int held_timer_r2 = 0;

int last_gem_count = 0;

void ChangeGemcountFlyIn(void)
{
    _globalGems = last_gem_count;

    if (_currentButtonOneFrame == R1_BUTTON)
    {
        _globalGems++;
        _loadingScreenTimer = _loadingScreenTimer > 260 ? 260 : _loadingScreenTimer;
        PlaySoundEffectSimple(SOUND_EFFECT_GEM_HIT_FLOOR);
    }
    else if (_currentButtonOneFrame == L1_BUTTON)
    {
        _globalGems--;
        _loadingScreenTimer = _loadingScreenTimer > 260 ? 260 : _loadingScreenTimer;
        PlaySoundEffectSimple(SOUND_EFFECT_GEM_HIT_FLOOR);
    }
    if (_currentButtonOneFrame == R2_BUTTON)
    {
        _globalGems += 10;
        _loadingScreenTimer = _loadingScreenTimer > 260 ? 260 : _loadingScreenTimer;
        PlaySoundEffectSimple(SOUND_EFFECT_GEM_HIT_FLOOR);
    }
    else if (_currentButtonOneFrame == L2_BUTTON)
    {
        _globalGems -= 10;
        _loadingScreenTimer = _loadingScreenTimer > 260 ? 260 : _loadingScreenTimer;
        PlaySoundEffectSimple(SOUND_EFFECT_GEM_HIT_FLOOR);
    }

    if (_currentButton == L2_BUTTON)
    {
        held_timer_l2++;
    }
    else
    {
        held_timer_l2 = 0;
    }
    if (_currentButton == R2_BUTTON)
    {
        held_timer_r2++;
    }
    else
    {
        held_timer_r2 = 0;
    }

    if (held_timer_l2 > 15)
    {
        _globalGems -= 50;
        _loadingScreenTimer = _loadingScreenTimer > 260 ? 260 : _loadingScreenTimer;
        PlaySoundEffectSimple(SOUND_EFFECT_GEM_HIT_FLOOR);
    }
    else if (held_timer_r2 > 15)
    {
        _globalGems += 50;
        _loadingScreenTimer = _loadingScreenTimer > 260 ? 260 : _loadingScreenTimer;
        PlaySoundEffectSimple(SOUND_EFFECT_GEM_HIT_FLOOR);
    }

    CapitalTextInfo gem_count_text_info = { 0 };
    gem_count_text_info.x = SCREEN_RIGHT_EDGE - 0x70;
    gem_count_text_info.y = SCREEN_BOTTOM_EDGE - 6;
    gem_count_text_info.size = DEFAULT_SIZE + 1500;

    char gem_count_ascii[16] = { 0 };
    sprintf(&gem_count_ascii, "GEMS %d", _globalGems);

    DrawTextCapitals(gem_count_ascii, &gem_count_text_info, DEFAULT_SPACING - 5, MOBY_COLOR_GOLD);

    last_gem_count = _globalGems;
}
