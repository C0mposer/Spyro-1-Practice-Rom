#include <common.h>
#include <main_updates.h>
#include <IGT.h>
#include <levelselect.h>
#include <bg_colors.h>
#include <rat_code.h>
#include <credits_skip.h>
#include <loot_plane.h>
#include <multitap.h>
#include <moving_geo.h>
#include <cd.h>

int ranOnce;

//*
//* ~ MAIN EVERY FRAME HOOK ~
//*
void MainFunc()
{   
    if(!ranOnce){
        _musicState = 0x40;                                 //Turns off Music
        LoadCdData(265759, 0x8000C000, 0x1000, 0, 600);     //Loads menu.bin
        LoadCdData(265761, 0x8000D000, 0x1000, 0, 600);     //Loads menu2.bin
        ranOnce = 1;                                        //Set Flag
    }
    else{
        //! Other functions to run every frame
        {
        LevelSelectUpdate();
        InstaLoadUpdate();
        }
        //! Other functions to run every frame at seperate memory locations (See buildlist.txt)
        {
            TimerUpdate();
            ILUpdate();
            //CdLoad();               //Becomes CustomMenuUpdate once code is loaded
            CustomMenuUpdate();
            CustomMenuUpdate2();
            RatCodeUpdate();
            CreditsSkipUpdate();
            LootPlaneUpdate();
            MultiTapUpdate();
            //GeoDataUpdate();
        }

        MainUpdate();
    }
}
