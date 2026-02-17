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
#include <hsl.h>

// Moved to skin_editor_menu_globals.c because of le/be deckard nonsense
extern SkinEditorMenu skin_editor_menu;
extern char hue_text_buf[10];
extern char saturation_text_buf[16];
extern char value_text_buf[11];
extern int right_press_timer;
extern int left_press_timer;
extern const char* hue_text_str;
extern const char* sat_text_str;
extern const char* light_text_str;
extern const char* section_strings[7];
extern const char* save_str;


extern Menu custom_menu;
extern ILMenu il_menu;
extern TimerMenu timer_menu;
extern SavestateMenu savestate_menu;
extern MiscMenu misc_menu;
extern ILDisplayMenu il_display_modes;

extern CosmeticMenu cosmetic_menu;


// Externing elsewhere
extern bool has_toggled_menu;

extern MenuState menu_state;
extern int current_menu;

extern int savestate_selection;

extern bool isMenuButtonHeld;

extern BackgroundColor bg_color_index;
extern SpyroColor spyro_color_index;
extern FlameColor flame_color_index;
extern SparxColor sparx_color_index;
extern bool should_update_bg_color;
extern bool should_load_spyro_color;
extern bool should_load_flame_color;
extern bool should_load_sparx_color;


extern int oldCdLocation[2];    // Array of 2 ints because the seek location takes up 8 bytes

extern int body_pixels;
extern int belly_pixels;
extern int wing_pixels;
extern int wingbone_pixels;
extern int horn_pixels;
extern int crest_pixels;
extern int eyes_pixels;

extern HSL hslBodyPartData[7];


