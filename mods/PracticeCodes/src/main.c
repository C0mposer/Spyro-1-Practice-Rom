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
        LoadCdData(265759, (int*)0x80008EB0, 0x1600, 0, 600);     //Loads kern.bin
        LoadCdData(265762, (int*)0x80007530, 0x1000, 0, 600);     //Loads kern2.bin
        LoadCdData(265764, EXTRA_DECKARD_CODE_REGION, 0x1000, 0, 600);     //Loads PS2M.bin
        #elif BUILD == PS2_IOP
        LoadCdData(265759, (int*)0x80008EB0, 0x1600, 0, 600);     //Loads kern.bin
        LoadCdData(265762, (int*)0x80007530, 0x1000, 0, 600);     //Loads kern2.bin
        #elif BUILD == 5
        LoadCdData(265759, (int*)0x8000C000, 0x1600, 0, 600);     //Loads kern.bin
        LoadCdData(265762, (int*)0x8000EA00, 0x1000, 0, 600);     //Loads kern2.bin
        LoadCdData(265764, 0x80300000, 0x1000, 0, 600);           //Loads EXTRA.bin
        #endif

        hasLoadedCDCode = true;                                 //Set Flag
    }
    else
    {
        //! Other functions to run every frame
        {
            LevelSelectUpdate();
            InstaLoadUpdate();
            TimerUpdate();
            ILUpdate();
            TrackConsistencyUpdate();
            GhostButtonCheck();
            SaveStateUpdate();
            CustomMenuUpdate();
            CustomMenuUpdate2();
            RatCodeUpdate();
            CreditsSkipUpdate();
            LootPlaneUpdate();
            MultiTapUpdate();
            CheckMiscTimerUpdate();
            CosmeticsUpdate();
            MainUpdate();
            SwapControllerUpdate();
            //GhostTest();

            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == 5
            LoadstateFixesUpdate();
            #endif

            //! WIP
            //ShempSkipFrameDataUpdate(); // Work on later
            //MemoryWatchTest();
        }
    }
}
