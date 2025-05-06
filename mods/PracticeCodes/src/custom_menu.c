#include <common.h>
#include <custom_menu.h>
#include <custom_text.h>
#include <sound.h>
#include <shared_funcs.h>
#include <main_updates.h>
#include <cosmetic.h>
#include <igt.h>
#include <moby.h>
#include <cd.h>

Menu custom_menu = { 0 };
ILMenu il_menu = { 0 };
TimerMenu timer_menu = { 0 };
SavestateMenu savestate_menu = { 0 };
MiscMenu misc_menu = { 0 };
ILDisplayMenu il_display_modes = { 0 };

bool has_toggled_menu = FALSE;
MenuState menu_state = MENU_HIDDEN;
CurrentMenu current_menu = MAIN_MENU;
SuperflyMode custom_superfly_state = false;

bool isMenuButtonHeld = false;
int time_in_display_menu = 0;

const short STOP_TIMER_BUTTONS[2] = { START_BUTTON, SELECT_BUTTON };
const short RESET_TIMER_BUTTONS[2] = { R3_BUTTON, SELECT_BUTTON };

const short SAVESTATE_BUTTONS[2] = { L3_BUTTON, START_BUTTON };
const short LOADSTATE_BUTTONS[3] = { R3_BUTTON, SELECT_BUTTON, (L2_BUTTON + R2_BUTTON + CIRCLE_BUTTON) };

int savestate_button_index;
int loadstate_button_index;
int savestate_selection = 0;

#if BUILD == 2 || BUILD == 0 || BUILD == 5
int switch_state_button_index;
#elif BUILD == 1 || BUILD == 3
bool respawn_on_loadstate = TRUE;
#endif

int oldCdLocation[2];   // Array of 2 ints because the seek location takes up 8 bytes


// Externed from elsewhere
extern bool show_sparx_range_mode; // From custom_menu_2.c

// From il_timer.c
typedef enum ILTimerState
{
    IL_FLYING_IN,
    IL_STARTED,
    IL_DISPLAYING,
    IL_STOPPED
} ILTimerState;
extern ILTimerState il_timer_state;
extern int mainTimerAtReset;


