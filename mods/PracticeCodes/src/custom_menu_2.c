#include <common.h>
#include <custom_menu.h>
#include <custom_text.h>
#include <sound.h>
#include <shared_funcs.h>
#include <main_updates.h>
#include <cosmetic.h>
#include <igt.h>
#include <custom_types.h>
#include <cd.h>
#include <skin_strings.h>

//! The custom menu code is split into 2 files, because of size limitations.
//! The first half exists in one codecave, and the second half exists in another.



extern Menu custom_menu;
extern ILMenu il_menu;
extern TimerMenu timer_menu;
extern SavestateMenu savestate_menu;
extern MiscMenu misc_menu;
extern ILDisplayMenu il_display_modes;

CosmeticMenu cosmetic_menu = { 0 };

bool consistency_tracker_mode; // removed from struct
bool disable_portal_entry;     // removed from struct
bool show_sparx_range_mode;
bool has_savestated_on_disabling_portal = false;

// Externing elsewhere
extern bool has_toggled_menu;

extern MenuState menu_state;
extern int current_menu;

extern int savestate_selection;

extern bool isMenuButtonHeld;

extern const short STOP_TIMER_BUTTONS[1];
extern const short RESET_TIMER_BUTTONS[2];

extern const short SAVESTATE_BUTTONS[2];
extern const short LOADSTATE_BUTTONS[3];

extern int savestate_button_index;
extern int loadstate_button_index;
extern int switch_state_button_index;

extern int consistency_tracker_render_time;

BackgroundColor bg_color_index;
SpyroColor spyro_color_index;
FlameColor flame_color_index;
SparxColor sparx_color_index;
bool should_update_bg_color = true;
bool should_load_spyro_color = false;
bool should_load_flame_color = false;
bool should_load_sparx_color = false;

extern TimerState timerState;

typedef enum ILTimerState
{
    IL_FLYING_IN,
    IL_STARTED,
    IL_DISPLAYING,
    IL_STOPPED
} ILTimerState;
extern ILTimerState il_timer_state;
extern int mainTimerAtReset;

extern int oldCdLocation[2];    // Array of 2 ints because the seek location takes up 8 bytes

