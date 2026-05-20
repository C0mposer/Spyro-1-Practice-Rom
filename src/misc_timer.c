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

typedef struct MiscTimerFlags
{
    bool has_landed : 1;
    bool has_glided : 1;
    bool has_entered_whirlwind : 1;
    bool has_bonked : 1;
    bool has_flamed : 1;
    bool has_collected_item : 1;
    bool has_collected_flight_item : 1;
} MiscTimerFlags;
MiscTimerFlags misc_timer_flags;



int misc_display_timer = 0;

static inline bool IsInMiscState(void)
{
    return _spyro.isGrounded == true || _spyro.state == GLIDE || _spyro.state == WHIRLWIND;
}
static inline bool IsFlaming(void)
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
    if (_spyro.isGrounded == true && (misc_display_timer == 0 || !misc_timer_flags.has_landed) && il_display_modes.il_display_landing)
    {
        misc_display_timer = 1;
        misc_timer_flags.has_landed = true;
    }
    else if (_spyro.isGrounded == false && misc_timer_flags.has_landed)
    {
        misc_timer_flags.has_landed = false;
    }

    // Should start counting land glide
    if (_spyro.state == GLIDE && (misc_display_timer == 0 || !misc_timer_flags.has_glided) && il_display_modes.il_display_glide)
    {
        misc_display_timer = 1;
        misc_timer_flags.has_glided = true;
    }
    else if (_spyro.state != GLIDE && misc_timer_flags.has_glided)
    {
        misc_timer_flags.has_glided = false;
    }

    // Should start counting whirlwind
    if (_spyro.state == WHIRLWIND && (misc_display_timer == 0 || !misc_timer_flags.has_entered_whirlwind) && il_display_modes.il_display_whirlwind)
    {
        misc_display_timer = 1;
        misc_timer_flags.has_entered_whirlwind = true;
    }
    else if (_spyro.state != WHIRLWIND && misc_timer_flags.has_entered_whirlwind)
    {
        misc_timer_flags.has_entered_whirlwind = false;
    }

    // Should start counting bonk
    if (_spyro.state == BONK && (misc_display_timer == 0 || !misc_timer_flags.has_bonked) && il_display_modes.il_display_bonk)
    {
        misc_display_timer = 1;
        misc_timer_flags.has_bonked = true;
    }
    else if (_spyro.state != BONK && misc_timer_flags.has_bonked)
    {
        misc_timer_flags.has_bonked = false;
    }

    // Should start counting flame
    if (IsFlaming() && (misc_display_timer == 0 || !misc_timer_flags.has_flamed) && il_display_modes.il_display_flame)
    {
        misc_display_timer = 1;
        misc_timer_flags.has_flamed = true;
    }
    else if (!IsFlaming() && misc_timer_flags.has_flamed)
    {
        misc_timer_flags.has_flamed = false;
    }

    // Reset collectable bool. Gets set to true in hook
    else if (misc_timer_flags.has_collected_item)
    {
        misc_timer_flags.has_collected_item = false;
    }
    // Reset flight collectable bool. Gets set to true in hook
    else if (misc_timer_flags.has_collected_flight_item)
    {
        misc_timer_flags.has_collected_flight_item = false;
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
    if ((misc_display_timer == 0 || !misc_timer_flags.has_collected_item) && il_display_modes.il_display_gem)
    {
        misc_display_timer = 1;
        misc_timer_flags.has_collected_item = true;
    }
    return;
}
void CollectFlightItemHook(void) // Hooking into CollectFlightItem to show time when a gem/other collectable has been collected.
{
    // Should start counting collected something
    if ((misc_display_timer == 0 || !misc_timer_flags.has_collected_flight_item) && il_display_modes.il_display_flight)
    {
        misc_display_timer = 1;
        misc_timer_flags.has_collected_flight_item = true;
    }
    return;
}

