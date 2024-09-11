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
bool readyToLoadstateAfterDeath = false;
bool switch_button_held;

int savestateSwitchedTimer = 0;

int savestated_level_ids[3] = {0}; // For keeping savestates upon loop

bool should_savestate_after_dragon_or_load = false;

bool should_loadstate_gems = false;

// Externed from elsewhere

extern int mainTimerAtReset;
extern bool should_loadstate_gems;
// from IGT.c
extern int savestate_selection;

extern int savestate_button_index;
extern int loadstate_button_index;
extern int switch_state_button_index;

extern const short SAVESTATE_BUTTONS[2];
extern const short LOADSTATE_BUTTONS[3];

extern int il_timer_offset[3];

extern bool should_write_spyro_bmp;
extern bool should_write_flame_bmp;
extern bool should_write_sparx_bmp;

bool is_greenscreen = false;


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
    _isPastTitleScreen = TRUE;                              //? This flag is checked by TheAdventureBeings() before it runs, so we must set this flag to TRUE.
    #if BUILD != 0 //Don't call adventure begins on redux
        ChangeIntroTextColor(MOBY_COLOR_PURPLE);
        TheAdventureBegins();                                 //Call The Adventure Begins cutscene sequence      
    #endif
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
        free_space = (byte*)0x80073d20;     //Free space 2 for fly in snap-shot for instaload
    }

    //Copying The Spyro struct and most of the camera struct
    memcpy(free_space, &_spyro, sizeof(_spyro));
    memcpy((byte*)free_space + 0x270, &_cameraStart, 0xFF);
    memcpy((byte *)free_space + 0x380, &_keyState, 0x4);

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
        free_space = (byte*)0x80073d20;
    }

    //Reloading The Spyro struct and most of the camera struct
    memcpy(&_spyro, free_space, sizeof(_spyro));
    memcpy(&_cameraStart, (byte*)free_space + 0x270, 0xFF);
    memcpy(&_keyState, (byte *)free_space + 0x380, 0x4);

}

//This function puts spyro at 0 which will always immediately kill him, since its a void spot.
void RespawnSpyro()
{
    _gameState = GAMESTATE_DEATH;
    _effect_ScreenFadeOut = 0x0;
    _globalLives = 100;
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

    // Key Glitch Fix
    _keyState = 0;
    _ptr_keyChestHostGem = NULL;
}

void DrawSavestateSwitchedText(void)
{
    char buf[3] = {0};
    sprintf(buf, "%d", savestate_selection + 1);
    DrawTextCapitals(buf, &(CapitalTextInfo){.x=0x100, .y=0xDC, .size=DEFAULT_SIZE}, DEFAULT_SPACING, MOBY_COLOR_PURPLE);
    RenderShadedMobyQueue();
}

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

