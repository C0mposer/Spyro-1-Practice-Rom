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
#include <deckard_strings.h>

SkinEditorMenu skin_editor_menu = { 0 };

char hue_text_buf[20] = { 0 };
char saturation_text_buf[20] = { 0 };
char value_text_buf[20] = { 0 };

const char* hue_text_str = deckard_str_skin_hue_fmt;
const char* sat_text_str = deckard_str_skin_sat_fmt;
const char* light_text_str = deckard_str_skin_light_fmt;
const char* section_strings[7] = {
    deckard_str_skin_sec_body,
    deckard_str_skin_sec_belly,
    deckard_str_skin_sec_wing,
    deckard_str_skin_sec_wingbone,
    deckard_str_skin_sec_horns,
    deckard_str_skin_sec_crest,
    deckard_str_skin_sec_eyes,
};
const char* save_str = deckard_str_skin_save;

int right_press_timer = 0;
int left_press_timer = 0;


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

extern const int body_pixels[];
extern const int belly_pixels[];
extern const int wing_pixels[];
extern const int wingbone_pixels[];
extern const int horn_pixels[];
extern const int crest_pixels[];
extern const int eyes_pixels[];

extern HSL hslBodyPartData[7];


//! Every Frame Update
void CustomMenuUpdateSkinEditor()
{
    if (menu_state == MENU_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
    {

        if (current_menu == SKIN_EDITOR_MENU)
        {
            StopCameraFollowSpyro();
            CosmeticMenuSpyroLockForPreviewTick();

            CosmeticMenuSetPreviewMode(COSMETIC_PREVIEW_SPYRO);
            CosmeticMenuDrawSpyroPreview();

            // Easy Exit
            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                skin_editor_menu.selection = 0;
                current_menu = COSMETIC_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
                StopDrawWorldAndObjects();
            }

            _spyro.isMovementLocked = TRUE;

            if (skin_editor_menu.selection == 0)
            {
                DrawTextSelected(skin_editor_menu.skin_section_text, SCREEN_LEFT_EDGE + 0x1A, 30, false);
            }
            else
            {
                DrawTextSelected(skin_editor_menu.skin_section_text, SCREEN_LEFT_EDGE + 0x1A, 30, true);
            }

            if (skin_editor_menu.selection == 1)
            {
                DrawTextSelected(skin_editor_menu.hue_text, SCREEN_LEFT_EDGE + 0x1A, 40, false);
            }
            else
            {
                DrawTextSelected(skin_editor_menu.hue_text, SCREEN_LEFT_EDGE + 0x1A, 40, true);
            }

            if (skin_editor_menu.selection == 2)
            {
                DrawTextSelected(skin_editor_menu.saturation_text, SCREEN_LEFT_EDGE + 0x1A, 50, false);
            }
            else
            {
                DrawTextSelected(skin_editor_menu.saturation_text, SCREEN_LEFT_EDGE + 0x1A, 50, true);
            }


            if (skin_editor_menu.selection == 3)
            {
                DrawTextSelected(skin_editor_menu.value_text, SCREEN_LEFT_EDGE + 0x1A, 60, false);
            }
            else
            {
                DrawTextSelected(skin_editor_menu.value_text, SCREEN_LEFT_EDGE + 0x1A, 60, true);
            }

            if (skin_editor_menu.selection == 4)
            {
                DrawTextSelected(skin_editor_menu.save_text, SCREEN_LEFT_EDGE + 0x1A, 70, false);
            }
            else
            {
                DrawTextSelected(skin_editor_menu.save_text, SCREEN_LEFT_EDGE + 0x1A, 70, true);
            }

            // Fill text with defaults if NULL
            if (skin_editor_menu.skin_section_text == NULL)
            {
                skin_editor_menu.skin_section_text = deckard_str_skin_default_section;
                skin_editor_menu.hue_text = deckard_str_skin_default_hue;
                skin_editor_menu.saturation_text = deckard_str_skin_default_sat;
                skin_editor_menu.value_text = deckard_str_skin_default_light;
                skin_editor_menu.save_text = save_str;
            }

            // Update Text
            skin_editor_menu.skin_section_text = section_strings[skin_editor_menu.skin_section];

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
            else if (skin_editor_menu.selection == 4)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    skin_editor_menu.selection = 0;
                    current_menu = COSMETIC_MENU;
                }
            }


            //RenderShadedMobyQueue(); // Why need to run again? Who knows!

            if (skin_editor_menu.skin_section == SECTION_BODY)
            {
                skin_editor_menu.index_ptr = (int*)(void*)body_pixels;
                skin_editor_menu.index_array_size = 100;
            }
            else if (skin_editor_menu.skin_section == SECTION_BELLY)
            {
                skin_editor_menu.index_ptr = &belly_pixels;
                skin_editor_menu.index_array_size = 43;
            }
            else if (skin_editor_menu.skin_section == SECTION_WING)
            {
                skin_editor_menu.index_ptr = (int*)(void*)wing_pixels;
                skin_editor_menu.index_array_size = 22;
            }
            else if (skin_editor_menu.skin_section == SECTION_WINGBONE)
            {
                skin_editor_menu.index_ptr = &wingbone_pixels;
                skin_editor_menu.index_array_size = 32;
            }
            else if (skin_editor_menu.skin_section == SECTION_HORNS)
            {
                skin_editor_menu.index_ptr = (int*)(void*)horn_pixels;
                skin_editor_menu.index_array_size = 15;
            }
            else if (skin_editor_menu.skin_section == SECTION_CREST)
            {
                skin_editor_menu.index_ptr = (int*)(void*)crest_pixels;
                skin_editor_menu.index_array_size = 15;
            }
            else if (skin_editor_menu.skin_section == SECTION_EYES)
            {
                skin_editor_menu.index_ptr = (int*)(void*)eyes_pixels;
                skin_editor_menu.index_array_size = 15;
            }
        }
    }
}