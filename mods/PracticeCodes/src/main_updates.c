#include <common.h>
#include <levelselect.h>
#include <shared_funcs.h>
#include <cosmetic.h>
#include <multitap.h>
#include <loot_plane.h>
#include <igt.h>
#include <right_stick.h>
#include <sound.h>
#include <custom_text.h>
#include <cosmetic.h>
#include "main_updates.h"

enum ModState
{
    GAME_STARTED,
    SKIPPED_INTRO,
    UNLOCKED_LEVELS
};
typedef enum ModState ModState;

ModState mod_state = GAME_STARTED;

bool hasSavedSpyro = false;

bool local_previous_level_id = 0;

bool default_mod_settings = false;

bool shouldUnNopRotationMoonJump = false;

int moonjumpShouldSlowdown = 0;

// Externed from elsewhere
extern int mainTimerAtReset;


extern bool should_write_spyro_bmp;
extern bool should_write_flame_bmp;
extern bool should_write_sparx_bmp;

extern bool disable_portal_entry;
extern bool has_savestated_on_disabling_portal;


bool is_greenscreen = false;

//! Main Basic Checks
void MainUpdate()
{
    //Run once upon starting game
    if (mod_state == GAME_STARTED && _globalTimer > 20)                // If the code hasn't ran once yet, and the global timer is greater than 20. Checking global timer because I have to wait a few frames to call The Adventure Begins
    {
        SkipIntro();
        mod_state = SKIPPED_INTRO;
        _musicVolume = 0;
    }

    // Run Once
    if (mod_state == SKIPPED_INTRO)
    {
        UnlockAllLevels();
        mod_state = UNLOCKED_LEVELS;
    }

    // Draw Version Number on The Adventure Continues
    if (_gameState == GAMESTATE_TITLE_SCREEN)
    {
        _globalLives = 99;

        // Mod Version Number
        CapitalTextInfo version_text_info = { 0 };
        version_text_info.x = SCREEN_RIGHT_EDGE - 70;
        version_text_info.y = SCREEN_BOTTOM_EDGE - 10;
        version_text_info.size = DEFAULT_SIZE;
        DrawTextCapitals(MOD_VERSION_STRING, &version_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);

        // Default Settings
        UpdateDefaultSettings();

        CapitalTextInfo default_settings_text_info = { 0 };
        default_settings_text_info.x = SCREEN_LEFT_EDGE + 15;
        default_settings_text_info.y = SCREEN_BOTTOM_EDGE - 10;
        default_settings_text_info.size = DEFAULT_SIZE;

        if (default_mod_settings == false)
            DrawTextCapitals("DEFAULTS OFF", &default_settings_text_info, DEFAULT_SPACING, MOBY_COLOR_BLUE);
        if (default_mod_settings == true)
            DrawTextCapitals("DEFAULTS ON", &default_settings_text_info, DEFAULT_SPACING, MOBY_COLOR_BLUE);

        // Render Text
        RenderShadedMobyQueue();
    }

    //Main Loop
    if (mod_state == UNLOCKED_LEVELS && _gameState == GAMESTATE_GAMEPLAY)
    {
         //Respawn spyro & reset level gems
        if (_currentButton == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON)
        {
            RespawnSpyro();
            ResetLevelCollectables();
        }

        //Make Nestor Skippable
        if (_levelID == ARTISANS_ID)
        {
            _nestorMakeUnskipable = FALSE;
        }

        //Lives always 99
        if (_globalLives != 99)
        {
            _globalLives = 99;
            _globalLivesCounter = 99;
        }

        //Give Key
        if (CheckButtonMultiTap(TRIANGLE_BUTTON, 3))
        {
            _keyState = 1;
        }

        MoonjumpUpdate();
    }
}

void MoonjumpUpdate()
{
    if (_currentButton == L1_BUTTON + L2_BUTTON + R1_BUTTON + R2_BUTTON + X_BUTTON)
    {
        _spyro.position.z += 500;

        //NOP SetCameraTurnDirection
        *(int*)0x80035F58 = 0x03E00008;
        *(int*)0x80035F5C = 0x00000000;

        _cameraTurnDirection = 0;

        shouldUnNopRotationMoonJump = true;
    }
    else if (shouldUnNopRotationMoonJump)
    {
        // Bring back SetCameraTurnDirection
        *(int*)0x80035F58 = 0x3C028008;
        *(int*)0x80035F5C = 0x8C428C4C;

        shouldUnNopRotationMoonJump = false;

        moonjumpShouldSlowdown = true;
    }

    if (moonjumpShouldSlowdown)
    {
        if (_spyro.state == FLOP)
        {
            _spyro.position.z += 285;
        }
        else
        {
            moonjumpShouldSlowdown = false;
        }
    }
}