//! Every Frame Update
void CustomMenuUpdate(void)
{
    // Open Menu
    if (_currentButton == L2_BUTTON + R2_BUTTON + TRIANGLE_BUTTON && !has_toggled_menu && menu_state == MENU_HIDDEN && _movementSubState != MOVEMENT_SUBSTATE_FLY_IN_TREE_TOPS)
    {
        menu_state = !menu_state;
        has_toggled_menu = TRUE;
        PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);

        if (menu_state == MENU_HIDDEN)
        {
            _spyro.isMovementLocked = FALSE;
            current_menu = MAIN_MENU;
            CdControlB(CDL_PRIMITIVE_SEEKL, (void*)&oldCdLocation, NULL);
            PlayMusic(_currentMusicTrack, 8);
        }
        else if (menu_state == MENU_DISPLAYING)
        {
            StopDrawWorldAndObjects();
        }
    }

    // When Displaying Menu
    if (menu_state == MENU_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
    {
        if (current_menu == MAIN_MENU)
        {
            CapitalTextInfo menu_text_info[5] = { {0} };

            // Easy Exit
            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                menu_state = MENU_HIDDEN;
                _spyro.isMovementLocked = FALSE;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
                RestartDrawWorldAndObjects();
            }

            DrawTextBox(0x30, 0x1D0, 0x30, 0xAA);

            menu_text_info[0].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[0].y = 70;
            menu_text_info[0].size = DEFAULT_SIZE;

            menu_text_info[1].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[1].y = 90;
            menu_text_info[1].size = DEFAULT_SIZE;

            menu_text_info[2].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[2].y = 110;
            menu_text_info[2].size = DEFAULT_SIZE;

            menu_text_info[3].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[3].y = 130;
            menu_text_info[3].size = DEFAULT_SIZE;

            menu_text_info[4].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[4].y = 150;
            menu_text_info[4].size = DEFAULT_SIZE;

            _spyro.isMovementLocked = TRUE;

            if (custom_menu.selection == 0)
            {
                DrawTextCapitals(custom_menu.il_menu_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(custom_menu.il_menu_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (custom_menu.selection == 1)
            {
                DrawTextCapitals(custom_menu.timer_menu_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(custom_menu.timer_menu_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (custom_menu.selection == 2)
            {
                DrawTextCapitals(custom_menu.savestate_menu_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(custom_menu.savestate_menu_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (custom_menu.selection == 3)
            {
                DrawTextCapitals(custom_menu.misc_menu_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(custom_menu.misc_menu_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (custom_menu.selection == 4)
            {
                DrawTextCapitals(custom_menu.cosmetic_menu_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(custom_menu.cosmetic_menu_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            // Fill text with defaults if NULL
            if (custom_menu.il_menu_text == NULL)
            {
                custom_menu.il_menu_text = "IL SETTINGS";
                custom_menu.timer_menu_text = "MANUAL TIMER SETTINGS";
                custom_menu.savestate_menu_text = "SAVESTATE SETTINGS";
                custom_menu.misc_menu_text = "MISC SETTINGS";
                custom_menu.cosmetic_menu_text = "COSMETIC SETTINGS";
            }

            // Change Selection
            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                custom_menu.selection = (custom_menu.selection + 1) % 5;
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                custom_menu.selection = (custom_menu.selection + 4) % 5;
            }

            // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == X_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Timer Selection
            if (custom_menu.selection == 0)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = IL_MENU;
                    return; // HACK HACK HACK
                }
            }

            else if (custom_menu.selection == 1)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = MANUAL_TIMER_MENU;
                    return; // HACK HACK HACK
                }
            }

            // Savestate Selection
            else if (custom_menu.selection == 2)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = SAVESTATE_MENU;
                    return; // HACK HACK HACK
                }
            }

            // Sparx Selection
            else if (custom_menu.selection == 3)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = MISC_MENU;
                    return; // HACK HACK HACK
                }
            }

            else if (custom_menu.selection == 4)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = COSMETIC_MENU;
                    CdControlB(CDL_PRIMITIVE_GETlocL, NULL, (void*)&oldCdLocation);
                    return; // HACK HACK HACK
                }
            }
        }

        if (current_menu == IL_MENU)
        {
            CapitalTextInfo menu_text_info[3] = { {0} };

            // Easy Exit
            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                current_menu = MAIN_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            DrawTextBox(0x30, 0x1D0, 0x30, 0x80);

            menu_text_info[0].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[0].y = 70;
            menu_text_info[0].size = DEFAULT_SIZE;

            menu_text_info[1].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[1].y = 90;
            menu_text_info[1].size = DEFAULT_SIZE;

            menu_text_info[2].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[2].y = 110;
            menu_text_info[2].size = DEFAULT_SIZE;

            _spyro.isMovementLocked = TRUE;

            if (il_menu.selection == 0)
            {
                DrawTextCapitals(il_menu.il_mode_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_menu.il_mode_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (il_menu.il_state != true)
            {
                DrawTextCapitals(il_menu.loop_level_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if (il_menu.selection == 1)
            {
                DrawTextCapitals(il_menu.loop_level_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_menu.loop_level_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if (il_menu.il_state != true)
            {
                DrawTextCapitals(il_menu.display_modes_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if (il_menu.selection == 2)
            {
                DrawTextCapitals(il_menu.display_modes_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_menu.display_modes_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            // Fill text with defaults if NULL
            if (il_menu.il_mode_text == NULL)
            {
                il_menu.il_mode_text = "IL MODE OFF";
                il_menu.loop_level_text = "LOOP LEVEL ON";
                il_menu.display_modes_text = "DISPLAY SETTINGS";
            }

            // Change Selection
            if (il_menu.il_state == true) {
                if (_currentButtonOneFrame == DOWN_BUTTON)
                {
                    il_menu.selection = (il_menu.selection + 1) % 3;
                }
                else if (_currentButtonOneFrame == UP_BUTTON)
                {
                    il_menu.selection = (il_menu.selection + 2) % 3;
                }
            }
            // Put cursor back at top if turned off through default settings
            else if (il_menu.il_state == false)
            {
                il_menu.selection = 0;
            }

            // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Update Text
            if (il_menu.il_state == false)
            {
                il_menu.il_mode_text = "IL MODE OFF";
            }
            else
            {
                il_menu.il_mode_text = "IL MODE ON";
            }
            if (il_menu.dont_loop_level == false)
            {
                il_menu.loop_level_text = "LOOP LEVEL ON";
            }
            else
            {
                il_menu.loop_level_text = "LOOP LEVEL OFF";
            }

            // Update Menu Options
            if (il_menu.selection == 0)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_menu.il_state = (il_menu.il_state + 1) % 2;
                }

            }
            else if (il_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_menu.dont_loop_level = (il_menu.dont_loop_level + 1) % 2;
                }

            }
            else if (il_menu.selection == 2)
            {

                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = IL_DISPLAY_OPTIONS_MENU;
                }
            }

            time_in_display_menu = 0;
        }

        if (current_menu == IL_DISPLAY_OPTIONS_MENU)
        {
            CapitalTextInfo menu_text_info[10] = { {0} };

            // Easy Exit
            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                current_menu = IL_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            DrawTextBox(0x30, 0x1F0, 0x0C, 0xDF);

            menu_text_info[0].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[0].y = 30;
            menu_text_info[0].size = DEFAULT_SIZE;

            menu_text_info[1].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[1].y = 50;
            menu_text_info[1].size = DEFAULT_SIZE;

            menu_text_info[2].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[2].y = 70;
            menu_text_info[2].size = DEFAULT_SIZE;

            menu_text_info[3].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[3].y = 90;
            menu_text_info[3].size = DEFAULT_SIZE;

            menu_text_info[4].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[4].y = 110;
            menu_text_info[4].size = DEFAULT_SIZE;

            menu_text_info[5].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[5].y = 130;
            menu_text_info[5].size = DEFAULT_SIZE;

            menu_text_info[6].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[6].y = 150;
            menu_text_info[6].size = DEFAULT_SIZE;

            menu_text_info[7].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[7].y = 170;
            menu_text_info[7].size = DEFAULT_SIZE;

            menu_text_info[8].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[8].y = 190;
            menu_text_info[8].size = DEFAULT_SIZE;

            menu_text_info[9].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[9].y = 210;
            menu_text_info[9].size = DEFAULT_SIZE;

            _spyro.isMovementLocked = TRUE;

            if (il_display_modes.selection == 0)
            {
                DrawTextCapitals(il_display_modes.il_display_enable_all_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_display_modes.il_display_enable_all_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if (il_display_modes.selection == 1)
            {
                DrawTextCapitals(il_display_modes.il_display_dragon_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_display_modes.il_display_dragon_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if (il_display_modes.selection == 2)
            {
                DrawTextCapitals(il_display_modes.il_display_landing_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_display_modes.il_display_landing_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if (il_display_modes.selection == 3)
            {
                DrawTextCapitals(il_display_modes.il_display_glide_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_display_modes.il_display_glide_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if (il_display_modes.selection == 4)
            {
                DrawTextCapitals(il_display_modes.il_display_flame_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_display_modes.il_display_flame_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if (il_display_modes.selection == 5)
            {
                DrawTextCapitals(il_display_modes.il_display_gem_text, &menu_text_info[5], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_display_modes.il_display_gem_text, &menu_text_info[5], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if (il_display_modes.selection == 6)
            {
                DrawTextCapitals(il_display_modes.il_display_flight_text, &menu_text_info[6], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_display_modes.il_display_flight_text, &menu_text_info[6], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if (il_display_modes.selection == 7)
            {
                DrawTextCapitals(il_display_modes.il_display_whirlwind_text, &menu_text_info[7], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_display_modes.il_display_whirlwind_text, &menu_text_info[7], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if (il_display_modes.selection == 8)
            {
                DrawTextCapitals(il_display_modes.il_display_bonk_text, &menu_text_info[8], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_display_modes.il_display_bonk_text, &menu_text_info[8], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if (il_display_modes.selection == 9)
            {
                DrawTextCapitals(il_display_modes.il_display_always_text, &menu_text_info[9], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_display_modes.il_display_always_text, &menu_text_info[9], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            // Fill text with defaults if NULL
            if (il_display_modes.il_display_enable_all_text == NULL)
            {
                il_display_modes.il_display_enable_all_text = "ENABLE ALL";
                il_display_modes.il_display_dragon_text = "DISPLAY DRAGON OFF";
                il_display_modes.il_display_landing_text = "DISPLAY LANDING OFF";
                il_display_modes.il_display_glide_text = "DISPLAY GLIDE OFF";
                il_display_modes.il_display_flame_text = "DISPLAY FLAME OFF";
                il_display_modes.il_display_gem_text = "DISPLAY GEM COLLECT OFF";
                il_display_modes.il_display_flight_text = "DISPLAY FLIGHT ITEM OFF";
                il_display_modes.il_display_whirlwind_text = "DISPLAY WHIRLWIND OFF";
                il_display_modes.il_display_bonk_text = "DISPLAY BONK OFF";
                il_display_modes.il_display_always_text = "DISPLAY ALWAYS OFF";
            }

            // Change Selection
            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                il_display_modes.selection = (il_display_modes.selection + 1) % 10;
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                il_display_modes.selection = (il_display_modes.selection + 9) % 10;
            }


        // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == X_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Update Displays
            if (il_display_modes.il_has_enabled_all == false)
            {
                il_display_modes.il_display_enable_all_text = "ENABLE ALL";
            }
            else
            {
                il_display_modes.il_display_enable_all_text = "DISABLE ALL";
            }
            if (il_display_modes.il_display_dragon == false)
            {
                il_display_modes.il_display_dragon_text = "DISPLAY DRAGON OFF";
            }
            else
            {
                il_display_modes.il_display_dragon_text = "DISPLAY DRAGON ON";
            }

            if (il_display_modes.il_display_landing == false)
            {
                il_display_modes.il_display_landing_text = "DISPLAY LANDING OFF";
            }
            else
            {
                il_display_modes.il_display_landing_text = "DISPLAY LANDING ON";
            }
            if (il_display_modes.il_display_glide == false)
            {
                il_display_modes.il_display_glide_text = "DISPLAY GLIDE OFF";
            }
            else
            {
                il_display_modes.il_display_glide_text = "DISPLAY GLIDE ON";
            }
            if (il_display_modes.il_display_flame == false)
            {
                il_display_modes.il_display_flame_text = "DISPLAY FLAME OFF";
            }
            else
            {
                il_display_modes.il_display_flame_text = "DISPLAY FLAME ON";
            }
            if (il_display_modes.il_display_gem == false)
            {
                il_display_modes.il_display_gem_text = "DISPLAY GEM COLLECT OFF";
            }
            else
            {
                il_display_modes.il_display_gem_text = "DISPLAY GEM COLLECT ON";
            }
            if (il_display_modes.il_display_flight == false)
            {
                il_display_modes.il_display_flight_text = "DISPLAY FLIGHT ITEM OFF";
            }
            else
            {
                il_display_modes.il_display_flight_text = "DISPLAY FLIGHT ITEM ON";
            }
            if (il_display_modes.il_display_whirlwind == false)
            {
                il_display_modes.il_display_whirlwind_text = "DISPLAY WHIRLWIND OFF";
            }
            else
            {
                il_display_modes.il_display_whirlwind_text = "DISPLAY WHIRLWIND ON";
            }
            if (il_display_modes.il_display_bonk == false)
            {
                il_display_modes.il_display_bonk_text = "DISPLAY BONK OFF";
            }
            else
            {
                il_display_modes.il_display_bonk_text = "DISPLAY BONK ON";
            }
            if (il_display_modes.il_display_always == false)
            {
                il_display_modes.il_display_always_text = "DISPLAY ALWAYS OFF";
            }
            else
            {
                il_display_modes.il_display_always_text = "DISPLAY ALWAYS ON";
            }

            // Enable all
            if (il_display_modes.selection == 0)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || (_currentButtonOneFrame == X_BUTTON && time_in_display_menu > 10) && il_display_modes.il_has_enabled_all == false)
                {
                    il_display_modes.il_display_dragon = true;
                    il_display_modes.il_display_landing = true;
                    il_display_modes.il_display_glide = true;
                    il_display_modes.il_display_flame = true;
                    il_display_modes.il_display_gem = true;
                    il_display_modes.il_display_flight = true;
                    il_display_modes.il_display_whirlwind = true;
                    il_display_modes.il_display_bonk = true;
                    il_display_modes.il_has_enabled_all = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON || (_currentButtonOneFrame == X_BUTTON && time_in_display_menu > 10) && il_display_modes.il_has_enabled_all == true)
                {
                    il_display_modes.il_display_dragon = false;
                    il_display_modes.il_display_landing = false;
                    il_display_modes.il_display_glide = false;
                    il_display_modes.il_display_flame = false;
                    il_display_modes.il_display_gem = false;
                    il_display_modes.il_display_flight = false;
                    il_display_modes.il_display_whirlwind = false;
                    il_display_modes.il_display_bonk = false;
                    il_display_modes.il_has_enabled_all = false;
                }
            }

            else if (il_display_modes.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_display_modes.il_display_dragon = (il_display_modes.il_display_dragon + 1) % 2;
                }

            }

            else if (il_display_modes.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_display_modes.il_display_landing = (il_display_modes.il_display_landing + 1) % 2;
                }
            }

            else if (il_display_modes.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_display_modes.il_display_glide = (il_display_modes.il_display_glide + 1) % 2;
                }
            }

            else if (il_display_modes.selection == 4)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_display_modes.il_display_flame = (il_display_modes.il_display_flame + 1) % 2;
                }
            }

            else if (il_display_modes.selection == 5)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_display_modes.il_display_gem = (il_display_modes.il_display_gem + 1) % 2;
                }
            }

            else if (il_display_modes.selection == 6)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_display_modes.il_display_flight = (il_display_modes.il_display_flight + 1) % 2;
                }
            }

            else if (il_display_modes.selection == 7)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_display_modes.il_display_whirlwind = (il_display_modes.il_display_whirlwind + 1) % 2;
                }
            }

            else if (il_display_modes.selection == 8)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_display_modes.il_display_bonk = (il_display_modes.il_display_bonk + 1) % 2;
                }
            }

            else if (il_display_modes.selection == 9)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_display_modes.il_display_always = (il_display_modes.il_display_always + 1) % 2;
                }
            }

            time_in_display_menu++;
        }


        if (current_menu == MANUAL_TIMER_MENU)
        {
            CapitalTextInfo menu_text_info[4] = { {0} };

            // Easy Exit
            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                current_menu = MAIN_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            DrawTextBox(0x30, 0x1F4, 0x30, 0x98);

            menu_text_info[0].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[0].y = 70;
            menu_text_info[0].size = DEFAULT_SIZE;

            menu_text_info[1].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[1].y = 90;
            menu_text_info[1].size = DEFAULT_SIZE;

            menu_text_info[2].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[2].y = 110;
            menu_text_info[2].size = DEFAULT_SIZE;

            menu_text_info[3].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[3].y = 130;
            menu_text_info[3].size = DEFAULT_SIZE;

            _spyro.isMovementLocked = TRUE;

            if (timer_menu.selection == 0)
            {
                DrawTextCapitals(timer_menu.timer_state_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(timer_menu.timer_state_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (timer_menu.timer_state != true)
            {
                DrawTextCapitals(timer_menu.timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if (timer_menu.selection == 1)
            {
                DrawTextCapitals(timer_menu.timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(timer_menu.timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (timer_menu.timer_state != true)
            {
                DrawTextCapitals(timer_menu.stop_timer_button_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if (timer_menu.selection == 2)
            {
                DrawTextCapitals(timer_menu.stop_timer_button_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(timer_menu.stop_timer_button_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (timer_menu.timer_state != true)
            {
                DrawTextCapitals(timer_menu.reset_timer_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if (timer_menu.selection == 3)
            {
                DrawTextCapitals(timer_menu.reset_timer_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(timer_menu.reset_timer_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            // Fill text with defaults if NULL
            if (timer_menu.timer_state_text == NULL)
            {
                timer_menu.timer_state_text = "MANUAL TIMER OFF";
                timer_menu.timer_display_mode_text = "DISPLAY ON STOP";
                timer_menu.stop_timer_button_text = "STOP BUTTON START";
                timer_menu.reset_timer_button_text = "RESET ON LOAD AND RESET";
            }

            // Change Selection
            if (timer_menu.timer_state == true) {
                if (_currentButtonOneFrame == DOWN_BUTTON)
                {
                    timer_menu.selection = (timer_menu.selection + 1) % 4;
                }
                else if (_currentButtonOneFrame == UP_BUTTON)
                {
                    timer_menu.selection = (timer_menu.selection + 3) % 4;
                }
            }

            // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Timer Selection
            if (timer_menu.selection == 0)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    timer_menu.timer_state = (timer_menu.timer_state + 1) % 2;
                }

                if (timer_menu.timer_state == FALSE)
                {
                    timer_menu.timer_state_text = "MANUAL TIMER OFF";
                }
                else
                {
                    timer_menu.timer_state_text = "MANUAL TIMER ON";
                }
            }

            else if (timer_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    timer_menu.timer_display_mode = (timer_menu.timer_display_mode + 1) % 2;
                    mainTimerAtReset = _globalTimer;
                }

                if (timer_menu.timer_display_mode == IL_TIMER_AT_END)
                {
                    timer_menu.timer_display_mode_text = "DISPLAY ON STOP";
                }
                else
                {
                    timer_menu.timer_display_mode_text = "DISPLAY ALWAYS";
                }
            }

            else if (timer_menu.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    timer_menu.stop_timer_button_index = (timer_menu.stop_timer_button_index + 1) % 2;
                }

                if (timer_menu.stop_timer_button_index == 0)
                {
                    timer_menu.stop_timer_button_text = "STOP BUTTON START";
                }
                else
                {
                    timer_menu.stop_timer_button_text = "STOP BUTTON SELECT";
                }
            }

            else if (timer_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    timer_menu.reset_timer_mode = (timer_menu.reset_timer_mode + 1) % 2;
                }

                if (timer_menu.reset_timer_mode == 0)
                {
                    timer_menu.reset_timer_button_text = "RESET ON LOAD AND RESET";
                }
                else
                {
                    timer_menu.reset_timer_button_text = "RESET ON LOAD ONLY";
                }
            }
        }

        if (current_menu == SAVESTATE_MENU)
        {
            // Set menu to 1 by default if not on DECKARD
            #if BUILD == 1 || BUILD == 3
            if (savestate_menu.selection == 0)
                savestate_menu.selection = 1;
            #endif

            CapitalTextInfo menu_text_info[4] = { {0} };

            // Easy Exit
            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                current_menu = MAIN_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }
            DrawTextBox(0x30, 0x1F4, 0x30, 0x98);

            menu_text_info[0].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[0].y = 70;
            menu_text_info[0].size = DEFAULT_SIZE;

            menu_text_info[1].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[1].y = 90;
            menu_text_info[1].size = DEFAULT_SIZE;

            menu_text_info[2].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[2].y = 110;
            menu_text_info[2].size = DEFAULT_SIZE;

            menu_text_info[3].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[3].y = 130;
            menu_text_info[3].size = DEFAULT_SIZE;

            _spyro.isMovementLocked = TRUE;

            #if BUILD == 2 || BUILD == 0 || BUILD == 5 // DECKARD HAS SAVESTATE SLOT
            if (savestate_menu.selection == 0)
            {
                DrawTextCapitals(savestate_menu.stateslot_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(savestate_menu.stateslot_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            #elif BUILD == 1 || BUILD == 3 // GREY OUT OPTION FOR OTHER PLATFORMS

            DrawTextCapitals(savestate_menu.stateslot_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);

            #endif

            if (savestate_menu.selection == 1)
            {
                DrawTextCapitals(savestate_menu.savestate_button_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(savestate_menu.savestate_button_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (savestate_menu.selection == 2)
            {
                DrawTextCapitals(savestate_menu.loadstate_button_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(savestate_menu.loadstate_button_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            #if BUILD == 2 || BUILD == 0 || BUILD == 5
            if (savestate_menu.selection == 3)
            {
                DrawTextCapitals(savestate_menu.switch_state_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(savestate_menu.switch_state_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            #elif BUILD == 1 || BUILD == 3
            if (savestate_menu.selection == 3)
            {
                DrawTextCapitals(savestate_menu.respawn_on_loadstate_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(savestate_menu.respawn_on_loadstate_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            #endif

            // Fill text with defaults if NULL
            #if BUILD == 2 || BUILD == 0 || BUILD == 5
            if (savestate_menu.stateslot_text == NULL)
            {
                savestate_menu.stateslot_text = "CURRENT SLOT 1";
                savestate_menu.savestate_button_text = "SAVE BUTTON L3";
                savestate_menu.loadstate_button_text = "LOAD BUTTON R3";
                savestate_menu.switch_state_button_text = "SWITCH SLOT RSTICK";
            }
            #elif BUILD == 1 || BUILD == 3
            if (savestate_menu.stateslot_text == NULL)
            {
                savestate_menu.stateslot_text = "CURRENT SLOT 1";
                savestate_menu.savestate_button_text = "SAVE BUTTON L3";
                savestate_menu.loadstate_button_text = "LOAD BUTTON R3";
                savestate_menu.respawn_on_loadstate_text = "RESPAWN ON LOADSTATE ON";
            }
            #endif

            // Change Selection
            #if BUILD == 2 || BUILD == 0 || BUILD == 5
            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                savestate_menu.selection = (savestate_menu.selection + 1) % 4;
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                savestate_menu.selection = (savestate_menu.selection + 3) % 4;      // +3 because it's the same as -1 in mod 4 math
            }
            #elif BUILD == 1 || BUILD == 3
            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                savestate_menu.selection = savestate_menu.selection % 3 + 1;        // +1 outside the mod operator to avoid it ever being 0
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                savestate_menu.selection = (savestate_menu.selection + 1) % 3 + 1;  // +1 outside the mod operator to avoid it ever being 0 and +1 inside to make it effectively +2 overal to make it the same as -1
            }
            #endif

                        // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            if (savestate_menu.selection == 0)
            {
                #if BUILD == 2 || BUILD == 0 || BUILD == 5
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    savestate_selection = (savestate_selection + 1) % 3;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    savestate_selection = (savestate_selection + 2) % 3;
                }

                if (savestate_selection == 0)
                {
                    savestate_menu.stateslot_text = "CURRENT SLOT 1";
                }
                else if (savestate_selection == 1)
                {
                    savestate_menu.stateslot_text = "CURRENT SLOT 2";
                }
                else
                {
                    savestate_menu.stateslot_text = "CURRENT SLOT 3";
                }
                #endif
            }

            else if (savestate_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    savestate_button_index = (savestate_button_index + 1) % 3;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    savestate_button_index = (savestate_button_index + 2) % 3;
                }

                if (savestate_button_index == 0)
                {
                    savestate_menu.savestate_button_text = "SAVE BUTTON L3";
                }
                else if (savestate_button_index == 1)
                {
                    savestate_menu.savestate_button_text = "SAVE BUTTON START";
                }
                else
                {
                    savestate_menu.savestate_button_text = "SAVE BUTTON L3 X2";
                }
            }

            else if (savestate_menu.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    loadstate_button_index = (loadstate_button_index + 1) % 2;
                }

                if (loadstate_button_index == 0)
                {
                    savestate_menu.loadstate_button_text = "LOAD BUTTON R3";
                    ChangeInventoryMenu(ON);
                }
                else
                {
                    savestate_menu.loadstate_button_text = "LOAD BUTTON SELECT";
                    ChangeInventoryMenu(OFF);
                }
            }

            #if BUILD == 2 || BUILD == 0 || BUILD == 5
            else if (savestate_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    switch_state_button_index = (switch_state_button_index + 1) % 2;
                }

                if (switch_state_button_index == 0)
                {
                    savestate_menu.switch_state_button_text = "SWITCH SLOT RSTICK";
                }
                else
                {
                    savestate_menu.switch_state_button_text = "SWITCH SLOT L1 R1 DPAD";
                }
            }
            #elif BUILD == 1 || BUILD == 3
            else if (savestate_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    respawn_on_loadstate = (respawn_on_loadstate + 1) % 2;
                }

                if (respawn_on_loadstate == 1)
                {
                    savestate_menu.respawn_on_loadstate_text = "RESPAWN ON LOADSTATE ON";
                }
                else
                {
                    savestate_menu.respawn_on_loadstate_text = "RESPAWN ON LOADSTATE OFF";
                }
            }
            #endif
        }
    }

    // Has Released Menu Button
    if (!(_currentButton & L2_BUTTON + R2_BUTTON + TRIANGLE_BUTTON))
    {
        has_toggled_menu = FALSE;
    }

    // Has Released Button
    if (!(_currentButton & L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON))
    {
        isMenuButtonHeld = FALSE;
    }
    // Has Released Button
    if (!(_currentButton & L1_BUTTON + R1_BUTTON + TRIANGLE_BUTTON))
    {
        isMenuButtonHeld = FALSE;
    }

    //! Checks
    {
        SparxChecks();

        // Superfly/Supercharge checks

        // Turn on Supercharge
        if (misc_menu.super_mode == true)
        {
            SuperchargeUpdate();
        }
        //Turn on Superfly
        if (_gameState == GAMESTATE_GAMEPLAY)
        {
            if (misc_menu.super_mode == true && _levelID == _levelIDPortalExit && _movementSubState == 0 && (custom_superfly_state == SUPERFLY_NOT_SET || custom_superfly_state == SUPERFLY_TURNED_OFF))
            {
                custom_superfly_state = SUPERFLY_TURNED_ON;
                //printf("SUPERFLY ON\n");
                _spyro.canSuperfly = true;
            }
        }
            //Turn off supercharge during portal entry
        if ((misc_menu.super_mode == false || _levelID != _levelIDPortalExit) && custom_superfly_state == SUPERFLY_TURNED_ON)
        {
            custom_superfly_state = SUPERFLY_TURNED_OFF;
            //printf("SUPERFLY OFF\n");
            _spyro.canSuperfly = false;
        }
        //Prepare to turn superfly back on, after portal exit ends since it turns it off on it's own
        if (misc_menu.super_mode == true && (_movementSubState == MOVEMENT_SUBSTATE_EXIT_PORTAL || _gameState == GAMESTATE_LOADING || _gameState == GAMESTATE_DEATH) && custom_superfly_state == SUPERFLY_TURNED_ON)
        {
            custom_superfly_state = SUPERFLY_TURNED_OFF;
            //printf("SUPERFLY OFF.\n");
            _spyro.canSuperfly = false;
        }

        //Increase & Decrease height cap
        if (custom_superfly_state == SUPERFLY_TURNED_ON)
        {
            if (_spyro.state == GLIDE)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    _height_cap += 0x1000;
                    //printf("%X\n", _height_cap);
                }
                else if (_currentButton == L2_BUTTON + R2_BUTTON)
                {
                    _height_cap = _spyro.position.z + 0x200;
                    _spyro.neutralJumpZPos = _spyro.position.z + 0x200;
                    //printf("%X\n", _height_cap);
                }
            }
        }

    }

    //! DRAGON TOUCH
    if (misc_menu.show_dragon_touch && _dragonState > 2 && _dragonState < 7)
    {
        CapitalTextInfo timer_text_info = { 0 };
        timer_text_info.x = SCREEN_RIGHT_EDGE - 0x40;
        timer_text_info.y = SCREEN_TOP_EDGE + 0x20;
        timer_text_info.size = DEFAULT_SIZE;
        char temp[4];
        sprintf(temp, "%dF", (_dragonWalkTime - 64) / 2);
        DrawTextCapitals(temp, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_GOLD);

        MyHudMoby dragon_logo = { .position.x = SCREEN_RIGHT_EDGE - 0x54, .position.y = SCREEN_TOP_EDGE + 0x27, .position.z = 3900 };
        CustomDrawMoby(MOBY_ID_DRAGON_FIGURINE, &dragon_logo, MOBY_COLOR_GOLD);
    }

    // if(((timer_menu.timer_display_mode == TIMER_ALWAYS || il_menu.il_timer_display_mode == IL_TIMER_ALWAYS || menu_state == MENU_DISPLAYING) && _gameState == GAMESTATE_GAMEPLAY) || (il_timer_state == IL_DISPLAYING && _gameState == GAMESTATE_LOADING) || ((il_menu.display_on_dragon == TRUE || misc_menu.show_dragon_touch == TRUE) && _gameState == GAMESTATE_DRAGON_STATE))
    // {
    //     //printf("RENDERING\n");
    //     RenderShadedMobyQueue();
    // }
}