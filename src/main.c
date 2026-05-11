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
#include <spin_wheel.h>
#include <deckard_usb_basic_test.h>
#include <ghosts_autoplay.h>
#include <ghost_replay_api.h>
#include <mds_visualizer.h>
#include <collision_wireframe.h>
#if BUILD == REDUX || BUILD == DUCKSTATION
#include <ghosts_autoplay_test.h>
#endif

bool hasLoadedCDCode;
#define MOD_EXPORT(label, mode, options)

//*
//* ~ MAIN EVERY FRAME HOOK ~
//*
MOD_EXPORT("TESTTsT", "slider", "min=0,max=100,step=1");
int teeeesst = 10;
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
        LoadCdData(DECKARD_PS2MSTR_SECTOR, (unsigned int*)DECKARD_STRINGS_REGION, 0x1000, 0, 600); // PS2MSTR.BIN (.data words flipped in build)
        LoadCdData(DECKARD_BE_SECTOR, (int*)EXTRA_DECKARD_CODE_REGION, 0xD000, 0, 600);     // BE.BIN (byteswapped in release script)
        #elif BUILD == PS2_IOP
        LoadCdData(265759, (int*)0x80008EB0, 0x1600, 0, 600);     //Loads kern.bin
        LoadCdData(265762, (int*)0x80007530, 0x1000, 0, 600);     //Loads kern2.bin
        #elif BUILD == DUCKSTATION
        LoadCdData(265759, (int*)0x8000C000, 0x1600, 0, 600);     //Loads kern.bin
        LoadCdData(265762, (int*)0x8000EA00, 0x1000, 0, 600);     //Loads kern2.bin
        LoadCdData(DUCKSTATION_EXTRA_SECTOR, (int*)DUCKSTATION_EXTRA_CODE_REGION, 0x20000, 0, 600); //Loads EXTRA.bin
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
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            //SpinWheelUpdate();
            GhostsAutoplayUpdate();
            #endif
            LevelSelectUpdate();
            InstaLoadUpdate();
            TimerUpdate();
            ILUpdate();
            TrackConsistencyUpdate();
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            GhostButtonCheck();
            GhostFrameTick();
            #endif
            SaveStateUpdate();
            CustomMenuUpdate();
            CustomMenuUpdate2();
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            MemoryWatchUpdate();
            TheatreModeUpdate();
            CustomMenuUpdateGhosts();
            CustomMenuUpdateSkinEditor();
            //CollisionWireframeUpdate();
            #endif
            RatCodeUpdate();
            CreditsSkipUpdate();
            LootPlaneUpdate();
            MultiTapUpdate();
            //CheckMiscTimerUpdate();
            CosmeticsUpdate();
            MainUpdate();
            SwapControllerUpdate();

            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            LoadstateFixesUpdate();
            #endif

            //! WIP
            //ShempSkipFrameDataUpdate(); // Work on later
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            TestUpdateSpyroColor();
            #endif
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            //GhostDebugDraw();
            #endif
        }
    }
}
