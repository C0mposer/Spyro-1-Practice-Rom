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
extern VisualizerMenu visualizer_menu;
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

GhostMenu ghost_menu = { 0 }; // This needs to live somewhere in main ram, so here it lives! 

enum
{
    DRAW_WORLD_STOP_WORLD_ONLY = 1,
    DRAW_WORLD_STOP_FULL = 2
};

static int s_draw_world_stop_reasons;

static void ApplyDrawWorldPatchState(void)
{
    static const int kDrawWorldOpcodes[2] = { 0x27BDFFE8, 0x3C048007 };

    if (s_draw_world_stop_reasons != 0)
        FunctionCallToggleApply((void*)0x8002b9cc, kDrawWorldOpcodes);
    else
        FunctionCallToggleRestore((void*)0x8002b9cc, kDrawWorldOpcodes);
}

//! Every Frame Update
void CustomMenuUpdate2()
{
    if (menu_state == MENU_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
    {
        if (current_menu != COSMETIC_MENU && current_menu != SKIN_EDITOR_MENU)
            CosmeticMenuSpyroAngleRestoreIfNeeded();

        if (current_menu == MISC_MENU)
        {
            int misc_item_count = 5;
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            misc_item_count = 6;
            #endif

            _spyro.isMovementLocked = TRUE;

            CheckBackMenu();

            int misc_menu_base_y = 48;
            int misc_menu_box_bottom = 0x92;
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            misc_menu_box_bottom = 0xA8;
            #endif
            DrawTextBox(0x30, 0x1D0, 0x1A, misc_menu_box_bottom);

            DrawMenuItem(misc_menu.sparx_mode_text, 0, misc_menu.selection, misc_menu_base_y);
            DrawMenuItem(misc_menu.show_dragon_touch_text, 1, misc_menu.selection, misc_menu_base_y);
            DrawMenuItem(misc_menu.disable_portal_entry_text, 2, misc_menu.selection, misc_menu_base_y);
            DrawMenuItem(misc_menu.consitency_tracker_text, 3, misc_menu.selection, misc_menu_base_y);
            DrawMenuItem(misc_menu.super_text, 4, misc_menu.selection, misc_menu_base_y);
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            DrawMenuItem(misc_menu.memory_watch_text, 5, misc_menu.selection, misc_menu_base_y);
            //DrawMenuItem(visualizer_menu.collision_wireframe_text, 6, misc_menu.selection, misc_menu_base_y);
            #endif

            // Fill text with defaults if NULL
            if (misc_menu.sparx_mode_text == NULL)
            {
                misc_menu.sparx_mode_text = "SPARX MODE NORMAL";
                misc_menu.show_dragon_touch_text = "SHOW DRAGON TOUCH OFF";
                misc_menu.disable_portal_entry_text = "DISABLE PORTAL OFF";
                misc_menu.consitency_tracker_text = "TRACK CONSISTENCY OFF";
                misc_menu.super_text = "SUPER MODE OFF";
                misc_menu.memory_watch_text = "MEMORY WATCH";
                //visualizer_menu.collision_wireframe_text = "COLLISION WIREFRAME OFF";
            }

            // Change Selection
            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                misc_menu.selection = (misc_menu.selection + 1) % misc_item_count;
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                misc_menu.selection = (misc_menu.selection + misc_item_count - 1) % misc_item_count;
            }

            // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlayMenuSound();
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


            misc_menu.show_dragon_touch_text = ToggleMenuBool(&misc_menu.show_dragon_touch, misc_menu.selection == 1, "SHOW DRAGON TOUCH ON", "SHOW DRAGON TOUCH OFF");
            misc_menu.disable_portal_entry_text = ToggleMenuBool(&disable_portal_entry, misc_menu.selection == 2, "DISABLE PORTAL ON", "DISABLE PORTAL OFF");
            misc_menu.super_text = ToggleMenuBool(&misc_menu.super_mode, misc_menu.selection == 4, "SUPER MODE ON", "SUPER MODE OFF");
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            visualizer_menu.collision_wireframe_text = ToggleMenuBool(&visualizer_menu.collision_wireframe, misc_menu.selection == 6, "COLLISION WIREFRAME ON", "COLLISION WIREFRAME OFF");
            #endif

            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            if (misc_menu.selection == 5 && _currentButtonOneFrame == X_BUTTON)
            {
                MemoryWatchOpenMenu();
            }
            #endif

            // Consistency tracker: has extra side-effect on enable
            misc_menu.consitency_tracker_text = consistency_tracker_mode ? "TRACK CONSISTENCY ON" : "TRACK CONSISTENCY OFF";
            if (misc_menu.selection == 3 && (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON))
            {
                consistency_tracker_mode = !consistency_tracker_mode;
                if (consistency_tracker_mode)
                    consistency_tracker_render_time = 30;
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

        }

        if (current_menu == VISUALIZER_MENU)
        {
            int visualizer_item_count = 1;
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            visualizer_item_count = 4;
            #endif

            _spyro.isMovementLocked = TRUE;

            int textbox_bottom = 0x52;
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            textbox_bottom = 0x92;
            #endif
            CheckBackMenu();

            DrawTextBox(0x30, 0x1D0, 0x35, textbox_bottom);

            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            DrawMenuItem(visualizer_menu.hitbox_viewer_text, 0, visualizer_menu.selection, 70);
            DrawMenuItem(visualizer_menu.show_sparx_range_text, 1, visualizer_menu.selection, 70);
            DrawMenuItem(visualizer_menu.free_cam_text, 2, visualizer_menu.selection, 70);
            DrawMenuItem(visualizer_menu.theatre_mode_text, 3, visualizer_menu.selection, 70);
            #else //NTSC + IOP
            DrawMenuItem(visualizer_menu.show_sparx_range_text, 0, visualizer_menu.selection, 70);
            #endif

            if (visualizer_menu.show_sparx_range_text == NULL)
            {
                visualizer_menu.hitbox_viewer_text = "HITBOX VIEWER OFF";
                visualizer_menu.show_sparx_range_text = "SHOW SPARX RANGE OFF";
                visualizer_menu.free_cam_text = "FREE CAM OFF";
                visualizer_menu.theatre_mode_text = "THEATRE MODE";
            }

            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                visualizer_menu.selection = (visualizer_menu.selection + 1) % visualizer_item_count;
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                visualizer_menu.selection = (visualizer_menu.selection + visualizer_item_count - 1) % visualizer_item_count;
            }

            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == X_BUTTON)
            {
                PlayMenuSound();
            }
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            visualizer_menu.hitbox_viewer_text = ToggleMenuBool(&visualizer_menu.hitbox_viewer, visualizer_menu.selection == 0, "HITBOX VIEWER ON", "HITBOX VIEWER OFF");
            visualizer_menu.show_sparx_range_text = ToggleMenuBool(&show_sparx_range_mode, visualizer_menu.selection == 1, "SHOW SPARX RANGE ON", "SHOW SPARX RANGE OFF");
            visualizer_menu.free_cam_text = ToggleMenuBool(&visualizer_menu.free_cam, visualizer_menu.selection == 2, "FREE CAM ON", "FREE CAM OFF");
            if (visualizer_menu.selection == 3 && _currentButtonOneFrame == X_BUTTON)
            {
                #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
                TheatreModeEnter();
                #endif
                menu_state = MENU_HIDDEN;
                _spyro.isMovementLocked = FALSE;
            }

            #else //NTSC + IOP
            visualizer_menu.show_sparx_range_text = ToggleMenuBool(&show_sparx_range_mode, visualizer_menu.selection == 0, "SHOW SPARX RANGE ON", "SHOW SPARX RANGE OFF");
            #endif
        }

        if (current_menu == COSMETIC_MENU)
        {
            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION // For spyro preview. Would like to move out of deckard eventually
            {
                StopCameraFollowSpyro();
                CosmeticMenuSpyroLockForPreviewTick();
            }
            #endif

            _spyro.isMovementLocked = TRUE;

            CheckBackMenu();

            DrawTextBox(0x30, 0x1D0, 0xD, 0x7F);

            DrawMenuItem(cosmetic_menu.bg_color_text, 0, cosmetic_menu.selection, 30);
            DrawMenuItem(cosmetic_menu.spyro_color_text, 1, cosmetic_menu.selection, 30);
            DrawMenuItem(cosmetic_menu.flame_color_text, 2, cosmetic_menu.selection, 30);
            DrawMenuItem(cosmetic_menu.sparx_color_text, 3, cosmetic_menu.selection, 30);
            DrawMenuItem(cosmetic_menu.skin_editor_menu_text, 4, cosmetic_menu.selection, 30);

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
                PlayMenuSound();
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
                cosmetic_menu.bg_color_text = BG_COLOR_STRING_LUT[bg_color_index]; // Surprisingly, repeating "BG COLOR" in every string saves space here, because of how few options there are, combined with the amount of instructions required to sprintf the new string
            }

            // Spyro Color
            else if (cosmetic_menu.selection == 1)
            {
                const int amount_of_skins = 31;
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    spyro_color_index = (spyro_color_index + 1) % amount_of_skins;
                    should_load_spyro_color = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    spyro_color_index = (spyro_color_index + amount_of_skins - 1) % amount_of_skins;
                    should_load_spyro_color = true;
                }

                // Construct and update the string (Look up table instead of switch)
                static char constructed_skin_name[25] = { 0 };
                sprintf(constructed_skin_name, "SPYRO SKIN %s", SPYRO_SKIN_STRING_LUT[spyro_color_index]); // Construct the string to save space on the word SPYRO SKIN
                cosmetic_menu.spyro_color_text = constructed_skin_name;

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
                static char constructed_skin_name[25] = { 0 };
                sprintf(constructed_skin_name, "FLAME SKIN %s", FLAME_SKIN_STRING_LUT[flame_color_index]); // Construct the string to save space on the word FLAME SKIN
                cosmetic_menu.flame_color_text = constructed_skin_name;
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
                static char constructed_skin_name[25] = { 0 };
                sprintf(constructed_skin_name, "SPARX SKIN %s", SPARX_SKIN_STRING_LUT[sparx_color_index]);
                cosmetic_menu.sparx_color_text = constructed_skin_name;
            }

            else if (cosmetic_menu.selection == 4)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = SKIN_EDITOR_MENU;
                    cosmetic_menu.selection = 0;
                    //RestartDrawWorldAndObjects();
                }
            }

            #if BUILD == PS2_DECKARD || BUILD == REDUX || BUILD == DUCKSTATION
            CosmeticMenuSetPreviewMode(COSMETIC_PREVIEW_SPYRO);
            CosmeticMenuDrawSpyroPreview();
            #endif
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
    static const int kDrawObjectsOpcodes[2] = { 0x27BDFFE8, 0xAFBF0010 };
    static const int kDrawParticlesOpcodes[2] = { 0x3C011F80, 0xAC300000 };
    /* RenderAllSkyBoxes @ 80050bd0 — original: lui v0,0x8007 ; lw v0,0x5858(v0) */
    static const int kRenderAllSkyBoxesOpcodes[2] = { 0x3C028007, 0x8C425858 };

    PracticeMenuPauseScreenFillSaveAndGrey();

    s_draw_world_stop_reasons |= DRAW_WORLD_STOP_FULL;
    ApplyDrawWorldPatchState();
    FunctionCallToggleApply((void*)0x80019698, kDrawObjectsOpcodes);
    FunctionCallToggleApply((void*)0x800573c8, kDrawParticlesOpcodes);
    FunctionCallToggleApply((void*)0x80050bd0, kRenderAllSkyBoxesOpcodes);
}

