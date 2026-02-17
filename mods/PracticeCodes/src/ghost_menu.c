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

//! The custom menu code is split into 2 files, because of size limitations.
//! The first half exists in one codecave, and the second half exists in another.

// Moved to ghost_menu_globals.c because of le/be deckard nonsense
extern GhostMenu ghost_menu;
extern const char* GHOSTS_DISABLED_TEXT;
extern const char* GHOSTS_ENABLED_TEXT;
extern const char* VISUAL_SPYRO_TEXT;
extern const char* VISUAL_POLYGON_TEXT;

extern Menu custom_menu;
extern ILMenu il_menu;
extern TimerMenu timer_menu;
extern SavestateMenu savestate_menu;
extern MiscMenu misc_menu;
extern ILDisplayMenu il_display_modes;

extern CosmeticMenu cosmetic_menu;

extern bool has_toggled_menu;

extern MenuState menu_state;
extern int current_menu;

extern int savestate_selection;

extern bool isMenuButtonHeld;

//! Every Frame Update
void CustomMenuUpdateGhosts()
{
    if (menu_state == MENU_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
    {

        if (current_menu == GHOST_MENU)
        {
            CapitalTextInfo menu_text_info[2] = { {0} };

            // Easy Exit
            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                current_menu = IL_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            DrawTextBox(0x30, 0x1D0, 0x30, 0x90);

            menu_text_info[0].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[0].y = 70;
            menu_text_info[0].size = DEFAULT_SIZE;

            menu_text_info[1].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[1].y = 90;
            menu_text_info[1].size = DEFAULT_SIZE;

            _spyro.isMovementLocked = TRUE;

            if (ghost_menu.selection == 0)
            {
                DrawTextCapitals(ghost_menu.ghost_enabled_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(ghost_menu.ghost_enabled_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if (ghost_menu.selection == 1 && ghost_menu.ghosts_enabled == true)
            {
                DrawTextCapitals(ghost_menu.ghost_visual_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else if (ghost_menu.selection == 0 && ghost_menu.ghosts_enabled == true)
            {
                DrawTextCapitals(ghost_menu.ghost_visual_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            else
            {
                DrawTextCapitals(ghost_menu.ghost_visual_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }

            // Fill text with defaults if NULL
            if (ghost_menu.ghost_enabled_text == NULL)
            {
                ghost_menu.ghost_enabled_text = GHOSTS_DISABLED_TEXT;
                ghost_menu.ghost_visual_text = VISUAL_SPYRO_TEXT;
            }

            if (ghost_menu.ghosts_enabled == true)
            {
                if (_currentButtonOneFrame == DOWN_BUTTON)
                {
                    ghost_menu.selection = (ghost_menu.selection + 1) % 2;
                }
                else if (_currentButtonOneFrame == UP_BUTTON)
                {
                    ghost_menu.selection = (ghost_menu.selection + 1) % 2;                    // +5 because it's the same as -1 in mod 6 math
                }
            }

                // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Update Text
            if (ghost_menu.ghosts_enabled == 0)
            {
                ghost_menu.ghost_enabled_text = GHOSTS_DISABLED_TEXT;
            }
            else
            {
                ghost_menu.ghost_enabled_text = GHOSTS_ENABLED_TEXT;
            }
            // Update Text
            if (ghost_menu.ghosts_visual == VISUAL_SPYRO)
            {
                ghost_menu.ghost_visual_text = VISUAL_SPYRO_TEXT;
            }
            else
            {
                ghost_menu.ghost_visual_text = VISUAL_POLYGON_TEXT;
            }

            // Update Menu Options
            if (ghost_menu.selection == 0)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    ghost_menu.ghosts_enabled = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    ghost_menu.ghosts_enabled = false;
                }
            }
            if (ghost_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    ghost_menu.ghosts_visual = VISUAL_POLYGON;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    ghost_menu.ghosts_visual = VISUAL_SPYRO;
                }
            }
            RenderShadedMobyQueue(); //why need to run again?
        }
    }
}


