#include <common.h>
#include <custom_menu.h>
#include <custom_text.h>
#include <font.h>
#include <sound.h>
#include <shared_funcs.h>
#include <main_updates.h>
#include <cosmetic.h>
#include <igt.h>
#include <custom_types.h>
#include <cd.h>
#include <deckard_strings.h>

//! The custom menu code is split into 2 files, because of size limitations.
//! The first half exists in one codecave, and the second half exists in another.

/* Same row layout as IL_MENU::DrawMenuItem(..., 70) in custom_menu.c */
#define GHOST_MENU_BASE_Y 70
#define GHOST_MENU_NUM_ITEMS 5

int g_ghost_menu_reset_confirm_active;
int g_ghost_menu_reset_yesno; /* 0 = No, 1 = Yes */

// Moved to ghost_menu_globals.c because of le/be deckard nonsense
extern GhostMenu ghost_menu;
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

void GhostResetAll(void);

extern int savestated_level_ids[3];

static const char* GetGhostColorText(int color)
{
    switch (color)
    {
        case GHOST_COLOR_BLUE:
        return GHOST_COLOR_BLUE_TEXT;
        case GHOST_COLOR_GREEN:
        return GHOST_COLOR_GREEN_TEXT;
        case GHOST_COLOR_PURPLE:
        return GHOST_COLOR_PURPLE_TEXT;
        case GHOST_COLOR_GOLD:
        return GHOST_COLOR_GOLD_TEXT;
        case GHOST_COLOR_BLACK:
        return GHOST_COLOR_BLACK_TEXT;
        case GHOST_COLOR_RED:
        default:
        return GHOST_COLOR_RED_TEXT;
    }
}

void ResetGhostsRegion()
{
    savestate_selection = 0; // Force savestate slot to 1
    // Clear savestate slots 2/3 level IDs
    savestated_level_ids[1] = 0;
    savestated_level_ids[2] = 0;

    memset(0x80A27000, 0x0, 0x48FD0);
}

