#include <common.h>
#include <multitap.h>
#include <spyro.h>

extern int UpdateSpyroAnim;
extern int UpdateSpyroAnim_2;
//#define OPCODE2 0x8C428AB4;

#define SUBSTATE_SUPERCHARGE 0xC0

#define OPCODE1 0x3C028008;

#define OPCODE2_NTSC 0x8C428AB4; 

bool isSupercharging = false;

bool IsInAnySuperchargeSubState(void)
{
    return (_spyro.state == SUPERCHARGE || _spyro.state == CHARGING_AIR_SC_JUMP || _spyro.state == CHARGING_AIR);
}

void DisableUpdateSpyroAnimation(void)
{
    // Make UpdateSpyroAnim return immediately
    UpdateSpyroAnim = JR_RA;
    UpdateSpyroAnim_2 = NOP;
}
void EnableUpdateSpyroAnimation(void)
{
    // Set original opcodes for UpdateSpyroAnim
    UpdateSpyroAnim = OPCODE1;
    UpdateSpyroAnim_2 = OPCODE2_NTSC;

}

void SuperchargeUpdate(void)
{
    if (CheckButtonMultiTap(SQUARE_BUTTON, 2) && _spyro.state == CHARGING_AIR)
    {
        isSupercharging = true;
    }

    else if (!IsInAnySuperchargeSubState())
    {
        isSupercharging = false;
    }

    if (isSupercharging)
    {

        _spyro.subState = SUBSTATE_SUPERCHARGE;

        DisableUpdateSpyroAnimation();

        // Update Speed
        if (_currentButtonOneFrame & R1_BUTTON)
        {
            _spyro.superchargeHeightStarted += 2000;
        }
        else if (_currentButtonOneFrame & L1_BUTTON)
        {
            _spyro.superchargeHeightStarted -= 2000;
        }


    }
    else if (!isSupercharging)
    {
        EnableUpdateSpyroAnimation();
    }
}