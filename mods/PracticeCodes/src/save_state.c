#include <common.h>
#include <levelselect.h>
#include <shared_funcs.h>
#include <multitap.h>
#include <moving_geo.h>

int appliedNopFixTimer = 0;

// from save_state_region.c
extern byte* mem_region;

// from IGT.c
extern int savestate_selection;
// from main_updates.c
//extern bool hasSavedSpyro;
extern int local_level_id;
extern savestated_level_ids[3];
//from moby_anim_crash_fix.c
extern frames_unloaded_moby_anim_function;


void FullSaveState(void)
{
    SetMemoryRegion();

    savestated_level_ids[savestate_selection] = _levelID;

    byte* local_mem_region = (byte*)mem_region; // Local region for adding to

    // Copy all level mobys, dynamic mobys, and moby data to the free DECKARD IOP area
    memcpy((int*)local_mem_region, (byte*)_ptr_levelMobys, 0x10000);
    local_mem_region += 0x10000;

    // Copy other misc data to the DECKARD IOP area
    memcpy((int*)local_mem_region, &_ptr_dynamicLevelMobys, sizeof(int*));
    local_mem_region += sizeof(int*);

    memcpy((int*)local_mem_region, &_current_number_of_dynamic_mobys, sizeof(int*));
    local_mem_region += sizeof(int*);

    memcpy((int*)local_mem_region, &_ptr_levelMobyData, sizeof(int*));
    local_mem_region += sizeof(int*);

    memcpy((int*)local_mem_region, &_ptr_sparx, sizeof(int*));
    local_mem_region += sizeof(int*);

    memcpy((int*)local_mem_region, &_ptr_sparx2, sizeof(int*));
    local_mem_region += sizeof(int*);

    memcpy((int*)local_mem_region, &_doesHaveSuperflame, sizeof(int));
    local_mem_region += sizeof(int);

    memcpy((int*)local_mem_region, &_superflameTimer, sizeof(int));
    local_mem_region += sizeof(int);


    memcpy((int*)local_mem_region, &_levelGemsCollectedArray, sizeof(_levelGemsCollectedArray));
    local_mem_region += sizeof(_levelGemsCollectedArray);

    memcpy((int*)local_mem_region, &_globalEggs, sizeof(int));
    local_mem_region += sizeof(int);
    memcpy((int*)local_mem_region, &_globalGems, sizeof(int));
    local_mem_region += sizeof(int);

    memcpy((int*)local_mem_region, &_globalDragons, sizeof(int));
    local_mem_region += sizeof(int);

    memcpy((int*)local_mem_region, &_globalFlightLevelTimer, sizeof(int));
    local_mem_region += sizeof(int);

    memcpy((int*)local_mem_region, &_globalFlightLevelCollectedItemsArray, sizeof(_globalFlightLevelCollectedItemsArray));
    local_mem_region += sizeof(_globalFlightLevelCollectedItemsArray);

    // Copy general hud timer area
    memcpy(local_mem_region, (void*)0x80077F20, 0x500);
    local_mem_region += 0x500;

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

    memcpy((byte*)local_mem_region, &_effect_ScreenFadeIn, 0x4);
    local_mem_region += 0x4;

    memcpy((byte*)local_mem_region, &_spyroInvisible, 0x4);
    local_mem_region += 0x4;

    //Whirlwind Data
    memcpy((byte*)local_mem_region, 0x80075668, 0x4);
    local_mem_region += 0x4;
    memcpy((byte*)local_mem_region, 0x80075724, 0x4);
    local_mem_region += 0x4;

    //Electric Pads in BM/Terrace
    memcpy((byte*)local_mem_region, _electricPadActivations, (sizeof(_electricPadActivations)));
    local_mem_region += sizeof(_electricPadActivations);

    // Electric Pad Visuals
    if (_levelID == BEAST_MAKERS_ID)
    {
        //Electric Pads Visuals BM
        memcpy((byte*)local_mem_region, 0x80178E88, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x80178FD4, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x80179120, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x8017923C, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x80179358, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x80179474, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x801795A8, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x801796C4, 0x4);
        local_mem_region += 0x4;
    }
    if (_levelID == TERRACE_VILLAGE_ID)
    {
        //Electric Pads Visuals Terrace
        memcpy((byte*)local_mem_region, 0x80167CC4, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x80168F80, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x801699C0, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x8016ADE4, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x8016BEF0, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x80170710, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x80170B5C, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x8017288C, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x80174694, 0x4);
        local_mem_region += 0x4;

        memcpy((byte*)local_mem_region, 0x80174FE4, 0x4);
        local_mem_region += 0x4;
    }

    if (_levelID != GNASTYS_WORLD_ID) // Gnasty's World has by far the most moving collision, but as it currently stands we so no reason to save it, as it doesn't move in any relavent way
    {
        SaveGeoData(local_mem_region);
    }
    //hasSavedSpyro = true;

    //printf("%X\n\n\n", local_mem_region);
}

