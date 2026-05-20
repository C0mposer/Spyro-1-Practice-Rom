#include <common.h>
#include <custom_text.h>
#include <sound.h>
#include <custom_menu.h>
#include <right_stick.h>

bool should_savestate_on_game_start = false; // Initial savestate for nestor skip (is now a timer too)
bool should_savestate_after_dragon_or_load = false;
bool should_loadstate_after_dragon = false;

int savestated_level_ids[3] = { 0 }; // For keeping savestates upon loop

int savestateSwitchedTimer = 0;

bool switch_savestate_button_held;

bool just_loaded_state = false;

bool fly_in_resets_loadstate_timer = 0;

//From other files
extern int savestate_selection;

extern int savestate_button_index;
extern int loadstate_button_index;
extern int switch_state_button_index;

extern const short SAVESTATE_BUTTONS[2];
extern const short LOADSTATE_BUTTONS[3];

extern int ilTimerStart;
extern int il_timer_offset[3];

extern bool disable_portal_entry;
extern bool has_savestated_on_disabling_portal;

extern bool hasSavedSpyro;

extern short fly_in_resets[37];
extern GhostMenu ghost_menu;
extern VisualizerMenu visualizer_menu;

#if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
void GhostSaveState(int slot);
void GhostLoadState(int slot);
#endif
#if BUILD == PS2_DECKARD
void GhostInvalidateStoredGhosts(void);
#endif

#if BUILD == PS1 || BUILD == PS2_IOP
extern bool respawn_on_loadstate;
bool readyToLoadstateAfterDeath = false;
#endif

extern bool fly_in_resets_loadstate_timer;
void FlyInResetsLoadstateTimerUpdate(void)
{
    if (fly_in_resets_loadstate_timer > 0)
    {
        fly_in_resets_loadstate_timer++;
    }
    if (fly_in_resets_loadstate_timer >= 30)
    {
        fly_in_resets_loadstate_timer = 0;
    }
}


