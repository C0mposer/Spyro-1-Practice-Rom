#include <common.h>
#include <moby.h>

extern int _1st_loot_key_collected;
extern Moby _1st_loot_key;

extern int _2nd_loot_key_collected;
extern Moby _2nd_loot_key;

extern int _3rd_loot_key_collected;
extern Moby _3rd_loot_key;

//! Every Frame Update
void LootPlaneUpdate(void)
{
    if (_levelID == GNASTYS_LOOT_ID)
    {
        // First Plane Practice
        if (_currentButton == L1_BUTTON + R1_BUTTON + SQUARE_BUTTON)
        {
            _spyro.position.x = 0x139B8;
            _spyro.position.y = 0x13B97;
            _spyro.position.z = 0x4967;
            _spyro.angle.yaw = 0x610;

            _cameraPosition.x = 0x14073;
            _cameraPosition.y = 0x1350F;
            _cameraPosition.z = 0x48D3;
            _cameraAngle.yaw = 0x609;

            _2nd_loot_key_collected = 1;

            _2nd_loot_key.keyPickedUp = -1;

        }


        // Second Plane Practice
        if (_currentButton == L1_BUTTON + R1_BUTTON + X_BUTTON)
        {
            _spyro.position.x = 0x17BC9;
            _spyro.position.y = 0xC891;
            _spyro.position.z = 0x516A;
            _spyro.angle.yaw = 0xEB0;

            _cameraPosition.x = 0x17365;
            _cameraPosition.y = 0xCD51;
            _cameraPosition.z = 0x53D4;
            _cameraAngle.yaw = 0xEB0;

            _3rd_loot_key_collected = 1;

            _3rd_loot_key.keyPickedUp = -1;
        }
    }
}

void LootGiveAllKeys(void)
{
    _1st_loot_key_collected = 1;
    _1st_loot_key.keyPickedUp = -1;

    _2nd_loot_key_collected = 1;
    _2nd_loot_key.keyPickedUp = -1;

    _3rd_loot_key_collected = 1;
    _3rd_loot_key.keyPickedUp = -1;
}