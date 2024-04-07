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



//*
//* ~ MAIN EVERY FRAME HOOK ~
//*
void MainFunc()
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
        CdLoad();               //Becomes CustomMenuUpdate once code is loaded
        // CustomMenuUpdate();
        // CustomMenuUpdate2();
        // RatCodeUpdate();
        // CreditsSkipUpdate();
        // LootPlaneUpdate();
        // MultiTapUpdate();
        //GeoDataUpdate();
    }

    // MainUpdate();
}