void SaveStateUpdate()
{
    //Save spyro & camera information or Savestate depending on console
    if (_gameState == GAMESTATE_GAMEPLAY)
    {
        if (savestate_button_index < 2)
        {
            if (_currentButtonOneFrame == SAVESTATE_BUTTONS[savestate_button_index] || should_savestate_after_dragon_or_load || should_savestate_on_game_start > 2)
            {
                #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
                FullSaveState();
                #if BUILD == PS2_DECKARD
                if (!ghost_menu.ghosts_enabled && savestate_selection > 0) // Mark ghost area as dirty
                {
                    GhostInvalidateStoredGhosts();
                }
                #endif
                #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
                GhostSaveState(savestate_selection);
                #endif
                #elif BUILD == PS1 || BUILD == PS2_IOP
                SaveSpyroAndCamera(false);
                #endif

                il_timer_offset[savestate_selection] = _globalTimer - ilTimerStart; // Save IL timer offset when savestate happens. In here instead of il_timer.c so it works on auto savestate after dragon

                should_savestate_after_dragon_or_load = false;
                should_savestate_on_game_start = false;

            }
        }
        if (savestate_button_index == 2) // for multi tap check
        {
            if (CheckButtonMultiTap(L3_BUTTON, 2))
            {
                #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
                FullSaveState();
                #if BUILD == PS2_DECKARD
                if (!ghost_menu.ghosts_enabled && savestate_selection > 0) // Mark ghost area as dirty
                {
                    GhostInvalidateStoredGhosts();
                }
                #endif
                #elif BUILD == PS1 || BUILD == PS2_IOP
                SaveSpyroAndCamera(false);
                #endif
            }
        }
    }

    if (_gameState == GAMESTATE_GAMEPLAY || (_gameState == GAMESTATE_DRAGON_STATE && (_dragonState < 1 || _dragonState == 7))) {
        // Load spyro & camera information or Loadstate depending on console
        #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION        // Real Loadstate
        if ((_currentButtonOneFrame == LOADSTATE_BUTTONS[loadstate_button_index]) || (should_loadstate_after_dragon))
        {
            #define DRAGON_SOUND_HANDLE *((u32*)0x80077084) //don't wanna make a symbol lol
            // if (_dragonState == 1) {                             // FIX SOUND CORRUPTION
            //     _dragonState = 6;
            //     *(int*)(&_dragonState + 1) = 4;
            //     FreeSoundSlot(DRAGON_SOUND_HANDLE, 4);
            //     FlushSPUKeyOnKeyOff();
            //     should_loadstate_after_dragon = true;
            // }
        // else {
            if (_gameState == GAMESTATE_DRAGON_STATE) {
                ExitDragon();
                should_loadstate_after_dragon = false;
            }

            FullLoadState();
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            GhostLoadState(savestate_selection);
            #endif

            if ((_movementSubState == MOVEMENT_SUBSTATE_FLY_IN_LOOP || _movementSubState == MOVEMENT_SUBSTATE_FLY_IN_CAMERA_180 || _movementSubState == MOVEMENT_SUBSTATE_FLY_IN_TREE_TOPS) && fly_in_resets_loadstate_timer == 0)
            {
                fly_in_resets[_levelIDIndex]++;
            }
            fly_in_resets_loadstate_timer = 1;

            // if (_levelID == GNASTYS_LOOT_ID)
            // {
            //     LootGiveAllKeys();
            // }
        // }
        }
        else
        {
            just_loaded_state = false;
        }
        #elif BUILD == PS1 || BUILD == PS2_IOP      // Sudo Load-State
        if ((_currentButtonOneFrame == LOADSTATE_BUTTONS[loadstate_button_index] && hasSavedSpyro == true)
            || should_loadstate_after_dragon
            || (readyToLoadstateAfterDeath == true && _effect_ScreenFadeIn != 0))
        {
            if (_dragonState == 1) {
                _dragonState = 6;
                *(int*)(&_dragonState + 1) = 4;
                should_loadstate_after_dragon = true;
            }
            else {
                if (respawn_on_loadstate) {
                    _effect_ScreenFadeIn = 0;
                }

                if (!respawn_on_loadstate || readyToLoadstateAfterDeath)
                {
                    if (_gameState == GAMESTATE_DRAGON_STATE) {
                        *(short*)(*(int*)0x800770c0 + 0x48) = 1;
                        ExitDragon();
                    }

                    ReloadSpyroAndCamera(false);

                    readyToLoadstateAfterDeath = false;

                    if (_levelID == TREE_TOPS_ID)
                    {
                        _treeTopsThiefTimer = 0;
                        _isInInGameCutscene = false;
                        _effect_ScreenLetterBox = 0;
                    }

                    if (_levelID == GNASTYS_LOOT_ID)
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
                should_loadstate_after_dragon = false;
            }
        }
        #endif
    }


    // Change Savestate Slot in either gameplay, or dragon state
    if (_gameState == GAMESTATE_GAMEPLAY || _gameState == GAMESTATE_DRAGON_STATE)
    {
        #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION  // Change Savestate Slots
        if (ghost_menu.ghosts_enabled == false)
        {
            if (visualizer_menu.free_cam == true)
            {
                SuppressHorizontalRightStickDirection();
            }
            else
            {
                // Quick Savestate Slot Selection
                if (switch_state_button_index == 0)
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
                    if (_currentButton == L1_BUTTON + R1_BUTTON + LEFT_BUTTON && savestate_selection > 0 && !switch_savestate_button_held)
                    {
                        savestate_selection--;
                        PlaySoundEffect(SOUND_EFFECT_PAUSE_MENU_CHANGE_SELECTION_DING, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
                        savestateSwitchedTimer = 1;

                        switch_savestate_button_held = true;
                    }
                    if (_currentButton == L1_BUTTON + R1_BUTTON + RIGHT_BUTTON && savestate_selection < 2 && !switch_savestate_button_held)
                    {
                        savestate_selection++;
                        PlaySoundEffect(SOUND_EFFECT_PAUSE_MENU_CHANGE_SELECTION_DING, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
                        savestateSwitchedTimer = 1;

                        switch_savestate_button_held = true;
                    }
                }
            }
            // Draw Savestate Slot Text
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
                    switch_savestate_button_held = false;
                }
            }
        }
        #endif
    }

        // Prepare savestate after dragon or portal
    if (_gameState == GAMESTATE_DRAGON_STATE || _gameState == GAMESTATE_LOADING)
    {
        if (_currentButtonOneFrame == SAVESTATE_BUTTONS[savestate_button_index])
        {
            should_savestate_after_dragon_or_load = true;
        }
    }

    // Allow for initial savestate whenever going back to adventure continues
    if (_gameState == GAMESTATE_TITLE_SCREEN)
    {
        //should_savestate_on_game_start = true;
    }

    // Prepare savestate after turning on disable portal
    bool does_savestate_already_exist = savestated_level_ids[savestate_selection] == _levelID;
    if (disable_portal_entry == true && has_savestated_on_disabling_portal == false)
    {
        has_savestated_on_disabling_portal = true;

        if (does_savestate_already_exist == false)
        {
            //printf("Savestated after enabling the disable portal option\n");
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            FullSaveState();
            #if BUILD == PS2_DECKARD
            if (!ghost_menu.ghosts_enabled && savestate_selection > 0) // Mark ghost area as dirty
            {
                GhostInvalidateStoredGhosts();
            }
            #endif
            #elif BUILD == PS1 || BUILD == PS2_IOP
            SaveSpyroAndCamera(false);
            #endif
        }

    }

    // Undo has_savestated_on_disabling_portal bool
    bool has_turned_off_disable_portal = (disable_portal_entry == false && has_savestated_on_disabling_portal == true);
    if (has_turned_off_disable_portal)
    {
        has_savestated_on_disabling_portal = false;
    }

    {
        //Safeguard against loading with another levels savestate/no savestate
        if (_levelLoadState < 0xB) // Checking for a level load state before 0xB instaload, to ensure not removing savestate on instaload
        {
            hasSavedSpyro = false;

            //Change savestate slot to 0 upon leaving level
            if (savestated_level_ids[savestate_selection] != _levelID)
            {
                savestate_selection = 0;
            }

            _globalEggs = 0;

            // for(int i = 0; i < 3; i++){                     //Setting the IL Timer offsets back to 0 when going to a new level
            //     il_timer_offset[i] = 0;
            // }

        }
    }

    FlyInResetsLoadstateTimerUpdate();

    if (should_savestate_on_game_start > 0)
        should_savestate_on_game_start++;   // This is a bit hacky, but basically because save stating the frame the game starts causes nestor skip to act strange, we need to wait a few frames. To save space, im sharing the bool logic and timer in one (true/1 is should count up.)
}