//! Every Frame Update
void CustomMenuUpdateGhosts()
{
    if (menu_state != MENU_DISPLAYING || _gameState != GAMESTATE_GAMEPLAY)
    {
        g_ghost_menu_reset_confirm_active = 0;
        return;
    }

    if (current_menu == GHOST_MENU)
    {
        const int ghost_item_count = GHOST_MENU_NUM_ITEMS;

        if (g_ghost_menu_reset_confirm_active)
        {
            _spyro.isMovementLocked = TRUE;

            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                g_ghost_menu_reset_confirm_active = 0;
                PlayMenuSound();
            }
            else if (_currentButtonOneFrame == LEFT_BUTTON)
            {
                g_ghost_menu_reset_yesno = 0;
                PlayMenuSound();
            }
            else if (_currentButtonOneFrame == RIGHT_BUTTON)
            {
                g_ghost_menu_reset_yesno = 1;
                PlayMenuSound();
            }
            else if (_currentButtonOneFrame == X_BUTTON)
            {
                if (g_ghost_menu_reset_yesno == 1)
                    GhostResetAll();
                g_ghost_menu_reset_confirm_active = 0;
                PlayMenuSound();
            }

            {
                const int ox = 110;
                RGB stroke = { 0x1, 0x1, 0x1 };
                RGB gray = { 0x80, 0x80, 0x80 };
                RGB yellowFlash = { 0xFF, 0xE8, 0x28 };
                int flash = (_globalTimer >> 3) & 1;
                int noSel = (g_ghost_menu_reset_yesno == 0);
                RGB noCol = noSel ? (flash ? yellowFlash : gray) : gray;
                RGB yesCol = !noSel ? (flash ? yellowFlash : gray) : gray;

                DrawTextSimple(deckard_str_ghost_reset_delete_line1, 16 + ox, 132);
                DrawTextSimple(deckard_str_ghost_reset_delete_line2, 16 + ox, 141);
                DrawText(deckard_str_ghost_reset_no, 56 + ox, 158, FONT_STROKE, noCol, stroke);
                DrawText(deckard_str_ghost_reset_yes, 168 + ox, 158, FONT_STROKE, yesCol, stroke);
            }

            RenderShadedMobyQueue(); //why need to run again?
            return;
        }

        if (_currentButtonOneFrame == CIRCLE_BUTTON)
        {
            current_menu = IL_MENU;
            ghost_menu.selection = 0;
            g_ghost_menu_reset_confirm_active = 0;
            g_ghost_menu_reset_yesno = 0;
            PlayMenuSound();
        }

        DrawTextBox(0x30, 0x1D0, 0x30, 0xB0);

        _spyro.isMovementLocked = TRUE;

        if (ghost_menu.ghost_enabled_text == NULL)
        {
            ghost_menu.ghost_enabled_text = GHOSTS_DISABLED_TEXT;
            ghost_menu.ghost_visual_text = VISUAL_SPYRO_TEXT;
            ghost_menu.ghost_color_text = GHOST_COLOR_RED_TEXT;
            ghost_menu.ghost_recording_length_text = SHOW_RECORDING_LENGTH_OFF_TEXT;
            ghost_menu.ghost_reset_text = RESET_ALL_GHOSTS_TEXT;
        }

        if (ghost_menu.ghosts_enabled == false)
        {
            ghost_menu.selection = 0;
        }

        /* 0: enable, 1: visual, 2: color, 3: rec length HUD, 4: reset (X) */
        DrawMenuItem(ghost_menu.ghost_enabled_text, 0, ghost_menu.selection, GHOST_MENU_BASE_Y);
        DrawPossiblyLockedMenuItem(ghost_menu.ghost_visual_text, 1, ghost_menu.selection, ghost_menu.ghosts_enabled, GHOST_MENU_BASE_Y);
        DrawPossiblyLockedMenuItem(ghost_menu.ghost_color_text, 2, ghost_menu.selection, ghost_menu.ghosts_enabled, GHOST_MENU_BASE_Y);
        DrawPossiblyLockedMenuItem(ghost_menu.ghost_recording_length_text, 3, ghost_menu.selection, ghost_menu.ghosts_enabled, GHOST_MENU_BASE_Y);
        DrawPossiblyLockedMenuItem(ghost_menu.ghost_reset_text, 4, ghost_menu.selection, ghost_menu.ghosts_enabled, GHOST_MENU_BASE_Y);



        if (ghost_menu.ghosts_enabled == true)
        {
            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                ghost_menu.selection = (ghost_menu.selection + 1) % ghost_item_count;
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                ghost_menu.selection = (ghost_menu.selection + (ghost_item_count - 1)) % ghost_item_count;
            }
        }

        if (ghost_menu.ghosts_enabled == false)
        {
            ghost_menu.selection = 0;
        }

        if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == X_BUTTON)
        {
            PlayMenuSound();
        }

        if (ghost_menu.ghosts_enabled == 0)
        {
            ghost_menu.ghost_enabled_text = GHOSTS_DISABLED_TEXT;
        }
        else
        {
            ghost_menu.ghost_enabled_text = GHOSTS_ENABLED_TEXT;
        }
        if (ghost_menu.ghosts_visual == VISUAL_SPYRO)
        {
            ghost_menu.ghost_visual_text = VISUAL_SPYRO_TEXT;
        }
        // else if (ghost_menu.ghosts_visual == VISUAL_SPYRO_WIREFRAME)
        // {
        //     ghost_menu.ghost_visual_text = VISUAL_SPYRO_WIREFRAME_TEXT;
        // }
        else
        {
            ghost_menu.ghost_visual_text = VISUAL_POLYGON_TEXT;
        }
        ghost_menu.ghost_color_text = GetGhostColorText(ghost_menu.ghosts_color);
        if (ghost_menu.show_recording_length)
        {
            ghost_menu.ghost_recording_length_text = SHOW_RECORDING_LENGTH_ON_TEXT;
        }
        else
        {
            ghost_menu.ghost_recording_length_text = SHOW_RECORDING_LENGTH_OFF_TEXT;
        }

        if (ghost_menu.selection == 0)
        {
            if (_currentButtonOneFrame == RIGHT_BUTTON && !ghost_menu.ghosts_enabled)
            {
                ghost_menu.ghosts_enabled = true;

                savestate_selection = 0;
                // Clear savestate slots 2/3 level IDs
                savestated_level_ids[1] = 0;
                savestated_level_ids[2] = 0;
            }
            else if (_currentButtonOneFrame == LEFT_BUTTON && ghost_menu.ghosts_enabled)
            {
                ghost_menu.ghosts_enabled = false;
                savestate_selection = 0;
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
        if (ghost_menu.selection == 2)
        {
            if (_currentButtonOneFrame == RIGHT_BUTTON)
            {
                ghost_menu.ghosts_color = (ghost_menu.ghosts_color + 1) % GHOST_COLOR_COUNT;
            }
            else if (_currentButtonOneFrame == LEFT_BUTTON)
            {
                ghost_menu.ghosts_color = (ghost_menu.ghosts_color + (GHOST_COLOR_COUNT - 1)) % GHOST_COLOR_COUNT;
            }
        }
        if (ghost_menu.selection == 3)
        {
            if (_currentButtonOneFrame == RIGHT_BUTTON)
            {
                ghost_menu.show_recording_length = true;
            }
            else if (_currentButtonOneFrame == LEFT_BUTTON)
            {
                ghost_menu.show_recording_length = false;
            }
        }
        if (ghost_menu.selection == 4 && ghost_menu.ghosts_enabled == true && _currentButtonOneFrame == X_BUTTON)
        {
            g_ghost_menu_reset_confirm_active = 1;
            g_ghost_menu_reset_yesno = 0;
        }
        RenderShadedMobyQueue(); //why need to run again?
    }
}
