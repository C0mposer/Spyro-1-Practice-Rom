#include <common.h>
#include <main_updates.h>
#include <IGT.h>
#include <levelselect.h>
#include <bg_colors.h>
#include <rat_code.h>
#include <credits_skip.h>
#include <loot_plane.h>
#include <multitap.h>



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
        InGameTimerUpdate();
        CustomMenuUpdate();
        RatCodeUpdate();
        CreditsSkipUpdate();
        LootPlaneUpdate();
        MultiTapUpdate();
    }

    MainUpdate();
}
