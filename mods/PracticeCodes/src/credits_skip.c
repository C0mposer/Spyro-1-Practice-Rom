#include <common.h>
#include <moby.h>
#include <credits_skip.h>

extern int gnasty_chase_state;
extern void* ptr_gnasty_end_fight;
extern void* path_ptr;

//! Every Frame Update
void CreditsSkipUpdate()
{
    if(_levelID == GNASTY_GNORC_ID)
    {
        if (_currentButton == L1_BUTTON + R1_BUTTON + X_BUTTON)
        {
            // Set gnasty's position to the end of the fight
            gnasty.position.x = 0x6F73;
            gnasty.position.y = 0x55BA;
            gnasty.position.z = 0x3300;

            // Set gnasty's state to his final hit state
            gnasty.state = 0xC;

            // Set the chase state to be the last hit of the fight
            gnasty_chase_state = 0x3;

            // Some pointer needs to be set to a pointer to gnasty, for the last hit?
            ptr_gnasty_end_fight = &gnasty;

            gnasty.type = 0x00A1;
            gnasty.subType = 0x0043;

            path_ptr = (void*)0x8016AEC4;

            // Set spyro's position to the end of the fight
            _spyro.position.x = 0x643C;
            _spyro.position.y = 0x7DE3;
            _spyro.position.z = 0x3166;
            _spyro.angle.yaw = 0x0C90;
            _spyro.health = YELLOW_SPARX;
            _spyroInvisible = false;

            _cameraPosition.x = 0x627F;
            _cameraPosition.y = 0x851A;
            _cameraPosition.z = 0x33A7;
            _cameraAngle.yaw = 0x0CBC;

            _isInInGameCutscene = false;
            _effect_ScreenLetterBox = 0;
            
        }
    }
}