void ChangeIntroTextColor(int color)
{
    *(short*)0x8001e784 = color;  // Value part of li opcode
    *(short*)0x8001e786 = 0x2402; // Regular part of li opcode
}

//Shows all levels in inventory menu, and automatically unlocks homeworlds for balloonists
void UnlockAllLevels()
{
    for (int i = 0; i < 35; i++)
    {
        if (i < 6)
        {
            _balloonist_requirements_state[i] = 2;
        }
        if (i != 5)  //Don't Unlock Sunny Flight
        {
            _entered_level_flags[i] = 1;
        }
    }
}

void ResetCutscene()
{
    _whichCutsceneToPlay = 1;
}

//This runs The Adventure Begins cutscene
void SkipIntro()
{
    _isPastTitleScreen = TRUE;                              //? This flag is checked by TheAdventureBeings() before it runs, so we must set this flag to TRUE.
    #if BUILD != 0 //Don't call adventure begins on redux
    ChangeIntroTextColor(MOBY_COLOR_PURPLE);
    TheAdventureBegins();                                 //Call The Adventure Begins cutscene sequence      
    #endif
}

void SaveSpyroAndCamera(bool flyInFlag)
{
    byte* free_space = NULL;

    if (!flyInFlag)
    {
        free_space = (byte*)0x80073990;     //Free space 1 for L3
        hasSavedSpyro = true;               //To load only after L3 has been pressed
    }
    else
    {
        free_space = (byte*)0x80073d20;     //Free space 2 for fly in snap-shot for instaload
    }

    //Copying The Spyro struct and most of the camera struct
    memcpy(free_space, &_spyro, sizeof(_spyro));
    memcpy((byte*)free_space + 0x270, &_cameraStart, 0xFF);
    memcpy((byte*)free_space + 0x380, &_keyState, 0x4);

}

void ReloadSpyroAndCamera(bool flyInFlag)
{
    byte* free_space = NULL;

    if (!flyInFlag)
    {
        free_space = (byte*)_freeSpace;
    }
    else
    {
        free_space = (byte*)0x80073d20;
    }

    //Reloading The Spyro struct and most of the camera struct
    memcpy(&_spyro, free_space, sizeof(_spyro));
    memcpy(&_cameraStart, (byte*)free_space + 0x270, 0xFF);
    memcpy(&_keyState, (byte*)free_space + 0x380, 0x4);

}

//This function puts spyro at 0 which will always immediately kill him, since its a void spot.
void RespawnSpyro()
{
    _gameState = GAMESTATE_DEATH;
    _effect_ScreenFadeOut = 0x0;
    _globalLives = 99;
    _shouldRespawnAtCheckpoint = FALSE;


}

//This function clears the bitflags for which collectables should spawn into a level/homeworld with spyro. This is the same area that the memory card writes to when loading a game.
void ResetLevelCollectables()
{
    memset(&_collectablesBitflags, 0, 0x4B0);
    for (int i = 0; i < 35; i++)
    {
        _levelGemsCollectedArray[i] = 0;
    }

    memset(&_flightLevelTimes, 0, sizeof(int) * 5);

    // Key Glitch Fix
    _keyState = 0;
    _ptr_keyChestHostGem = NULL;
}

// void EnableInventoryMenu()
// {
//     // UN-NOP Call to PrepareInventoryGamestate
//     *(int*)(0x80033C28) = 0x0C00B1C5;
//     *(int*)(0x80033C2C) = 0x24040001;
// }

// void DisableInventoryMenu()
// {
//     // NOP Call to PrepareInventoryGamestate
//     *(int*)(0x80033C28) = 0x0;
//     *(int*)(0x80033C2C) = 0x0;

// }

void ChangeInventoryMenu(SwitchButton state)
{
    if (state == OFF)
    {
        // NOP Call to PrepareInventoryGamestate
        *(int*)(0x80033C28) = 0x0;
        *(int*)(0x80033C2C) = 0x0;
    }
    else if (state == ON)
    {
        // UN-NOP Call to PrepareInventoryGamestate
        *(int*)(0x80033C28) = 0x0C00B1C5;
        *(int*)(0x80033C2C) = 0x24040001;
    }
}

void UpdateDefaultSettings()
{
    if (_currentButtonOneFrame == RIGHT_BUTTON)
    {
        default_mod_settings = true;
        //PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
    }
    if (_currentButtonOneFrame == LEFT_BUTTON)
    {
        default_mod_settings = false;
        //PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
    }

    if (default_mod_settings == true)
    {
        TurnOnDefaultSettings();
    }
    if (default_mod_settings == false)
    {
        TurnOffDefaultSettings();
    }

}