//! Every Frame Update
void CustomMenuUpdate2()
{
    if (menu_state == MENU_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
    {

        if (current_menu == MISC_MENU)
        {
            _spyro.isMovementLocked = TRUE;

            CheckBackMenu();

            DrawTextBox(0x30, 0x1D0, 0x30, 0xBC);

            DrawMenuItem(misc_menu.sparx_mode_text, 0, misc_menu.selection, 70);
            DrawMenuItem(misc_menu.show_dragon_touch_text, 1, misc_menu.selection, 70);
            DrawMenuItem(misc_menu.disable_portal_entry_text, 2, misc_menu.selection, 70);
            DrawMenuItem(misc_menu.consitency_tracker_text, 3, misc_menu.selection, 70);
            DrawMenuItem(misc_menu.show_sparx_range_text, 4, misc_menu.selection, 70);
            DrawMenuItem(misc_menu.super_text, 5, misc_menu.selection, 70);

            // Fill text with defaults if NULL
            if (misc_menu.sparx_mode_text == NULL)
            {
                misc_menu.sparx_mode_text = "SPARX MODE NORMAL";
                misc_menu.show_dragon_touch_text = "SHOW DRAGON TOUCH OFF";
                misc_menu.disable_portal_entry_text = "DISABLE PORTAL OFF";
                misc_menu.consitency_tracker_text = "TRACK CONSISTENCY OFF";
                misc_menu.show_sparx_range_text = "SHOW SPARX RANGE OFF";
                misc_menu.super_text = "SUPER MODE OFF";
            }

            // Change Selection
            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                misc_menu.selection = (misc_menu.selection + 1) % 6;
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                misc_menu.selection = (misc_menu.selection + 5) % 6;                    // +5 because it's the same as -1 in mod 6 math
            }

            // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Update Text
            if (misc_menu.sparx_mode == 0)
            {
                misc_menu.sparx_mode_text = "SPARX NORMAL";
            }
            else if (misc_menu.sparx_mode == 1)
            {
                misc_menu.sparx_mode_text = "SPARXLESS";
            }
            else
            {
                if (il_menu.il_state == OFF)
                    misc_menu.sparx_mode_text = "PERMA SPARX";
                else
                    misc_menu.sparx_mode_text = "SPARX NORMAL";
            }


            if (misc_menu.show_dragon_touch == false)
            {
                misc_menu.show_dragon_touch_text = "SHOW DRAGON TOUCH OFF";
            }
            else
            {
                misc_menu.show_dragon_touch_text = "SHOW DRAGON TOUCH ON";
            }

            if (disable_portal_entry == false)
            {
                misc_menu.disable_portal_entry_text = "DISABLE PORTAL OFF";
            }
            else
            {
                misc_menu.disable_portal_entry_text = "DISABLE PORTAL ON";
            }

            if (consistency_tracker_mode == false)
            {
                misc_menu.consitency_tracker_text = "TRACK CONSISTENCY OFF";
            }
            else if (consistency_tracker_mode == true)
            {
                misc_menu.consitency_tracker_text = "TRACK CONSISTENCY ON";
            }

            if (show_sparx_range_mode == false)
            {
                misc_menu.show_sparx_range_text = "SHOW SPARX RANGE OFF";
            }
            else if (show_sparx_range_mode == true)
            {
                misc_menu.show_sparx_range_text = "SHOW SPARX RANGE ON";
            }

            if (misc_menu.super_mode == false)
            {
                misc_menu.super_text = "SUPER MODE OFF";
            }
            else
            {
                misc_menu.super_text = "SUPER MODE ON";
            }

            // Update Menu Options
            if (misc_menu.selection == 0)
            {
                int num_items = 0;
                if (il_menu.il_state == OFF)
                {
                    num_items = 3;
                }
                if (il_menu.il_state == ON)
                {

                    num_items = 2;
                }

                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    misc_menu.sparx_mode = (misc_menu.sparx_mode + 1) % num_items;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    misc_menu.sparx_mode = (misc_menu.sparx_mode + (num_items - 1)) % num_items;
                }
            }

            else if (misc_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    misc_menu.show_dragon_touch = (misc_menu.show_dragon_touch + 1) % 2;
                }
            }

            else if (misc_menu.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    disable_portal_entry = (disable_portal_entry + 1) % 2;
                }
            }

            if (misc_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    consistency_tracker_mode = (consistency_tracker_mode + 1) % 2;

                    if (consistency_tracker_mode)
                    {
                        consistency_tracker_render_time = 30; // Render for 1 second when you enable it
                    }
                }
            }

            if (misc_menu.selection == 4)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    show_sparx_range_mode = (show_sparx_range_mode + 1) % 2;
                }
            }

            else if (misc_menu.selection == 5)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    misc_menu.super_mode = (misc_menu.super_mode + 1) % 2;
                }
            }

        }

        if (current_menu == COSMETIC_MENU)
        {
            _spyro.isMovementLocked = TRUE;

            CheckBackMenu();

            DrawTextBox(0x30, 0x1D0, 0x30, 0xA4);

            DrawMenuItem(cosmetic_menu.bg_color_text, 0, cosmetic_menu.selection, 70);
            DrawMenuItem(cosmetic_menu.spyro_color_text, 1, cosmetic_menu.selection, 70);
            DrawMenuItem(cosmetic_menu.flame_color_text, 2, cosmetic_menu.selection, 70);
            DrawMenuItem(cosmetic_menu.sparx_color_text, 3, cosmetic_menu.selection, 70);
            DrawMenuItem(cosmetic_menu.skin_editor_menu_text, 4, cosmetic_menu.selection, 70);

            // Fill text with defaults if NULL
            if (cosmetic_menu.bg_color_text == NULL)
            {
                cosmetic_menu.bg_color_text = "BG BLUE";
                cosmetic_menu.spyro_color_text = "SPYRO SKIN DEFAULT";
                cosmetic_menu.flame_color_text = "FLAME SKIN DEFAULT";
                cosmetic_menu.sparx_color_text = "SPARX SKIN DEFAULT";
                cosmetic_menu.skin_editor_menu_text = "SKIN EDITOR";
            }

            // Change Selection
            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                cosmetic_menu.selection = (cosmetic_menu.selection + 1) % 5;
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                cosmetic_menu.selection = (cosmetic_menu.selection + 4) % 5;                    // +3 because it's the same as -1 in mod 4 math
            }

            // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            if (cosmetic_menu.selection == 0)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    bg_color_index = (bg_color_index + 1) % 7;
                    should_update_bg_color = TRUE;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    bg_color_index = (bg_color_index + 6) % 7;
                    should_update_bg_color = TRUE;
                }

                // Update the string (Look up table instead of switch)
                cosmetic_menu.bg_color_text = BG_COLOR_STRING_LUT[bg_color_index];
            }

            // Spyro Color
            else if (cosmetic_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    spyro_color_index = (spyro_color_index + 1) % 18;
                    should_load_spyro_color = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    spyro_color_index = (spyro_color_index + 17) % 18;
                    should_load_spyro_color = true;
                }

                // Update the string (Look up table instead of switch)
                cosmetic_menu.spyro_color_text = SPYRO_SKIN_STRING_LUT[spyro_color_index];

            }
            else if (cosmetic_menu.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    flame_color_index = (flame_color_index + 1) % 15;
                    should_load_flame_color = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    flame_color_index = (flame_color_index + 14) % 15;
                    should_load_flame_color = true;
                }

                // Update the string (Look up table instead of switch)
                cosmetic_menu.flame_color_text = FLAME_SKIN_STRING_LUT[flame_color_index];
            }

            else if (cosmetic_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    sparx_color_index = (sparx_color_index + 1) % 11;
                    should_load_sparx_color = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    sparx_color_index = (sparx_color_index + 10) % 11;
                    should_load_sparx_color = true;
                }

                cosmetic_menu.sparx_color_text = SPARX_SKIN_STRING_LUT[sparx_color_index];
            }

            else if (cosmetic_menu.selection == 4)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = SKIN_EDITOR_MENU;
                    cosmetic_menu.selection = 0;
                    RestartDrawWorldAndObjects();
                }
            }

        }
    }

    if (((timer_menu.timer_display_mode == TIMER_ALWAYS || il_display_modes.il_display_always == true || timerState == TIMER_DISPLAYING || menu_state == MENU_DISPLAYING || ShouldDisplayMiscTime()) && _gameState == GAMESTATE_GAMEPLAY) || (il_timer_state == IL_DISPLAYING) || ((il_display_modes.il_display_dragon == TRUE || misc_menu.show_dragon_touch == TRUE) && _gameState == GAMESTATE_DRAGON_STATE))
    {
        // printf("RENDERING\n");
        RenderShadedMobyQueue();
    }
}


