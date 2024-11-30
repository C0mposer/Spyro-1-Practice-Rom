#include <common.h>
#include <spyro.h>

int misc_display_timer = 0;
bool has_entered_new_state = false;
bool has_bonked = false;
bool has_flamed = false;
bool has_collected_item = false;
bool has_collected_flight_item = false;

bool IsInMiscState(void)
{
    return _spyro.isGrounded == true || _spyro.state == GLIDE || _spyro.state == BONK || _spyro.state == WHIRLWIND;
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

    // Should start counting
    if (IsInMiscState() == true && (misc_display_timer == 0 || has_entered_new_state == false))
    {
        misc_display_timer = 1;
        has_entered_new_state = true;
    }
    else if (IsInMiscState() == false && has_entered_new_state == true)
    {
        has_entered_new_state = false;
    }

    // Should start counting bonk
    if (_spyro.state == BONK && (misc_display_timer == 0 || has_bonked == false))
    {
        misc_display_timer = 1;
        has_bonked = true;
    }
    else if (_spyro.state != BONK && has_bonked == true)
    {
        has_bonked = false;
    }

    // Should start counting flame
    if (IsFlaming() && (misc_display_timer == 0 || has_flamed == false))
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
    else if (IsInMiscState() == false && misc_display_timer > 30)
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
    if (misc_display_timer == 0 || has_collected_item == false)
    {
        misc_display_timer = 1;
        has_collected_item = true;
    }
    return;
}
void CollectFlightItemHook(void) // Hooking into CollectFlightItem to show time when a gem/other collectable has been collected.
{
    // Should start counting collected something
    if (misc_display_timer == 0 || has_collected_flight_item == false)
    {
        misc_display_timer = 1;
        has_collected_flight_item = true;
    }
    return;
}

