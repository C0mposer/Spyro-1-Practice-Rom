#include <common.h>
#include "levelselect.h"
#include <shared_funcs.h>
#include <bg_colors.h>

enum ModState
{
    GAME_STARTED,
    SKIPPED_INTRO,
    UNLOCKED_LEVELS
};
typedef enum ModState ModState;

ModState mod_state = GAME_STARTED;

RedGreen bg_colors[6] = {{0x70, 0}, {0xA0, 0xA0}, {0x00, 0x50}, {0x40, 0x18}, {0, 0x10}, {0x50, 0x50}};

//Externs
extern BackgroundColor bg_color_index;

extern bool should_update_bg_color;

extern signed int instaLoadLevelID;

extern int mainTimerAtReset;

extern bool should_reset_collectables;


//Shows all levels in inventory menu, and automatically unlocks homeworlds for balloonists
void UnlockAllLevels()
{
    for (int i = 0; i < 33; i++)
    {
        if(i < 6)
        {
            _balloonist_requirements_state[i] = 2;
        }
        if(i != 5)  //Don't Unlock Sunny Flight
        {
            _entered_level_flags[i] = 1;
        }
    }
}

void SkipIntro()
{
    _isPastTitleScreen = TRUE;                            //? This flag is checked by TheAdventureBeings() before it runs, so we must set this flag to TRUE.
    TheAdventureBegins();                                 //? Call The Adventure Begins cutscene sequence      
}


void SaveSpyroAndCamera()
{  
    //Copying The Spyro struct and most of the camera struct
    memcpy(_freeSpace, &_spyro, sizeof(_spyro));
    memcpy((byte*)_freeSpace + 0x370, &_cameraStart, 0xFF);

}

void ReloadSpyroAndCamera()
{
    //Reloading The Spyro struct and most of the camera struct
    memcpy(&_spyro, _freeSpace, sizeof(_spyro));
    memcpy(&_cameraStart, (byte*)_freeSpace + 0x370, 0xFF);

}

void RespawnSpyro()
{
    _spyro.position.z = 0;
    _shouldRespawnAtCheckpoint = FALSE;
}

void ResetLevelCollectables()
{
    if(should_reset_collectables)
    {
        memset(&_collectablesBitflags, 0, 0x4B0);
        for (int i = 0; i < 35; i++)
        {
            _levelGemsCollectedArray[i] = 0;
        }
    }
}

//Changing asm instructions for pause menu RGB. Cannot change B, as the value is in a shared register.
inline void SetTitleScreenColor(byte r, byte g)
{
    *(short*)(0x8001A674) = r;
    *(short*)(0x8001A67C) = g;
}


//*
//* ~ MAIN HOOK ~
//*
void MainFunc()
{   
    //! At seperate memory locations
    {
        InGameTimerHook();
    }

    LevelSelect();
    InstaLoad();

    _globalLives = 99;

    //Run once upon starting game
    if(mod_state == GAME_STARTED && _globalTimer > 20)                //? If the code hasn't ran once yet, and the global timer is greater than 20. Checking global timer because I have to wait a few frames to call The Adventure Begins
    {
        SkipIntro();
        _musicVolume = 0;
        mod_state = SKIPPED_INTRO;
    }

    //Run once upon skipping intro
    if(mod_state == SKIPPED_INTRO)
    {
        UnlockAllLevels();
        //memset((void*)0x8001b648, 0, 0x350); //Clear top of inventory menu   
        mod_state = UNLOCKED_LEVELS;
    }

    if(should_update_bg_color)
    {
        SetTitleScreenColor(bg_colors[bg_color_index].r, bg_colors[bg_color_index].g);
        should_update_bg_color = false;
    }
    
    //Main Loop
    if(mod_state == UNLOCKED_LEVELS && _gameState == GAMESTATE_GAMEPLAY)
    {
        //Save/Load spyro & camera information
        if(_currentButtonOneFrame == L3_BUTTON)
        {
            SaveSpyroAndCamera();
            instaLoadLevelID = -1;                                  //signals to the instaload function that the fly-in position and camera needs to be saved again
        }
        if(_currentButtonOneFrame == R3_BUTTON)
        {
            ReloadSpyroAndCamera();
        }

        //Respawn spyro & reset level gems
        if(_currentButton == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON)
        {
            RespawnSpyro();
            mainTimerAtReset = _globalTimer;
        }
        if((_currentButton == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON || _movementSubState == MOVEMENT_SUBSTATE_LOADING || _gameState == GAMESTATE_DEATH))
        {
            ResetLevelCollectables();
        }

        //Make Nestor Skippable
        if(_levelID == ARTISANS_ID)
        {
            _isNestorUnskipable = FALSE;
        }

        //Moonjump
        if(_currentButton == L1_BUTTON + L2_BUTTON + R1_BUTTON + R2_BUTTON + X_BUTTON)
        {
            _spyro.position.z += 500;
        }

    }
}