void StopDrawWorldAndObjects(void)
{
    // Function addresses
    int* draw_world_func = 0x8002b9cc;
    int* draw_objects_func = 0x80019698;
    int* draw_particles_func = 0x800573c8;

    *draw_world_func = JR_RA;
    *(draw_world_func + 1) = NOP;

    *draw_objects_func = JR_RA;
    *(draw_objects_func + 1) = NOP;

    *draw_particles_func = JR_RA;
    *(draw_particles_func + 1) = NOP;
}
void RestartDrawWorldAndObjects(void)
{
    // Function addresses
    int* draw_world_func = 0x8002b9cc;
    int* draw_objects_func = 0x80019698;
    int* draw_particles_func = 0x800573c8;

    // Original Opcodes
    *draw_world_func = 0x27BDFFE8;
    *(draw_world_func + 1) = 0x3C048007;

    *draw_objects_func = 0x27BDFFE8;
    *(draw_objects_func + 1) = 0xAFBF0010;

    *draw_particles_func = 0x3C011F80;
    *(draw_particles_func + 1) = 0xAC300000;
}

void TurnOnDefaultSettings()
{
    il_menu.il_state = true;
    il_display_modes.il_display_dragon = true;

    misc_menu.show_dragon_touch = true;

    //timer_menu.timer_state = true;
}
void TurnOffDefaultSettings()
{
    il_menu.il_state = false;
    il_display_modes.il_display_dragon = false;

    misc_menu.show_dragon_touch = false;

    //timer_menu.timer_state = false;
}
