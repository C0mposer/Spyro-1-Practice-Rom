#include <common.h>
#include <cd.h>

#define SPLASH_1_RAM_AREA 0x8007DDE8
#define SPLASH_2_RAM_AREA 0x8014a800

bool has_loaded_splash_1 = false;
bool has_loaded_splash_2 = false;

int* test = 0;

void SplashScreen1(void)
{
    if (!has_loaded_splash_1)
    {
        #if BUILD == PS1 || BUILD == PS2_IOP
        LoadCdData(265774, SPLASH_1_RAM_AREA, 0x59000, 0, 600);     //Loads kern.bin
        #elif BUILD == PS2_DECKARD
        LoadCdData(265776, SPLASH_1_RAM_AREA, 0x59000, 0, 600);     //Loads kern.bin
        #endif

        has_loaded_splash_1 = true;
    }

    return;
}

void SplashScreen2(void)
{
    if (!has_loaded_splash_2)
    {
        #if BUILD == PS1 || BUILD == PS2_IOP
        LoadCdData(265954, SPLASH_2_RAM_AREA, 0x59000, 0, 600);     //Loads kern.bin
        #elif BUILD == PS2_DECKARD
        LoadCdData(265956, SPLASH_2_RAM_AREA, 0x59000, 0, 600);     //Loads kern.bin
        #endif

        has_loaded_splash_2 = true;
    }

    return;
}