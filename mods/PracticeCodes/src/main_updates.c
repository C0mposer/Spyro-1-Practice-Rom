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
#include <vram.h>

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

const RedGreen bg_colors[7] = {{0x0, 0x25}, {0x40, 0x18}, {0x00, 0x50}, {0x50, 0x50}, {0x70, 0}, {0xCD, 0x80}, {0xA0, 0xA0}};

// Externed from elsewhere
extern BackgroundColor bg_color_index;
extern SpyroColor spyro_color_index;
extern FlameColor flame_color_index;

extern bool should_update_bg_color;
extern bool should_load_spyro_color;
extern bool should_load_flame_color;

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

//Changing asm instructions for pause menu RGB. Cannot change B value, as the value is in a shared register with other crucial parts of the struct.
inline void SetTitleScreenColor(byte r, byte g)
{
    *(short*)(0x8001A674) = r;
    *(short*)(0x8001A67C) = g;
}

void DrawSavestateSwitchedText(void)
{
    char buf[3] = {0};
    sprintf(&buf, "%d", savestate_selection + 1);
    DrawTextCapitals(buf, &(CapitalTextInfo){.x=0x100, .y=0xDC, .size=DEFAULT_SIZE}, DEFAULT_SPACING, MOBY_COLOR_PURPLE);
    MobyRender();
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
    //Change Spyro Skin
    if(should_load_spyro_color)
    {
        LoadSpyroBMPToMainRam(SKIN_SECTOR);
        should_load_spyro_color = false;
    }
    if(should_write_spyro_bmp && _isLoading == false)
    {
        WriteSpyroBMPToVram();
    }
    //Change Flame Skin
    if(should_load_flame_color)
    {
        LoadFlameBMPToMainRam(FLAME_SECTOR);
        should_load_flame_color = false;
    }
    if(should_write_flame_bmp && _isLoading == false)
    {
        WriteFlameBMPToVram();
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

        //Reload Spyro Skin in Load
        if(_levelLoadState == 0x7 && _gameState == GAMESTATE_LOADING && spyro_color_index > 0) 
        {
            should_write_spyro_bmp = true;    
        }   
        //Reload Flame Skin in Load
        if(_levelLoadState == 0x7 && _gameState == GAMESTATE_LOADING && flame_color_index > 0) 
        {
            should_write_flame_bmp = true;  
        }   
    }

    // //Every frame check to check for nopping MobyAnimationUpdate
    // {
    //     MobyAnimCrashFix();
    // }

    // {
    //     SkinTester();
    // }

}
