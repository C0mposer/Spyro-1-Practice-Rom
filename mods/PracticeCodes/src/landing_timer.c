#include <common.h>
#include <spyro.h>

int landing_display_timer = 0;


void CheckLandingUpdate(void)
{
    // Should cound up
    if (landing_display_timer > 0)
    {
        
        landing_display_timer++;
    }

    // Should start counting
    if (_spyro.isGrounded == true && landing_display_timer == 0)
    {
        landing_display_timer = 1;
    }
    
    // Should reset
    else if (_spyro.isGrounded == false && landing_display_timer > 30)
    {
        landing_display_timer = 0;
    }

}

bool ShouldSaveLandingTime(void)
{
    if (landing_display_timer == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ShouldDisplayLandingTime(void)
{
    if (landing_display_timer > 0 && landing_display_timer <= 30)
    {
        return true;
    }
    else
    {
        return false;
    }
}