void RestartDrawWorldAndObjects(void)
{
    static const int kDrawObjectsOpcodes[2] = { 0x27BDFFE8, 0xAFBF0010 };
    static const int kDrawParticlesOpcodes[2] = { 0x3C011F80, 0xAC300000 };
    static const int kRenderAllSkyBoxesOpcodes[2] = { 0x3C028007, 0x8C425858 };

    s_draw_world_stop_reasons &= ~DRAW_WORLD_STOP_FULL;
    ApplyDrawWorldPatchState();
    FunctionCallToggleRestore((void*)0x80019698, kDrawObjectsOpcodes);
    FunctionCallToggleRestore((void*)0x800573c8, kDrawParticlesOpcodes);
    FunctionCallToggleRestore((void*)0x80050bd0, kRenderAllSkyBoxesOpcodes);

    PracticeMenuPauseScreenFillRestore();
}

void StopDrawWorldOnly(void)
{
    s_draw_world_stop_reasons |= DRAW_WORLD_STOP_WORLD_ONLY;
    ApplyDrawWorldPatchState();
}

void RestartDrawWorldOnly(void)
{
    s_draw_world_stop_reasons &= ~DRAW_WORLD_STOP_WORLD_ONLY;
    ApplyDrawWorldPatchState();
}

void StopCameraFollowSpyro(void)
{
    int* camera_follow_spyro_func = 0x80037bd4;

    *camera_follow_spyro_func = JR_RA;
    *(camera_follow_spyro_func + 1) = NOP;
}
void RestartCameraFollowSpyro(void)
{
    int* camera_follow_spyro_func = 0x80037bd4;

    *camera_follow_spyro_func = 0x27BDFFE8;
    *(camera_follow_spyro_func + 1) = 0x3C038008;

}

void StopUpdateSpyro(void)
{
    int* update_spyro_func = 0x8004a200;

    *update_spyro_func = JR_RA;
    *(update_spyro_func + 1) = NOP;
}
void RestartUpdateSpyro(void)
{
    int* update_spyro_func = 0x8004a200;

    *update_spyro_func = 0x3C028008;
    *(update_spyro_func + 1) = 0x8C428C4C;
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
