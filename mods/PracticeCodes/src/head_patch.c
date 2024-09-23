#include <common.h>

bool headBools[3] = { FALSE, FALSE, FALSE };

enum {
    INITIAL = 0,
    COMPLETE = 1,
    UPDATE_TWILIGHT = 2,
    UPDATE_GNASTY = 3,
    UPDATE_LOOT = 4
};


void HeadControl() {
    // register void* s3 asm("$s1");
    // int* ptr_mobyData = (int *)s3;

    if (*_ptr_headControlMoby != COMPLETE) {          //Check if the heads need to be updated either because it's the first time or because of a manual change
        for (int i = 0; i < 3; i++) {                //vv Checks if it's the inital head update or if you're flying out of a portal
            if ((*_ptr_headControlMoby == INITIAL && headBools[i]) || (_spyro.state == 0xf && (TerrainSkipToKeyFrame(i, 0x3b, 0), _spyro.subState == MOVEMENT_SUBSTATE_EXIT_PORTAL))) {
                UpdateTerrain(i, 0xfc, 0);                                  //Initiates the head opening animation
            }
            if ((*_ptr_headControlMoby == (i + 2))) {                         //Checks if a head needs to be toggled by a manual input
                UpdateTerrain(i, 0xfc, 0);                                  //Initiates the head opening/closing animation
                if ((TerrainCurrentKeyFrame(i) >> 8 & 0xffU) == 0) {
                    headBools[i] = TRUE;                                    //Checks if the head was already closed and sets the state of the head to be true (open)
                }
                else if ((TerrainCurrentKeyFrame(i) >> 8 & 0xffU) == 0x3c) {
                    headBools[i] = FALSE;                                   //Checks if the head was already open and sets the state of the head to be false (closed)
                }
            }
        }
        *_ptr_headControlMoby = 1;
    }






    if (CheckButtonMultiTap(L2_BUTTON, 3)) {
        *_ptr_headControlMoby = 2;                                          //Sets the moby special data to toggle the Twilight head
    }

    if (CheckButtonMultiTap(R2_BUTTON, 3)) {
        *_ptr_headControlMoby = 3;                                          //Sets the moby special data to toggle the Gnasty head
    }

    if (CheckButtonMultiTap(R1_BUTTON, 3)) {
        *_ptr_headControlMoby = 4;                                          //Sets the moby special data to toggle the Loot head
    }
}


// Original Code //

// void headControl(){
//     register void* s3 asm("$s1");

//     int* ptr_mobyData = (int *)s3;

//     if(*ptr_mobyData == 0){
//         if(_entered_level_flags[INDEX_TWILIGHT_HARBOR] != 0 && (UpdateTerrain(0, 0xfC, 0), _spyro.state == 0xf) && _spyro.subState == MOVEMENT_SUBSTATE_EXIT_PORTAL){
//             TerrainSkipToKeyFrame(0, 0x3b, 0);
//         }
//         if(_entered_level_flags[INDEX_GNASTY_GNORC] != 0 && (UpdateTerrain(1, 0xfC, 0), _spyro.state == 0xf) &&  _spyro.subState == MOVEMENT_SUBSTATE_EXIT_PORTAL){
//             TerrainSkipToKeyFrame(1, 0x3b, 0);
//         }
//         if(_entered_level_flags[INDEX_GNASTYS_LOOT] != 0 && (UpdateTerrain(2, 0xfC, 0), _spyro.state == 0xf) && _spyro.subState == MOVEMENT_SUBSTATE_EXIT_PORTAL){
//             TerrainSkipToKeyFrame(2, 0x3b, 0);
//         }
//         *ptr_mobyData = 1;
//     }

//     if ((TerrainCurrentKeyFrame(0) >> 8 & 0xffU) == 0 && _vortexFlags[INDEX_GNORC_COVE] != 0 && _levelDragonCounts[INDEX_GNASTYS_WORLD] > 0){
//         UpdateTerrain(0, 0xfc, 0);
//     }
//     if ((TerrainCurrentKeyFrame(1) >> 8 & 0xffU) == 0 && _vortexFlags[INDEX_TWILIGHT_HARBOR] != 0){
//         UpdateTerrain(1, 0xfc, 0);
//     }
//     if ((TerrainCurrentKeyFrame(2) >> 8 & 0xffU) == 0 && _globalGems >= 12000 && _globalDragons >= 80 && _globalEggs >= 12){
//         UpdateTerrain(2, 0xfc, 0);
//     }
// }