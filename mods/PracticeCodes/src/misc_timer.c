#include <common.h>
#include <spyro.h>
#include <custom_menu.h>

extern ILDisplayMenu il_display_modes;

// typedef union MiscDisplayTimer
// {
//     bool should_display_time;
//     int display_timer;
// } MiscDisplayTimer;

// MiscDisplayTimer misc_display_timer;
// misc_display_timer.should

int misc_display_timer = 0;

bool has_landed = false;
bool has_glided = false;
bool has_entered_whirlwind = false;
bool has_bonked = false;
bool has_flamed = false;
bool has_collected_item = false;
bool has_collected_flight_item = false;

bool IsInMiscState(void)
{
    return _spyro.isGrounded == true || _spyro.state == GLIDE || _spyro.state == WHIRLWIND;
}
bool IsFlaming(void)
{
    return (*(byte*)0x800786E8) > 0 && (*(byte*)0x800786E8) < 4; // Not actually has flamed bool, but instead the flame anim timer, so making a func instead of a var
}

//! Every Frame Update
void CheckMiscTimerUpdate(void)
{
    // Should count up
    if (misc_display_timer > 0)
    {

        misc_display_timer++;
    }

    // Should start counting land
    if (_spyro.isGrounded == true && (misc_display_timer == 0 || has_landed == false) && il_display_modes.il_display_landing)
    {
        misc_display_timer = 1;
        has_landed = true;
    }
    else if (_spyro.isGrounded == false && has_landed == true)
    {
        has_landed = false;
    }

    // Should start counting land glide
    if (_spyro.state == GLIDE && (misc_display_timer == 0 || has_glided == false) && il_display_modes.il_display_glide)
    {
        misc_display_timer = 1;
        has_glided = true;
    }
    else if (_spyro.state != GLIDE && has_glided == true)
    {
        has_glided = false;
    }

    // Should start counting whirlwind
    if (_spyro.state == WHIRLWIND && (misc_display_timer == 0 || has_entered_whirlwind == false) && il_display_modes.il_display_whirlwind)
    {
        misc_display_timer = 1;
        has_entered_whirlwind = true;
    }
    else if (_spyro.state != WHIRLWIND && has_entered_whirlwind == true)
    {
        has_entered_whirlwind = false;
    }

    // Should start counting bonk
    if (_spyro.state == BONK && (misc_display_timer == 0 || has_bonked == false) && il_display_modes.il_display_bonk)
    {
        misc_display_timer = 1;
        has_bonked = true;
    }
    else if (_spyro.state != BONK && has_bonked == true)
    {
        has_bonked = false;
    }

    // Should start counting flame
    if (IsFlaming() && (misc_display_timer == 0 || has_flamed == false) && il_display_modes.il_display_flame)
    {
        misc_display_timer = 1;
        has_flamed = true;
    }
    else if (_spyro.state != BONK && has_flamed == true)
    {
        has_flamed = false;
    }

    // Reset collectable bool. Gets set to true in hook
    else if (has_collected_item == true)
    {
        has_collected_item = false;
    }
    // Reset flight collectable bool. Gets set to true in hook
    else if (has_collected_flight_item == true)
    {
        has_collected_flight_item = false;
    }

    // Should reset
    else if (IsInMiscState() == false && misc_display_timer > 40)
    {
        misc_display_timer = 0;
    }

}

bool ShouldSaveMiscTime(void)
{
    if (misc_display_timer == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ShouldDisplayMiscTime(void)
{
    if (misc_display_timer > 0 && misc_display_timer <= 40)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Hooks for timer conditions
void CollectCollectableHook(void) // Hooking into CollectCollectable to show time when a gem/other collectable has been collected.
{
    // Should start counting collected something
    if ((misc_display_timer == 0 || has_collected_item == false) && il_display_modes.il_display_gem)
    {
        misc_display_timer = 1;
        has_collected_item = true;
    }
    return;
}
void CollectFlightItemHook(void) // Hooking into CollectFlightItem to show time when a gem/other collectable has been collected.
{
    // Should start counting collected something
    if ((misc_display_timer == 0 || has_collected_flight_item == false) && il_display_modes.il_display_flight)
    {
        misc_display_timer = 1;
        has_collected_flight_item = true;
    }
    return;
}

