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

int savestate_free_space[12] = { 0 };

bool hasLoadedCDCode;

int loaded_timer = 0;

//*
//* ~ MAIN EVERY FRAME HOOK ~
//*
void MainFunc()
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
        #endif

        hasLoadedCDCode = true;                                 //Set Flag
    }
    else
    {
        //! Other functions to run every frame
        {
            LevelSelectUpdate();
            InstaLoadUpdate();
        }
        //! Other functions to run every frame at seperate memory locations (See buildlist.txt)
        {
            TimerUpdate();
            ILUpdate();
            CustomMenuUpdate();
            CustomMenuUpdate2();
            SaveStateUpdate();
            RatCodeUpdate();
            CreditsSkipUpdate();
            LootPlaneUpdate();
            MultiTapUpdate();
            CheckLandingUpdate();
            TrackConsistencyUpdate();
            CosmeticsUpdate();
            //CalculatePlaytimeUpdate();
            //ShempSkipFrameDataUpdate(); // Work on later

            //MemoryWatchTest();

            #if BUILD == PS2_DECKARD || BUILD == REDUX
            LoadstateFixesUpdate();
            #endif
        }

        MainUpdate();
    }
}
