#include <common.h>
#include <levelselect.h>
#include <shared_funcs.h>
#include <bg_colors.h>
#include <rat_code.h>
#include <multitap.h>

enum ModState
{
    GAME_STARTED,
    SKIPPED_INTRO,
    UNLOCKED_LEVELS
};
typedef enum ModState ModState;

ModState mod_state = GAME_STARTED;

bool hasSavedSpyro = false;
bool hasResetSavestate = false;

const RedGreen bg_colors[6] = {{0x70, 0}, {0xA0, 0xA0}, {0x00, 0x50}, {0x40, 0x18}, {0, 0x10}, {0x50, 0x50}};

// Externed from elsewhere
extern BackgroundColor bg_color_index;
extern bool should_update_bg_color;
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

//This runs The Adventure Begins cutscene
void SkipIntro()
{
    _isPastTitleScreen = TRUE;                            //? This flag is checked by TheAdventureBeings() before it runs, so we must set this flag to TRUE.
    TheAdventureBegins();                                 //Call The Adventure Begins cutscene sequence      
}

void SaveSpyroAndCamera(bool flyInFlag)
{  
    byte* free_space = NULL;

    if(!flyInFlag)
    {
        free_space = (byte*)0x80073990;     //Free space 1 for L3
        hasSavedSpyro = true;               //To load only after L3 has been pressed
    }
    else
    {
        free_space = (byte*)0x80073e10;     //Free space 2 for fly in snap-shot for instaload
    }

    //Copying The Spyro struct and most of the camera struct
    memcpy(free_space, &_spyro, sizeof(_spyro));
    memcpy((byte*)free_space + 0x270, &_cameraStart, 0xFF);

}

void ReloadSpyroAndCamera(bool flyInFlag)
{
    byte* free_space = NULL;

    if(!flyInFlag)
    {
        free_space = (byte*)_freeSpace;
    }
    else
    {
        free_space = (byte*)0x80073e10;
    }

    //Reloading The Spyro struct and most of the camera struct
    memcpy(&_spyro, free_space, sizeof(_spyro));
    memcpy(&_cameraStart, (byte*)free_space + 0x270, 0xFF);
}

//This function puts spyro at 0 which will always immediately kill him, since its a void spot.
void RespawnSpyro()
{
    _spyro.position.z = 0;
    _shouldRespawnAtCheckpoint = FALSE;
}

//This function clears the bitflags for which collectables should spawn into a level/homeworld with spyro. This is the same area that the memory card writes to when loading a game.
void ResetLevelCollectables()
{
    if(should_reset_collectables)
    {
        memset(&_collectablesBitflags, 0, 0x4B0);
        for (int i = 0; i < 35; i++)
        {
            _levelGemsCollectedArray[i] = 0;
        }

        // Key Glitch Fix
        _keyState = 0;
        _ptr_keyChestHostGem = NULL;
    }
}

//Changing asm instructions for pause menu RGB. Cannot change B value, as the value is in a shared register with other crucial parts of the struct.
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
    //! Other functions to run every frame
    LevelSelect();
    InstaLoad();

    //! At seperate memory locations (See buildlist.txt)
    {
        InGameTimerHook();
        RatCodeUpdate();
        MultiTapUpdate();
    }

    //Run once upon starting game
    if(mod_state == GAME_STARTED && _globalTimer > 20)                // If the code hasn't ran once yet, and the global timer is greater than 20. Checking global timer because I have to wait a few frames to call The Adventure Begins
    {
        SkipIntro();
        mod_state = SKIPPED_INTRO;
        _musicVolume = 0;
    }

    //Run once upon skipping intro
    if(mod_state == SKIPPED_INTRO)
    {
        UnlockAllLevels();
        mod_state = UNLOCKED_LEVELS;
        _globalLives = 99;
        //memset((void*)0x8001b648, 0, 0x350); //Clear top of inventory menu   
    }

    //Change background color when menu gets updated
    if(should_update_bg_color)
    {
        SetTitleScreenColor(bg_colors[bg_color_index].r, bg_colors[bg_color_index].g);
        should_update_bg_color = false;
    }
    
    //Main Loop
    if(mod_state == UNLOCKED_LEVELS && _gameState == GAMESTATE_GAMEPLAY)
    {
        //Save spyro & camera information
        if(_currentButtonOneFrame == L3_BUTTON)
        {
            SaveSpyroAndCamera(false);
        }

        //Load spyro & camera information
        if(_currentButtonOneFrame == R3_BUTTON && hasSavedSpyro == true)
        {
            ReloadSpyroAndCamera(false);
        }

        //Respawn spyro & reset level gems
        if(_currentButton == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON)
        {
            RespawnSpyro();
            ResetLevelCollectables();
            mainTimerAtReset = _globalTimer;  //Resets timer to 0 by syncing up to the global timer
            _globalLives = 100;
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

    //Safeguard against loading with another levels savestate/no savestate
    {     
        if(!hasResetSavestate && _levelLoadState == 5)
        {
            hasSavedSpyro = false;
            hasResetSavestate = true;
        }   
        else if(_levelLoadState == -1)
        {
            hasResetSavestate = false;
        }
    }
}
