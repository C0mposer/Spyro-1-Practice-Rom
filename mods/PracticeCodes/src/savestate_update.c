#include <common.h>
#include <custom_text.h>
#include <sound.h>

bool should_savestate_on_game_start = true; // Initial savestate for nestor skip
bool should_savestate_after_dragon_or_load = false;

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

#if BUILD == 1 || BUILD == 3
extern bool respawn_on_loadstate;
bool readyToLoadstateAfterDeath = false;
#endif

void SaveStateUpdate()
{
    //Save spyro & camera information or Savestate depending on console
    if (_gameState == GAMESTATE_GAMEPLAY)
    {
        if (savestate_button_index < 2)
        {
            if (_currentButtonOneFrame == SAVESTATE_BUTTONS[savestate_button_index] || should_savestate_after_dragon_or_load || should_savestate_on_game_start)
            {
                #if BUILD == 2 || BUILD == 0 || BUILD == 5
                FullSaveState();
                #elif BUILD == 1 || BUILD == 3
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
                #if BUILD == 2 || BUILD == 0 || BUILD == 5
                FullSaveState();
                #elif BUILD == 1 || BUILD == 3
                SaveSpyroAndCamera(false);
                #endif
            }
        }


        // Load spyro & camera information or Loadstate depending on console
        #if BUILD == 2 || BUILD == 0 || BUILD == 5        // Real Loadstate
        if ((_currentButtonOneFrame == LOADSTATE_BUTTONS[loadstate_button_index]))
        {
            FullLoadState();

            if ((_movementSubState == MOVEMENT_SUBSTATE_FLY_IN_LOOP || _movementSubState == MOVEMENT_SUBSTATE_FLY_IN_CAMERA_180 || _movementSubState == MOVEMENT_SUBSTATE_FLY_IN_TREE_TOPS) && fly_in_resets_loadstate_timer == 0)
            {
                fly_in_resets[_levelIDIndex]++;
            }
            fly_in_resets_loadstate_timer = 1;

            if (_levelID == GNASTYS_LOOT_ID)
            {
                LootGiveAllKeys();
            }
        }
        else
        {
            just_loaded_state = false;
        }
        #elif BUILD == 1 || BUILD == 3      // Sudo Load-State
        if ((_currentButtonOneFrame == LOADSTATE_BUTTONS[loadstate_button_index] && hasSavedSpyro == true) || (readyToLoadstateAfterDeath == true && _effect_ScreenFadeIn != 0))
        {
            if (!respawn_on_loadstate || (_effect_ScreenFadeIn = 0, readyToLoadstateAfterDeath))
            {

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
        }
        #endif

        #if BUILD == 2 || BUILD == 0 || BUILD == 5  // Change Savestate Slots
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
        should_savestate_on_game_start = true;
    }

    // Prepare savestate after turning on disable portal
    bool does_savestate_already_exist = savestated_level_ids[savestate_selection] == _levelID;
    if (disable_portal_entry == true && has_savestated_on_disabling_portal == false)
    {
        has_savestated_on_disabling_portal = true;

        if (does_savestate_already_exist == false)
        {
            //printf("Savestated after enabling the disable portal option\n");
            #if BUILD == 2 || BUILD == 0 || BUILD == 5
            FullSaveState();
            #elif BUILD == 1 || BUILD == 3
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
}

void DrawSavestateSwitchedText(void)
{
    char buf[3] = { 0 };
    sprintf(buf, "%d", savestate_selection + 1);
    DrawTextCapitals(buf, &(CapitalTextInfo){.x = 0x100, .y = 0xDC, .size = DEFAULT_SIZE}, DEFAULT_SPACING, MOBY_COLOR_PURPLE);
    RenderShadedMobyQueue();
}

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



// Moved from Savestate.c for space
int appliedNopFixTimer = 0;
void LoadstateNopFixes(void)
{
    *(int*)0x80056528 = 0x00000000;					// NOP-ing the Vec3Length call in the SFX proccessing function. This fixes a weird bug with some specific sound sources crashing right after a loadstate

    appliedNopFixTimer = 1;
}

void RevertLoadstateNOPFixes(void)
{
    if (appliedNopFixTimer == 10)
        *(int*)0x80056528 = 0x0C005C7F;

    if (appliedNopFixTimer > 0)
    {
        appliedNopFixTimer++;
    }
}

// Check to revert NOP's every frame
void LoadstateFixesUpdate()
{
    RevertLoadstateNOPFixes();
}