void FullLoadState(void)
{
    SetMemoryRegion();

    byte* local_mem_region = (byte*)mem_region; // Local region for adding to

    if ((int*)local_mem_region != NULL)
    {
        if (savestated_level_ids[savestate_selection] == _levelID)
        {
                  // Reload all level mobys, dynamic mobys, and moby data from the free DECKARD IOP area
            memcpy((byte*)_ptr_levelMobys, local_mem_region, 0x10000);
            local_mem_region += 0x10000;

            // Reload other misc data from the DECKARD IOP area
            memcpy(&_ptr_dynamicLevelMobys, local_mem_region, sizeof(int*));
            local_mem_region += sizeof(int*);

            memcpy(&_current_number_of_dynamic_mobys, (int*)local_mem_region, sizeof(int*));
            local_mem_region += sizeof(int*);

            memcpy(&_ptr_levelMobyData, local_mem_region, sizeof(int*));
            local_mem_region += sizeof(int*);

            memcpy(&_ptr_sparx, local_mem_region, sizeof(int*));
            local_mem_region += sizeof(int*);

            memcpy(&_ptr_sparx2, local_mem_region, sizeof(int*));
            local_mem_region += sizeof(int*);

            memcpy(&_doesHaveSuperflame, local_mem_region, sizeof(int*));
            local_mem_region += sizeof(int*);

            memcpy(&_superflameTimer, local_mem_region, sizeof(int*));
            local_mem_region += sizeof(int*);

            memcpy(&_levelGemsCollectedArray, local_mem_region, sizeof(_levelGemsCollectedArray));
            local_mem_region += sizeof(_levelGemsCollectedArray);

            memcpy(&_globalEggs, local_mem_region, sizeof(int));
            local_mem_region += sizeof(int);

            memcpy(&_globalGems, local_mem_region, sizeof(int));
            local_mem_region += sizeof(int);

            memcpy(&_globalDragons, local_mem_region, sizeof(int));
            local_mem_region += sizeof(int);

            memcpy(&_globalFlightLevelTimer, local_mem_region, sizeof(int));
            local_mem_region += sizeof(int);

            memcpy(&_globalFlightLevelCollectedItemsArray, local_mem_region, sizeof(_globalFlightLevelCollectedItemsArray));
            local_mem_region += sizeof(_globalFlightLevelCollectedItemsArray);

            // Copy general hud timer area
            memcpy((void*)0x80077F20, local_mem_region, 0x500);
            local_mem_region += 0x500;

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

            memcpy(&_effect_ScreenFadeIn, (byte*)local_mem_region, 0x4);
            local_mem_region += 0x4;

            memcpy(&_spyroInvisible, (byte*)local_mem_region, 0x4);
            local_mem_region += 0x4;

            //Whirlwind Data
            memcpy(0x80075668, (byte*)local_mem_region, 0x4);
            local_mem_region += 0x4;
            memcpy(0x80075724, (byte*)local_mem_region, 0x4);
            local_mem_region += 0x4;

            //Electric Pads in BM/Terrace
            memcpy(_electricPadActivations, (byte*)local_mem_region, (sizeof(_electricPadActivations)));
            local_mem_region += sizeof(_electricPadActivations);

            //printf("%X\n\n\n", local_mem_region);

            // Electric Pad Visuals
            if (_levelID == BEAST_MAKERS_ID)
            {
                //Electric Pads Visuals BM
                memcpy(0x80178E88, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x80178FD4, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x80179120, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x8017923C, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x80179358, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x80179474, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x801795A8, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x801796C4, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;
            }
            if (_levelID == TERRACE_VILLAGE_ID)
            {
                //Electric Pads Visuals Terrace
                memcpy(0x80167CC4, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x80168F80, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x801699C0, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x8016ADE4, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x8016BEF0, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x80170710, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x80170B5C, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x8017288C, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x80174694, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;

                memcpy(0x80174FE4, (byte*)local_mem_region, 0x4);
                local_mem_region += 0x4;
            }

            if (_levelID != GNASTYS_WORLD_ID) // Gnasty's World has by far the most moving collision, but as it currently stands we so no reason to save it, as it doesn't move in any relavent way
            {
                LoadGeoData(local_mem_region);
            }

            LoadstateNopFixes();
        }
    }
}

void LoadstateNopFixes(void)
{
    *(int*)0x80056528 = 0x00000000;					// NOP-ing the Vec3Length call in the SFX proccessing function. This fixes a weird bug with some specific sound sources crashing right after a loadstate

    appliedNopFixTimer = 1;
}

void RevertLoadstateNOPFixes(void)
{
    if (appliedNopFixTimer == 10)
        *(int*)0x80056528 = 0x0C005C7F;

    if (appliedNopFixTimer > 0)
    {
        appliedNopFixTimer++;
    }
}

// Check to revert NOP's every frame
void LoadstateFixesUpdate()
{
    RevertLoadstateNOPFixes();
}