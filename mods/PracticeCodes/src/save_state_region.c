#include <common.h>
#include <levelselect.h>
#include <shared_funcs.h>
#include <multitap.h>
#include <moving_geo.h>

// from IGT.c
extern int savestate_selection;

byte* mem_region = STARTING_EXTRA_MEM;

// from IGT.c
extern int savestate_selection;

// Choose the memory region to save/load a state based on the menu option. Each region takes up ~0x12000. Being safe because of collision/texture data, and doing 0x13000
void SetMemoryRegion(void)
{
    switch (savestate_selection)
    {
      case 0:
        mem_region = STARTING_EXTRA_MEM;
        break;
      case 1:
        mem_region = STARTING_EXTRA_MEM + 0x15000;
        break;
      case 2:
        mem_region = STARTING_EXTRA_MEM + 0x2A000;
        break;
      default:
        mem_region = STARTING_EXTRA_MEM;
    }
}