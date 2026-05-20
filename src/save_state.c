#include <common.h>
#include <levelselect.h>
#include <custom_text.h>
#include <shared_funcs.h>
#include <multitap.h>
#include <moving_geo.h>

enum { LOAD, SAVE };

void* g_save_state_bm_pad_addrs[8] = {
    (void*)0x80178E88, (void*)0x80178FD4, (void*)0x80179120, (void*)0x8017923C,
    (void*)0x80179358, (void*)0x80179474, (void*)0x801795A8, (void*)0x801796C4
};

void* g_save_state_terrace_pad_addrs[10] = {
    (void*)0x80167CC4, (void*)0x80168F80, (void*)0x801699C0, (void*)0x8016ADE4,
    (void*)0x8016BEF0, (void*)0x80170710, (void*)0x80170B5C, (void*)0x8017288C,
    (void*)0x80174694, (void*)0x80174FE4
};

// from save_state_region.c
extern byte* mem_region;

// from IGT.c
extern int savestate_selection;
// from main_updates.c 
extern int local_level_id;
extern savestated_level_ids[3];
//from moby_anim_crash_fix.c
extern frames_unloaded_moby_anim_function;

static void TransferField(byte** r, void* addr, int size, bool direction)
{
    if (direction == SAVE)
    {
        memcpy(*r, addr, size);
    }
    else
        memcpy(addr, *r, size);

    *r += size;
}

static byte* SaveOrLoadState(bool direction)
{
    byte* r = (byte*)mem_region;

    // Level mobys
    TransferField(&r, (byte*)_ptr_levelMobys, 0x10000, direction);

    // Misc pointers and counts
    TransferField(&r, &_ptr_dynamicLevelMobys, sizeof(int*), direction);
    TransferField(&r, &_current_number_of_dynamic_mobys, sizeof(int*), direction);
    TransferField(&r, &_ptr_levelMobyData, sizeof(int*), direction);
    TransferField(&r, &_ptr_sparx, sizeof(int*), direction);
    TransferField(&r, &_ptr_sparx2, sizeof(int*), direction);

    TransferField(&r, &_doesHaveSuperflame, sizeof(int), direction);
    TransferField(&r, &_superflameTimer, sizeof(int), direction);

    TransferField(&r, &_levelGemsCollectedArray, sizeof(_levelGemsCollectedArray), direction);

    TransferField(&r, &_globalEggs, sizeof(int), direction);
    TransferField(&r, &_globalGems, sizeof(int), direction);
    TransferField(&r, &_globalDragons, sizeof(int), direction);

    TransferField(&r, &_flightLevelExpirationTimer, sizeof(int), direction);
    TransferField(&r, &_flightLevelTimer, sizeof(int), direction);
    TransferField(&r, &_flightTargetHorizontalRotation, sizeof(int), direction);
    TransferField(&r, &_flightTargetVerticalRotation, sizeof(int), direction);
    TransferField(&r, &_flightRoll, sizeof(int), direction);

    TransferField(&r, &_globalFlightLevelCollectedItemsArray, sizeof(_globalFlightLevelCollectedItemsArray), direction);

    // General HUD timer area
    TransferField(&r, (void*)0x80077F20, 0x500, direction);

    // Spyro struct
    TransferField(&r, &_spyro, sizeof(_spyro) + 0x100, direction);

    // Flame data
    TransferField(&r, (void*)0x800786C8, 0x148, direction);

    TransferField(&r, &_cameraStart, 0xFF, direction);

    TransferField(&r, &_isInInGameCutscene, 0x4, direction);
    TransferField(&r, &_effect_ScreenLetterBox, 0x4, direction);
    TransferField(&r, &_effect_ScreenFadeIn, 0x4, direction);
    TransferField(&r, &_spyroInvisible, 0x4, direction);

    // Whirlwind data
    TransferField(&r, (void*)0x80075668, 0x4, direction);
    TransferField(&r, (void*)0x80075724, 0x4, direction);

    // Key data
    TransferField(&r, &_keyState, 0x4, direction);
    TransferField(&r, &_ptr_keyChestHostGem, 0x4, direction);

    // Particle data
    TransferField(&r, _ptr_particleBuffer, 0x2000, direction);
    TransferField(&r, &_ptr_nextFreeParticleSlot, 0x4, direction);

    // Respawn position and angle
    TransferField(&r, (void*)0x800778D8, 0x16, direction);
    TransferField(&r, (void*)0x800777A0, 0x4, direction);

    TransferField(&r, &_collectablesBitflags, 0x4B0, direction);

    // Electric pad activations (BM / Terrace)
    TransferField(&r, _electricPadActivations, sizeof(_electricPadActivations), direction);

    // Electric pad visuals (level-specific)
    if (_levelID == BEAST_MAKERS_ID)
    {
        for (int i = 0; i < 8; i++)
            TransferField(&r, g_save_state_bm_pad_addrs[i], 0x4, direction);
    }
    if (_levelID == TERRACE_VILLAGE_ID)
    {
        for (int i = 0; i < 10; i++)
            TransferField(&r, g_save_state_terrace_pad_addrs[i], 0x4, direction);
    }

    // Thief teleport state
    TransferField(&r, (void*)0x80075870, 0x4, direction);

    // memcpy((byte*)local_mem_region, 0x80075874, 0x4);
    // local_mem_region += 0x4;

    return r;
}


void FullSaveState(void)
{
    SetMemoryRegion();

    savestated_level_ids[savestate_selection] = _levelID;

    byte* r = SaveOrLoadState(SAVE);

    // Gnasty's World has by far the most moving collision, but as it currently stands
    // we see no reason to save it, as it doesn't move in any relevant way
    if (_levelID != GNASTYS_WORLD_ID)
    {
        SaveGeoData(r);
    }

    //hasSavedSpyro = true;
    //printf("%X\n\n\n", r);
}

void FullLoadState(void)
{
    SetMemoryRegion();

    if ((int*)mem_region != NULL)
    {
        if (savestated_level_ids[savestate_selection] == _levelID)
        {
            byte* r = SaveOrLoadState(LOAD);

            if (_levelID != GNASTYS_WORLD_ID)
            {
                LoadGeoData(r);
            }

            LoadstateNopFixes();
        }
    }
}

void DrawSavestateSwitchedText(void)
{
    #define percent_d_pointer ((char*)0x800755bc) // The game already has a %d pointer, and we're in deckard ram so, avoid new rodata strings!

    CapitalTextInfo text_info = { .x = 0x100, .y = 0xDC, .size = DEFAULT_SIZE }; // Normal location

    // Raise, and move to left if in dragon cutscene to avoid black bar, and rescued text
    if (_gameState == GAMESTATE_DRAGON_STATE)
    {
        text_info.y = 0xD0;
        text_info.x = 0x15;
    }

    char buf[3] = { 0 };
    sprintf(buf, percent_d_pointer, savestate_selection + 1);
    DrawTextCapitals(buf, &text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);
    RenderShadedMobyQueue();
}


// Fixes
int appliedNopFixTimer = 0;
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
