#include <common.h>
#include <main_updates.h>
#include <IGT.h>
#include <levelselect.h>
#include <custom_menu.h>
#include <cosmetic.h>
#include <rat_code.h>
#include <credits_skip.h>
#include <loot_plane.h>
#include <multitap.h>
#include <moving_geo.h>
#include <cd.h>

bool hasLoadedCDCode;

//*
//* ~ MAIN EVERY FRAME HOOK ~
//*
void MainHook()
{
    if (!hasLoadedCDCode)
    {
        _musicState = 0x40;

        #if BUILD == PS1                   
        LoadCdData(265759, (int*)0x8000C000, 0x1600, 0, 600);     //Loads kern.bin
        LoadCdData(265762, (int*)0x8000EA00, 0x1000, 0, 600);     //Loads kern2.bin
        #elif BUILD == PS2_DECKARD
        //patch_lut();
        LoadCdData(265759, (int*)0x80008EB0, 0x1600, 0, 600);     //Loads kern.bin
        LoadCdData(265762, (int*)0x80007530, 0x1000, 0, 600);     //Loads kern2.bin
        LoadCdData(266136, EXTRA_DECKARD_CODE_REGION, 0x6000, 0, 600);     //Loads PS2M.bin in BE
        #elif BUILD == PS2_IOP
        LoadCdData(265759, (int*)0x80008EB0, 0x1600, 0, 600);     //Loads kern.bin
        LoadCdData(265762, (int*)0x80007530, 0x1000, 0, 600);     //Loads kern2.bin
        #elif BUILD == 5
        LoadCdData(265759, (int*)0x8000C000, 0x1600, 0, 600);     //Loads kern.bin
        LoadCdData(265762, (int*)0x8000EA00, 0x1000, 0, 600);     //Loads kern2.bin
        LoadCdData(266136, 0x80300000, 0x4000, 0, 600);           //Loads EXTRA.bin
        LoadCdData(266141, 0x80300000, 0x1000, 0, 600);           //Loads EXTRA.bin
        #endif

        //PrepareMemcard();

        //TestMemcard();

        hasLoadedCDCode = true;                                 //Set Flag
    }
    else
    {
        //! Other functions to run every frame
        {
            //FontTesting();
            LevelSelectUpdate();
            InstaLoadUpdate();
            TimerUpdate();
            ILUpdate();
            TrackConsistencyUpdate();
            #if BUILD == PS2_DECKARD || BUILD == REDUX
            GhostButtonCheck();
            #endif
            SaveStateUpdate();
            CustomMenuUpdate();
            CustomMenuUpdate2();
            #if BUILD == PS2_DECKARD || BUILD == REDUX
            CustomMenuUpdateGhosts();
            #endif
            RatCodeUpdate();
            CreditsSkipUpdate();
            LootPlaneUpdate();
            MultiTapUpdate();
            CheckMiscTimerUpdate();
            CosmeticsUpdate();
            MainUpdate();
            SwapControllerUpdate();

            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == 5
            LoadstateFixesUpdate();
            #endif

            //! WIP
            //ShempSkipFrameDataUpdate(); // Work on later
            //MemoryWatchTest();
            #if BUILD == PS2_DECKARD || BUILD == REDUX
            //TestUpdateSpyroColor();
            //CustomMenuUpdateSkinEditor();
            #endif
        }
    }
}
