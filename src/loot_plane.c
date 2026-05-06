#include <common.h>
#include <moby.h>

extern int _1st_loot_key_collected;
extern Moby _1st_loot_key;

extern int _2nd_loot_key_collected;
extern Moby _2nd_loot_key;

extern int _3rd_loot_key_collected;
extern Moby _3rd_loot_key;

typedef struct { int spx, spy, spz, syaw, cpx, cpy, cpz, cyaw; } LootPlanePos;

static const LootPlanePos loot_positions[2] = {
    { 0x139B8, 0x13B97, 0x4967, 0x610, 0x14073, 0x1350F, 0x48D3, 0x609 },
    { 0x17BC9, 0xC891,  0x516A, 0xEB0, 0x17365, 0xCD51,  0x53D4, 0xEB0 }
};

static void ApplyLootPlanePos(const LootPlanePos* p)
{
    _spyro.position.x = p->spx;
    _spyro.position.y = p->spy;
    _spyro.position.z = p->spz;
    _spyro.angle.yaw  = p->syaw;
    _cameraPosition.x = p->cpx;
    _cameraPosition.y = p->cpy;
    _cameraPosition.z = p->cpz;
    _cameraAngle.yaw  = p->cyaw;
}

//! Every Frame Update
void LootPlaneUpdate(void)
{
    if (_levelID == GNASTYS_LOOT_ID)
    {
        // First Plane Practice
        if (_currentButton == L1_BUTTON + R1_BUTTON + SQUARE_BUTTON)
        {
            ApplyLootPlanePos(&loot_positions[0]);
            _2nd_loot_key_collected = 1;
            _2nd_loot_key.keyPickedUp = -1;
        }

        // Second Plane Practice
        if (_currentButton == L1_BUTTON + R1_BUTTON + X_BUTTON)
        {
            ApplyLootPlanePos(&loot_positions[1]);
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