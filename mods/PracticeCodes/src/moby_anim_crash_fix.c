#include <common.h>

int frames_unloaded_moby_anim_function = 6;

void MobyAnimCrashFix(void)
{
    //NOP MobyAnimationUpdate
    if (frames_unloaded_moby_anim_function < 5)
    {
        //*((int*)0x800522c0) = 0x03e00008;
        //*((int*)0x800522c4) = 0x0;

        frames_unloaded_moby_anim_function++;
    }
    else if (frames_unloaded_moby_anim_function == 5)
    {
        //Bring Back MobyAnimationUpdate
        *((int*)0x800522c0) = 0x3C018007;
        *((int*)0x800522c4) = 0x2421FCF4;
        frames_unloaded_moby_anim_function = 6;
    }

    return;
}