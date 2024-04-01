#include <common.h>
#include <levelselect.h>
#include <shared_funcs.h>
#include <multitap.h>
#include <moving_geo.h>

byte* mem_region = STARTING_MEM;

// from IGT.c
extern int savestate_selection;
// from main_updates.c
extern bool hasSavedSpyro;


// Choose the memory region to save/load a state based on the menu option. Each region takes up ~0x11000. Being safe because of collision/texture data
void SetMemoryRegion(void)
{
    switch (savestate_selection)
    {
      case 0:
        mem_region = STARTING_MEM;
        break;
      case 1:
        mem_region = STARTING_MEM + 0x13000;
        break;
      case 2:
        mem_region = STARTING_MEM + 0x26000;
        break;
      default:
        mem_region = STARTING_MEM;
    }
}

void SaveStateTest(void)
{

    SetMemoryRegion();
    
    byte* local_mem_region = (byte*)mem_region; // Local region for adding to

    // Copy all level mobys, dynamic mobys, and moby data to the free DECKARD IOP area
    memcpy((int*)local_mem_region, (byte*)_ptr_levelMobys, 0x10000);
    local_mem_region += 0x10000;

    // Copy other misc data to the DECKARD IOP area
    memcpy((int*)local_mem_region,       &_ptr_dynamicLevelMobys,     sizeof(int*));
    local_mem_region += sizeof(int*);

    memcpy((int*)local_mem_region,   &_ptr_levelMobyData,         sizeof(int*));
    local_mem_region += sizeof(int*);

    memcpy((int*)local_mem_region,   &_ptr_sparx,                 sizeof(int*));
    local_mem_region += sizeof(int*);

    memcpy((int*)local_mem_region,   &_ptr_sparx2,                sizeof(int*));
    local_mem_region += sizeof(int*);

    memcpy((int*)local_mem_region,   &_doesHaveSuperflame,        sizeof(int));
    local_mem_region += sizeof(int);

    memcpy((int*)local_mem_region,   &_superflameTimer,           sizeof(int));
    local_mem_region += sizeof(int);


    memcpy((int*)local_mem_region,   &_levelGemsCollectedArray,   sizeof(_levelGemsCollectedArray));
    local_mem_region += sizeof(_levelGemsCollectedArray);

    memcpy((int*)local_mem_region,   &_globalEggs,            sizeof(int));
    local_mem_region += sizeof(int);
    memcpy((int*)local_mem_region,   &_globalGems,                sizeof(int));
    local_mem_region += sizeof(int);

    memcpy((int*)local_mem_region,   &_globalDragons,             sizeof(int));
    local_mem_region += sizeof(int);

    memcpy((int*)local_mem_region, &_globalFlightLevelTimer,    sizeof(int));
    local_mem_region += sizeof(int);

    memcpy((int*)local_mem_region, &_globalFlightLevelCollectedItemsArray, sizeof(_globalFlightLevelCollectedItemsArray));
    local_mem_region += sizeof(_globalFlightLevelCollectedItemsArray);

        // Copy general hud timer area
    memcpy((int*)local_mem_region,      (void*)0x80077F20, 0x260);
    local_mem_region += 0x260;

    // Copy spyro (moved from other area)
    memcpy((byte*)local_mem_region, &_spyro, sizeof(_spyro) + 0x100);
    local_mem_region += sizeof(_spyro) + 0x100;

    // Copy Flame Data
    memcpy((byte*)local_mem_region, (byte*)0x800786C8, 0x148);
    local_mem_region += 0x148;

    memcpy((byte*)local_mem_region, &_cameraStart, 0xFF);
    local_mem_region += 0xFF;

    memcpy((byte*)local_mem_region, &_keyState, 0x4);
    local_mem_region += 0x4;

    memcpy((byte*)local_mem_region, &_isInInGameCutscene, 0x4);
    local_mem_region += 0x4;

    memcpy((byte*)local_mem_region, &_effect_ScreenLetterBox, 0x4);
    local_mem_region += 0x4;

    memcpy((byte*)local_mem_region, &_spyroInvisible, 0x4);
    local_mem_region += 0x4;

    SaveGeoData();

    hasSavedSpyro = true;

    printf("%X\n\n\n", local_mem_region);
}

void LoadStateTest(void)
{
    SetMemoryRegion();

    byte* local_mem_region = (byte*)mem_region; // Local region for adding to

    if (*local_mem_region != NULL)
    {
        // Reload all level mobys, dynamic mobys, and moby data from the free DECKARD IOP area
        memcpy( (byte*)_ptr_levelMobys, local_mem_region, 0x10000);
        local_mem_region += 0x10000;

        // Reload other misc data from the DECKARD IOP area
        memcpy(&_ptr_dynamicLevelMobys,                 local_mem_region,     sizeof(int*));
        local_mem_region += sizeof(int*);

        memcpy(&_ptr_levelMobyData,                     local_mem_region, sizeof(int*));
        local_mem_region += sizeof(int*);

        memcpy(&_ptr_sparx,                             local_mem_region, sizeof(int*));
        local_mem_region += sizeof(int*);

        memcpy(&_ptr_sparx2,                            local_mem_region, sizeof(int*));
        local_mem_region += sizeof(int*);

        memcpy(&_doesHaveSuperflame,                    local_mem_region, sizeof(int*));
        local_mem_region += sizeof(int*);
        
        memcpy(&_superflameTimer,                       local_mem_region, sizeof(int*));
        local_mem_region += sizeof(int*);

        memcpy(&_levelGemsCollectedArray,               local_mem_region, sizeof(_levelGemsCollectedArray));
        local_mem_region += sizeof(_levelGemsCollectedArray);

        memcpy(&_globalEggs,                            local_mem_region, sizeof(int));
        local_mem_region += sizeof(int);

        memcpy(&_globalGems,                            local_mem_region, sizeof(int));
        local_mem_region += sizeof(int);

        memcpy(&_globalDragons,                         local_mem_region, sizeof(int));
        local_mem_region += sizeof(int);

        memcpy(&_globalFlightLevelTimer,                local_mem_region, sizeof(int));
        local_mem_region += sizeof(int);

        memcpy(&_globalFlightLevelCollectedItemsArray,  local_mem_region, sizeof(_globalFlightLevelCollectedItemsArray));
        local_mem_region += sizeof(_globalFlightLevelCollectedItemsArray);

        // Copy general hud timer area
        memcpy((void*)0x80077F20, local_mem_region, 0x260);
        local_mem_region += 0x260;

        // Copy spyro (moved from other area)
        memcpy(&_spyro, local_mem_region, sizeof(_spyro) + 0x100);
        local_mem_region += sizeof(_spyro) + 0x100;

        // Copy Flame Data
        memcpy((byte*)0x800786C8, (byte*)local_mem_region, 0x148);
        local_mem_region += 0x148;

        memcpy(&_cameraStart, local_mem_region, 0xFF);
        local_mem_region += 0xFF;

        memcpy(&_keyState, local_mem_region, 0x4);
        local_mem_region += 0x4;
       
        memcpy(&_isInInGameCutscene, (byte*)local_mem_region, 0x4);
        local_mem_region += 0x4;

        memcpy(&_effect_ScreenLetterBox, (byte*)local_mem_region, 0x4);
        local_mem_region += 0x4;

        memcpy(&_spyroInvisible, (byte*)local_mem_region, 0x4);
        local_mem_region += 0x4;

        //printf("%X\n\n\n", local_mem_region);
        LoadGeoData();
    }
}