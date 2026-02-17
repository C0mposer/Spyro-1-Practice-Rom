#include <common.h>
#include <spyro.h>

void MaybeUpdateShadowPosition(int param_1)

{
    int iVar1;
    int local_20;
    int local_1c;
    int local_18;

    local_20 = 0;
    local_1c = 0;
    local_18 = -0x164;
    Vec3ApplyRotationMatrix(0x80078a8c, &local_20, &local_20);
    Vec3Add(&local_20, &local_20, &_spyro);
    Vec3Copy(0x8007aa20, &local_20);
    *(int*)0x8007aa28 = _spyro.unkown_heightRelated;
    if ((byte)_spyro.unkown3 < 0x7f) {
        _spyro.unkown3 = '\x05';
    }
    *(int*)0x8007aa2c = 3;
    if (0x364 < _spyro.position.z - _spyro.unkown_heightRelated) {
        *(int*)0x8007aa2c = 5;
    }
    *(int*)0x8007aa34 = 0;
    if ((_spyro.unkown_heightRelated <= *(int*)(0x8006e9a4 + *(int*)0x80075964 * 4)) ||
        (_spyro.isOverGoop != 0)) {
        *(int*)0x8007aa34 = 1;
    }
    *(int*)0x8007aa30 = *(int*)0x8007aa30 + 1 & 7;
    if (param_1 == 0) {
        *(char*)((int)0x8007aa18 + *(int*)0x8007aa30) = 0;
    }
    else {
        Vec3ApplyRotationMatrix(0x80078a8c, *(int*)0x8007aa30 * 0xc + -0x7ff91d58, &local_20);
        Vec3Add(&local_20, &local_20, &_spyro);
        local_18 = local_18 + 0x200;
        iVar1 = MobyUpdatePosition(&local_20, 0x400);
        if ((local_18 - iVar1) - 0x180U < 0x100) {
            *(char*)((int)0x8007aa18 + *(int*)0x8007aa30) = 0;
            iVar1 = (*(int*)0x8007aa28 - iVar1) * 0xc2;
            if (iVar1 < 0) {
                iVar1 = iVar1 + 0x1ff;
            }
            *(char*)((int)0x8007aa10 + *(int*)0x8007aa30) = (char)(iVar1 >> 9);
            return;
        }
        *(char*)((int)0x8007aa18 + *(int*)0x8007aa30) = 1;
    }
    *(char*)((int)0x8007aa10 + *(int*)0x8007aa30) = 0;
    return;
}