//! Main Basic Checks
void MainUpdate()
{
    
    //Run once upon starting game
    if(mod_state == GAME_STARTED && _globalTimer > 20)                // If the code hasn't ran once yet, and the global timer is greater than 20. Checking global timer because I have to wait a few frames to call The Adventure Begins
    {
        SkipIntro();
        mod_state = SKIPPED_INTRO;
        _musicVolume = 0;
    }

    // Run Once
    if(mod_state == SKIPPED_INTRO)
    {
        UnlockAllLevels();
        mod_state = UNLOCKED_LEVELS;
    }

    // Draw Version Number on The Adventure Continues
    if(_gameState == GAMESTATE_TITLE_SCREEN)
    {
        _globalLives = 99;
        
        CapitalTextInfo version_text_info = {0};
        version_text_info.x = SCREEN_RIGHT_EDGE - 70;
        version_text_info.y = SCREEN_BOTTOM_EDGE - 10;
        version_text_info.size = DEFAULT_SIZE;
        
        DrawTextCapitals(MOD_VERSION_STRING, &version_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);
        RenderShadedMobyQueue();
    }
    
    //Main Loop
    if(mod_state == UNLOCKED_LEVELS && _gameState == GAMESTATE_GAMEPLAY)
    {
        //Save spyro & camera information
        if(savestate_button_index < 2) //
        {
            if(_currentButtonOneFrame == SAVESTATE_BUTTONS[savestate_button_index] || should_savestate_after_dragon_or_load)
            {
                #if BUILD == 2 || BUILD == 0
                    SaveStateTest();
                #elif BUILD == 1 || BUILD == 3
                    SaveSpyroAndCamera(false);
                #endif

                should_savestate_after_dragon_or_load = false;
                
            }
        }
        if(savestate_button_index == 2) // for multi tap check
        {
            if(CheckButtonMultiTap(L3_BUTTON, 2))
            {
                #if BUILD == 2 || BUILD == 0
                SaveStateTest();
                #elif BUILD == 1 || BUILD == 3
                SaveSpyroAndCamera(false);
                #endif
            }
        }

        //Load spyro & camera information
        if((_currentButtonOneFrame == LOADSTATE_BUTTONS[loadstate_button_index] && hasSavedSpyro == true) || (readyToLoadstateAfterDeath == true && _effect_ScreenFadeIn != 0))
        {
            if(!should_loadstate_gems || (_effect_ScreenFadeIn = 0, readyToLoadstateAfterDeath))
            {
                #if BUILD == 2 || BUILD == 0
                    LoadStateTest();
                #elif BUILD == 1 || BUILD == 3
                    ReloadSpyroAndCamera(false);
                #endif
                
                readyToLoadstateAfterDeath = false;

                if (_levelID == TREE_TOPS_ID)
                {
                    _treeTopsThiefTimer = 0;
                    _isInInGameCutscene = false;
                    _effect_ScreenLetterBox = 0;
                }

                if(_levelID == GNASTYS_LOOT_ID)
                {
                    LootGiveAllKeys();
                }
            }
            else
            {
                RespawnSpyro();
                ResetLevelCollectables();
                readyToLoadstateAfterDeath = true;
            }
        }

        //Respawn spyro & reset level gems
        if(_currentButton == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON)
        {
            RespawnSpyro();
            ResetLevelCollectables();
        }

        //Make Nestor Skippable
        if(_levelID == ARTISANS_ID)
        {
            _nestorMakeUnskipable = FALSE;
        }

        //Moonjump
        if(_currentButton == L1_BUTTON + L2_BUTTON + R1_BUTTON + R2_BUTTON + X_BUTTON)
        {
            _spyro.position.z += 500;
        }

        //Give Key
        if(CheckButtonMultiTap(TRIANGLE_BUTTON, 3))
        {
            _keyState = 1;
        }

        #if BUILD == 2 || BUILD == 0  // ONLY DO SAVESTATE SLOTS ON DECKARD
            // Quick Savestate Slot Selection
            if(switch_state_button_index == 0)
            {    
                int direction = GetHorizontalRightStickDirection();

                if (direction == LEFT && savestate_selection > 0)
                {
                    savestate_selection--;
                    PlaySoundEffect(SOUND_EFFECT_PAUSE_MENU_CHANGE_SELECTION_DING, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
                    savestateSwitchedTimer = 1;
                }
                else if (direction == RIGHT && savestate_selection < 2)
                {
                    savestate_selection++;
                    PlaySoundEffect(SOUND_EFFECT_PAUSE_MENU_CHANGE_SELECTION_DING, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
                    savestateSwitchedTimer = 1;
                }

            }
            else if (switch_state_button_index == 1)
            {        
                if (_currentButton == L1_BUTTON + R1_BUTTON + LEFT_BUTTON && savestate_selection > 0 && !switch_button_held)
                {
                    savestate_selection--;
                    PlaySoundEffect(SOUND_EFFECT_PAUSE_MENU_CHANGE_SELECTION_DING, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
                    savestateSwitchedTimer = 1;

                    switch_button_held = true;
                }
                if (_currentButton == L1_BUTTON + R1_BUTTON + RIGHT_BUTTON && savestate_selection < 2 && !switch_button_held)
                {
                    savestate_selection++;
                    PlaySoundEffect(SOUND_EFFECT_PAUSE_MENU_CHANGE_SELECTION_DING, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
                    savestateSwitchedTimer = 1;

                    switch_button_held = true;
                }
            }
            // Draw Switched Savestate Text
            if (savestateSwitchedTimer > 0)
            {
                DrawSavestateSwitchedText();
                savestateSwitchedTimer++;
            }
            if (savestateSwitchedTimer > 30)
            {
                savestateSwitchedTimer = 0;
            }



            // Check for release
            {
                if (_currentButton != L1_BUTTON + R1_BUTTON + LEFT_BUTTON && _currentButton != L1_BUTTON + R1_BUTTON + RIGHT_BUTTON)
                {
                    switch_button_held = false;
                }
            }
            
        #endif
    }

    // Prepare savestate after dragon
    if(mod_state == UNLOCKED_LEVELS && (_gameState == GAMESTATE_DRAGON_STATE || _gameState == GAMESTATE_LOADING))
    {
        if(_currentButtonOneFrame == SAVESTATE_BUTTONS[savestate_button_index])
        {
            should_savestate_after_dragon_or_load = true;
        }
    }


    {     
        //Safeguard against loading with another levels savestate/no savestate
        if(_levelLoadState < 0xB) // Checking for a level load state before 0xB instaload, to ensure not removing savestate on instaload
        {
            _globalEggs = 0;

            if(savestated_level_ids[savestate_selection] != _levelID)
            {
                savestate_selection = 0;
            }

            // for(int i = 0; i < 3; i++){                     //Setting the IL Timer offsets back to 0 when going to a new level
            //     il_timer_offset[i] = 0;
            // }

        }   
    }

    // //! Green Screen Spyro
    // {
    //     // Nop Draw World
    //     if (CheckButtonMultiTap(TRIANGLE_BUTTON, 3) && is_greenscreen == false)
    //     {
    //         // JR RA Draw World
    //         *((int*)0x8002B9CC) = 0x03e00008;
    //         *((int*)0x8002B9D0) = 0x00000000;

    //         // JR RA Draw Skybox
    //         *((int*)0x8004EBA8) = 0x03e00008;
    //         *((int*)0x8004EBAC) = 0x00000000;
            
    //         is_greenscreen = true;
    //     }
    //     // Nop Draw World
    //     else if (CheckButtonMultiTap(CIRCLE_BUTTON, 2) && is_greenscreen == true)
    //     {
    //         printf("Back On\n");
    //         // Bring Back Draw World
    //         *((int*)0x8002B9CC) = 0x27BDFFE8;
    //         *((int*)0x8002B9D0) = 0x3C048007;

    //         // Bring Back Draw Skybox
    //         *((int*)0x8004EBA8) = 0x3C018007;
    //         *((int*)0x8004EBAC) = 0x24217DD8;

    //         is_greenscreen = false;
    //     }



    //VramTester();
    // //Every frame check to check for nopping MobyAnimationUpdate
    // {
    //     MobyAnimCrashFix();
    // }

    // {
    //     SkinTester();
    // }

}