//! Every Frame Update
void CustomMenuUpdateSkinEditor()
{
    if (menu_state == MENU_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
    {

        if (current_menu == SKIN_EDITOR_MENU)
        {

            CapitalTextInfo menu_text_info[5] = { {0} };

            // Easy Exit
            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                current_menu = COSMETIC_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
                StopDrawWorldAndObjects();
            }

            //DrawTextBox(0x30, 0x1D0, 0x30, 0xBC);

            menu_text_info[0].x = SCREEN_LEFT_EDGE + 0x1A;
            menu_text_info[0].y = 30;
            menu_text_info[0].size = DEFAULT_SIZE;

            menu_text_info[1].x = SCREEN_LEFT_EDGE + 0x1A;
            menu_text_info[1].y = 50;
            menu_text_info[1].size = DEFAULT_SIZE;

            menu_text_info[2].x = SCREEN_LEFT_EDGE + 0x1A;
            menu_text_info[2].y = 70;
            menu_text_info[2].size = DEFAULT_SIZE;

            menu_text_info[3].x = SCREEN_LEFT_EDGE + 0x1A;
            menu_text_info[3].y = 90;
            menu_text_info[3].size = DEFAULT_SIZE;

            menu_text_info[4].x = SCREEN_LEFT_EDGE + 0x1A;
            menu_text_info[4].y = 110;
            menu_text_info[4].size = DEFAULT_SIZE;

            _spyro.isMovementLocked = TRUE;

            if (skin_editor_menu.selection == 0)
            {
                DrawTextCapitals(skin_editor_menu.skin_section_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(skin_editor_menu.skin_section_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (skin_editor_menu.selection == 1)
            {
                DrawTextCapitals(skin_editor_menu.hue_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(skin_editor_menu.hue_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (skin_editor_menu.selection == 2)
            {
                DrawTextCapitals(skin_editor_menu.saturation_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(skin_editor_menu.saturation_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }


            if (skin_editor_menu.selection == 3)
            {
                DrawTextCapitals(skin_editor_menu.value_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(skin_editor_menu.value_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (skin_editor_menu.selection == 4)
            {
                DrawTextCapitals(skin_editor_menu.save_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(skin_editor_menu.save_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            // Fill text with defaults if NULL
            if (skin_editor_menu.skin_section_text == NULL)
            {
                skin_editor_menu.skin_section_text = "SECTION BODY";
                skin_editor_menu.hue_text = "HUE 0";
                skin_editor_menu.saturation_text = "SATURATION 0";
                skin_editor_menu.value_text = "LIGHTNESS 0";
                skin_editor_menu.save_text = save_str;
            }

            // Change Selection
            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                skin_editor_menu.selection = (skin_editor_menu.selection + 1) % 5;
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                skin_editor_menu.selection = (skin_editor_menu.selection + 4) % 5;                    // +5 because it's the same as -1 in mod 6 math
            }
            // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Update Text
            if (skin_editor_menu.skin_section == SECTION_BODY)
            {
                skin_editor_menu.skin_section_text = section_strings[skin_editor_menu.skin_section];
            }
            else if (skin_editor_menu.skin_section == SECTION_BELLY)
            {
                skin_editor_menu.skin_section_text = section_strings[skin_editor_menu.skin_section];
            }
            else if (skin_editor_menu.skin_section == SECTION_WING)
            {
                skin_editor_menu.skin_section_text = section_strings[skin_editor_menu.skin_section];
            }
            else if (skin_editor_menu.skin_section == SECTION_WINGBONE)
            {
                skin_editor_menu.skin_section_text = section_strings[skin_editor_menu.skin_section];
            }
            else if (skin_editor_menu.skin_section == SECTION_HORNS)
            {
                skin_editor_menu.skin_section_text = section_strings[skin_editor_menu.skin_section];
            }
            else if (skin_editor_menu.skin_section == SECTION_CREST)
            {
                skin_editor_menu.skin_section_text = section_strings[skin_editor_menu.skin_section];
            }
            else if (skin_editor_menu.skin_section == SECTION_EYES)
            {
                skin_editor_menu.skin_section_text = section_strings[skin_editor_menu.skin_section];
            }

            // Edit HSV values
            sprintf(hue_text_buf, hue_text_str, skin_editor_menu.hue_level);
            skin_editor_menu.hue_text = hue_text_buf;

            sprintf(saturation_text_buf, sat_text_str, skin_editor_menu.saturation_level);
            skin_editor_menu.saturation_text = saturation_text_buf;

            sprintf(value_text_buf, light_text_str, skin_editor_menu.lightness_level);
            skin_editor_menu.value_text = value_text_buf;


            // Hold down button
            if (_currentButton == RIGHT_BUTTON)
            {
                right_press_timer++;
            }
            else
            {
                right_press_timer = 0;
            }
            if (_currentButton == LEFT_BUTTON)
            {
                left_press_timer++;
            }
            else
            {
                left_press_timer = 0;
            }

            // Change Selections
            if (skin_editor_menu.selection == 0)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    skin_editor_menu.skin_section = (skin_editor_menu.skin_section + 1) % 7;
                    // Update hsl on body part change
                    skin_editor_menu.hue_level = hslBodyPartData[skin_editor_menu.skin_section].h;
                    skin_editor_menu.saturation_level = hslBodyPartData[skin_editor_menu.skin_section].s;
                    skin_editor_menu.lightness_level = hslBodyPartData[skin_editor_menu.skin_section].l;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    skin_editor_menu.skin_section = (skin_editor_menu.skin_section + (7 - 1)) % 7;
                    // Update hsl on body part change
                    skin_editor_menu.hue_level = hslBodyPartData[skin_editor_menu.skin_section].h;
                    skin_editor_menu.saturation_level = hslBodyPartData[skin_editor_menu.skin_section].s;
                    skin_editor_menu.lightness_level = hslBodyPartData[skin_editor_menu.skin_section].l;
                }
            }


            else if (skin_editor_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    skin_editor_menu.hue_level = (skin_editor_menu.hue_level + 1) % 360;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    skin_editor_menu.hue_level = (skin_editor_menu.hue_level + (360 - 1)) % 360;
                }
                //Has Held
                if (right_press_timer > 10)
                {
                    skin_editor_menu.hue_level = (skin_editor_menu.hue_level + 3) % 360;
                }
                else if (left_press_timer > 10)
                {
                    skin_editor_menu.hue_level = (skin_editor_menu.hue_level + (360 - 3)) % 360;
                }
            }
            else if (skin_editor_menu.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON && skin_editor_menu.saturation_level < 100)
                {
                    skin_editor_menu.saturation_level = skin_editor_menu.saturation_level + 1;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && skin_editor_menu.saturation_level > -100)
                {
                    skin_editor_menu.saturation_level = skin_editor_menu.saturation_level - 1;
                }
                //Has Held
                if (right_press_timer > 10 && skin_editor_menu.saturation_level < 100)
                {
                    skin_editor_menu.saturation_level = skin_editor_menu.saturation_level + 1;
                }
                else if (left_press_timer > 10 && skin_editor_menu.saturation_level > -100)
                {
                    skin_editor_menu.saturation_level = skin_editor_menu.saturation_level - 1;
                }
            }
            else if (skin_editor_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON && skin_editor_menu.lightness_level < 100)
                {
                    skin_editor_menu.lightness_level = skin_editor_menu.lightness_level + 1;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && skin_editor_menu.lightness_level > -100)
                {
                    skin_editor_menu.lightness_level = skin_editor_menu.lightness_level - 1;
                }
                //Has Held
                if (right_press_timer > 10 && skin_editor_menu.lightness_level < 100)
                {
                    skin_editor_menu.lightness_level = skin_editor_menu.lightness_level + 1;
                }
                else if (left_press_timer > 10 && skin_editor_menu.lightness_level > -100)
                {
                    skin_editor_menu.lightness_level = skin_editor_menu.lightness_level - 1;
                }
            }


            RenderShadedMobyQueue(); //why need to run again?

            if (skin_editor_menu.skin_section == SECTION_BODY)
            {
                skin_editor_menu.index_ptr = &body_pixels;
                skin_editor_menu.index_array_size = 100;
            }
            else if (skin_editor_menu.skin_section == SECTION_BELLY)
            {
                skin_editor_menu.index_ptr = &belly_pixels;
                skin_editor_menu.index_array_size = 43;
            }
            else if (skin_editor_menu.skin_section == SECTION_WING)
            {
                skin_editor_menu.index_ptr = &wing_pixels;
                skin_editor_menu.index_array_size = 22;
            }
            else if (skin_editor_menu.skin_section == SECTION_WINGBONE)
            {
                skin_editor_menu.index_ptr = &wingbone_pixels;
                skin_editor_menu.index_array_size = 32;
            }
            else if (skin_editor_menu.skin_section == SECTION_HORNS)
            {
                skin_editor_menu.index_ptr = &horn_pixels;
                skin_editor_menu.index_array_size = 15;
            }
            else if (skin_editor_menu.skin_section == SECTION_CREST)
            {
                skin_editor_menu.index_ptr = &crest_pixels;
                skin_editor_menu.index_array_size = 15;
            }
            else if (skin_editor_menu.skin_section == SECTION_EYES)
            {
                skin_editor_menu.index_ptr = &eyes_pixels;
                skin_editor_menu.index_array_size = 15;
            }
        }
    }
}