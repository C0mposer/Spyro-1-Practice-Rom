#include <common.h>
#include <multitap.h>
#include <spyro.h>

bool isSupercharging = false;

bool IsInAnySuperchargeSubState(void)
{
    return (_spyro.state == SUPERCHARGE || _spyro.state == CHARGING_AIR_SC_JUMP || _spyro.state == CHARGING_AIR);
}

void SuperchargeUpdate(void)
{
    if (CheckButtonMultiTap(SQUARE_BUTTON, 3))
    {
        isSupercharging = true;
    }

    else if (!IsInAnySuperchargeSubState())
    {
        isSupercharging = false;
    }

    if (isSupercharging)
    {
        _spyro.subState = 0xC0;

        *(int*)(0x8003CCE4) = 0x03E00008;
        *(int*)(0x8003CCE8) = 0x0;
        

    }
    else if (!isSupercharging)
    {
        *(int*)(0x8003CCE4) = 0x3C028008;
        *(int*)(0x8003CCE8) = 0x8C428AB4;
    }
}