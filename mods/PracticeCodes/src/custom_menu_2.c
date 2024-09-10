#include <common.h>
#include <custom_text.h>
#include <sound.h>
#include <shared_funcs.h>
#include <main_updates.h>
#include <cosmetic.h>
#include <igt.h>
#include <custom_types.h>


typedef enum MenuState
{
    MENU_HIDDEN,
    MENU_DISPLAYING

}MenuState;

typedef enum TimerState
{
    TIMER_RUNNING,
    TIMER_DISPLAYING

}TimerState;

typedef enum TimerDisplayMode
{
    TIMER_ONLY_PAUSE,
    TIMER_ALWAYS

}TimerDisplayMode;

typedef enum ILTimerDisplayMode
{
    IL_TIMER_AT_END,
    IL_TIMER_ALWAYS

}ILTimerDisplayMode;

typedef enum SparxMode
{
    PERMA_SPARX_OFF,
    PERMA_SPARX_ON,
    SPARXLESS

}SparxMode;

typedef struct Menu
{
    int selection;
    char* il_menu_text;
    char* timer_menu_text;
    char* savestate_menu_text;
    char* misc_menu_text;
}Menu;
extern Menu custom_menu;

typedef struct ILMenu
{
    int selection;
    bool il_state;
    char* il_mode_text;
    ILTimerDisplayMode il_timer_display_mode;
    char* il_timer_display_mode_text;
    bool display_on_dragon;
    char* display_on_dragon_text;
    bool display_on_land;
    char* display_on_land_text;
    bool dont_loop_level;
    char* loop_level_text;

}ILMenu;
extern ILMenu il_menu;
typedef struct TimerMenu
{
    int selection;
    bool timer_state;
    char* timer_state_text;
    TimerDisplayMode timer_display_mode;
    char* timer_display_mode_text;
    short stop_timer_button_index;
    char* stop_timer_button_text;
    short reset_timer_mode;
    char* reset_timer_button_text;

}TimerMenu;
extern TimerMenu timer_menu;

typedef struct SavestateMenu
{
    int selection;
    char* stateslot_text;
    //int savestate_button_index;
    char* savestate_button_text;
    //int loadstate_button_index;
    char* loadstate_button_text;
    //int switch_state_button_index;
    char* switch_state_button_text;

}SavestateMenu;
extern SavestateMenu savestate_menu;

typedef struct MiscMenu
{
    int selection;
    SparxMode sparx_mode;
    char* sparx_mode_text;
    bool show_dragon_touch;
    char* show_dragon_touch_text;
    bool quick_goop_mode;
    char* quick_goop_text;
}MiscMenu;
extern MiscMenu misc_menu;
bool counter_mode; // removed from struct

typedef struct CosmeticMenu
{
    int selection;
    char* bg_color_text;
    char* spyro_color_text;
    char* flame_color_text;
    char* sparx_color_text;
}CosmeticMenu;
CosmeticMenu cosmetic_menu = {0};

typedef enum CurrentMenu
{
    MAIN_MENU,
    IL_MENU,
    TIMER_MENU,
    SAVESTATE_MENU,
    MISC_MENU,
    COSMETIC_MENU
}CurrentMenu;

extern bool has_toggled_menu;

extern MenuState menu_state;
extern int current_menu;

extern int savestate_selection;

extern bool isHeld;

extern const short STOP_TIMER_BUTTONS[1];
extern const short RESET_TIMER_BUTTONS[2];

extern const short SAVESTATE_BUTTONS[2];
extern const short LOADSTATE_BUTTONS[3];

extern int savestate_button_index;
extern int loadstate_button_index;
extern int switch_state_button_index;

// Externing elsewhere
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
}ILTimerState;
extern ILTimerState il_timer_state;
extern mainTimerAtReset;


void CustomMenuUpdate2()
{
    if(menu_state == MENU_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
    {

        if (current_menu == MISC_MENU)
        {       
            CapitalTextInfo menu_text_info[3] = {{0}};

            // Easy Exit
            if(_currentButtonOneFrame == CIRCLE_BUTTON)
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

            if(misc_menu.selection == 0)
            {
                DrawTextCapitals(misc_menu.sparx_mode_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(misc_menu.sparx_mode_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            

            if(misc_menu.selection == 1)
            {
                DrawTextCapitals(misc_menu.show_dragon_touch_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(misc_menu.show_dragon_touch_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            
            if(misc_menu.selection == 2)
            {
                DrawTextCapitals(misc_menu.quick_goop_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(misc_menu.quick_goop_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }


            // Fill text with defaults if NULL
            if(misc_menu.sparx_mode_text == NULL)
            {
                misc_menu.sparx_mode_text = "SPARX MODE NORMAL";
                misc_menu.show_dragon_touch_text = "SHOW DRAGON TOUCH OFF";
                misc_menu.quick_goop_text = "QUICK GOOP OFF";
            }

            // Change Selection
            if(_currentButtonOneFrame == DOWN_BUTTON)
            {
                misc_menu.selection = (misc_menu.selection + 1) % 3;
            }
            else if(_currentButtonOneFrame == UP_BUTTON && misc_menu.selection != 0)
            {
                misc_menu.selection = misc_menu.selection - 1;
            }
            else if(_currentButtonOneFrame == UP_BUTTON && misc_menu.selection == 0)
            {
                misc_menu.selection = 2;
            }
            
            // Play Sound Effect
            if(_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            
            if(misc_menu.selection == 0)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    misc_menu.sparx_mode = (misc_menu.sparx_mode + 1) % 3;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && misc_menu.sparx_mode > 0)
                {
                    misc_menu.sparx_mode--;
                }

                if(misc_menu.sparx_mode == 0)
                {
                    misc_menu.sparx_mode_text = "SPARX NORMAL";
                }
                else if(misc_menu.sparx_mode == 1)
                {
                    misc_menu.sparx_mode_text = "PERMA SPARX";
                }
                else
                {
                    misc_menu.sparx_mode_text = "SPARXLESS";
                }
            }
            
            
            else if (misc_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    misc_menu.show_dragon_touch = (misc_menu.show_dragon_touch + 1) % 2;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && misc_menu.show_dragon_touch > 0)
                {
                    misc_menu.show_dragon_touch--;
                }

                if (misc_menu.show_dragon_touch == false)
                {
                    misc_menu.show_dragon_touch_text = "SHOW DRAGON TOUCH OFF";

                }
                else
                {
                    misc_menu.show_dragon_touch_text = "SHOW DRAGON TOUCH ON";

                }
            }


            else if (misc_menu.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    misc_menu.quick_goop_mode = (misc_menu.quick_goop_mode + 1) % 2;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && misc_menu.quick_goop_mode > 0)
                {
                    misc_menu.quick_goop_mode--;
                }

                if (misc_menu.quick_goop_mode == false)
                {
                    misc_menu.quick_goop_text = "QUICK GOOP OFF";
                }
                else
                {
                    misc_menu.quick_goop_text = "QUICK GOOP ON";
                }
            }
        }

        if (current_menu == COSMETIC_MENU)
        {       
            CapitalTextInfo cosmetic_text_info[4] = {{0}};

            // Easy Exit
            if(_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                current_menu = MAIN_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
                PlayMusic(_currentMusicTrack, 8);
            }

            DrawTextBox(0x30, 0x1D0, 0x30, 0x94);
            
            cosmetic_text_info[0].x = SCREEN_LEFT_EDGE + 0x4A;
            cosmetic_text_info[0].y = 70;
            cosmetic_text_info[0].size = DEFAULT_SIZE;

            cosmetic_text_info[1].x = SCREEN_LEFT_EDGE + 0x4A;
            cosmetic_text_info[1].y = 90;
            cosmetic_text_info[1].size = DEFAULT_SIZE;

            cosmetic_text_info[2].x = SCREEN_LEFT_EDGE + 0x4A;
            cosmetic_text_info[2].y = 110;
            cosmetic_text_info[2].size = DEFAULT_SIZE;

            cosmetic_text_info[3].x = SCREEN_LEFT_EDGE + 0x4A;
            cosmetic_text_info[3].y = 130;
            cosmetic_text_info[3].size = DEFAULT_SIZE;

            _spyro.isMovementLocked = TRUE;

            if(cosmetic_menu.selection == 0)
            {
                DrawTextCapitals(cosmetic_menu.bg_color_text, &cosmetic_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(cosmetic_menu.bg_color_text, &cosmetic_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            

            if(cosmetic_menu.selection == 1)
            {
                DrawTextCapitals(cosmetic_menu.spyro_color_text, &cosmetic_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(cosmetic_menu.spyro_color_text, &cosmetic_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            
            if(cosmetic_menu.selection == 2)
            {
                DrawTextCapitals(cosmetic_menu.flame_color_text, &cosmetic_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(cosmetic_menu.flame_color_text, &cosmetic_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if(cosmetic_menu.selection == 3)
            {
                DrawTextCapitals(cosmetic_menu.sparx_color_text, &cosmetic_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(cosmetic_menu.sparx_color_text, &cosmetic_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }


            // Fill text with defaults if NULL
            if(cosmetic_menu.bg_color_text == NULL)
            {
                cosmetic_menu.bg_color_text = "BG BLUE";
                cosmetic_menu.spyro_color_text = "SPYRO SKIN DEFAULT";
                cosmetic_menu.flame_color_text = "FLAME SKIN DEFAULT";
                cosmetic_menu.sparx_color_text = "SPARX SKIN DEFAULT";
            }

            // Change Selection
            if(_currentButtonOneFrame == DOWN_BUTTON)
            {
                cosmetic_menu.selection = (cosmetic_menu.selection + 1) % 4;
            }
            else if(_currentButtonOneFrame == UP_BUTTON && cosmetic_menu.selection != 0)
            {
                cosmetic_menu.selection = cosmetic_menu.selection - 1;
            }
            else if(_currentButtonOneFrame == UP_BUTTON && cosmetic_menu.selection == 0)
            {
                cosmetic_menu.selection = 3;
            }
            
            // Play Sound Effect
            if(_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }


            if(cosmetic_menu.selection == 0)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    bg_color_index = (bg_color_index + 1) % 7;
                    should_update_bg_color = TRUE;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && bg_color_index > 0)
                {
                    bg_color_index--;
                    should_update_bg_color = TRUE;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && bg_color_index == 0)
                {
                    bg_color_index = 6;
                    should_update_bg_color = TRUE;
                }

                switch(bg_color_index)
                {
                    case(BG_BLUE):
                    {
                        cosmetic_menu.bg_color_text = "BG BLUE";
                        break;
                    }
                    case(BG_PURPLE):
                    {
                        cosmetic_menu.bg_color_text = "BG PURPLE";
                        break;
                    }
                    case(BG_TEAL):
                    {
                        cosmetic_menu.bg_color_text = "BG TEAL";
                        break;
                    }
                    case(BG_GREY):
                    {
                        cosmetic_menu.bg_color_text = "BG GREY";
                        break;
                    }
                    case(BG_PINK):
                    {
                        cosmetic_menu.bg_color_text = "BG PINK";
                        break;
                    }
                    case(BG_ORANGE):
                    {
                        cosmetic_menu.bg_color_text = "BG ORANGE";
                        break;    
                    }
                    case(BG_YELLOW):
                    {
                        cosmetic_menu.bg_color_text = "BG YELLOW";
                        break;
                    }
                }
            }

            // Spyro Color
            else if(cosmetic_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    spyro_color_index = (spyro_color_index + 1) % 17;
                    should_load_spyro_color = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && spyro_color_index > 0)
                {
                    spyro_color_index--;
                    should_load_spyro_color = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && spyro_color_index == 0)
                {
                    spyro_color_index = 16;
                    should_load_spyro_color = true;
                }

                switch(spyro_color_index)
                {
                    case(SKIN_DEFAULT):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN DEFAULT";
                        break;
                    }
                    case(SKIN_DARK_RED):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN DARK RED";
                        break;
                    }
                    case(SKIN_ICY):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN ICY";
                        break;
                    }
                    case(SKIN_EMERALD):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN EMERALD";
                        break;
                    }
                    case(SKIN_CORAL_BLUE):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN CORAL BLUE";
                        break;
                    }
                    case(SKIN_GOLD):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN GOLD";
                        break;
                    }
                    case(SKIN_BERRY):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN BERRY";
                        break;
                    }
                    case(SKIN_GREY):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN GREY";
                        break;
                    }
                    case(SKIN_ZERA):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN ZERA";
                        break;
                    }
                    case(SKIN_PIXIE):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN PIXIE";
                        break;
                    }
                    case(SKIN_JAYO):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN JAYO";
                        break;
                    }
                    case(SKIN_PERIDOT):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN PERIDOT";
                        break;
                    }
                    case(SKIN_TRANS):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN TRANS";
                        break;
                    }
                    case(SKIN_GOTH):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN GOTH";
                        break;
                    }
                    case(SKIN_DITTO):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN DITTO";
                        break;
                    }
                    case(SKIN_EMBER):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN EMBER";
                        break;
                    }
                    case(SKIN_CYNDER):
                    {
                        cosmetic_menu.spyro_color_text = "SPYRO SKIN CYNDER";
                        break;
                    }
                }
            }
            else if(cosmetic_menu.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    flame_color_index = (flame_color_index + 1) % 5;
                    should_load_flame_color = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && flame_color_index > 0)
                {
                    flame_color_index--;
                    should_load_flame_color = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && flame_color_index == 0)
                {
                    flame_color_index = 4;
                    should_load_flame_color = true;
                }

                switch(flame_color_index)
                {
                    case(FLAME_SKIN_DEFAULT):
                    {
                        cosmetic_menu.flame_color_text = "FLAME SKIN DEFAULT";
                        break;
                    }
                    case(FLAME_SKIN_BLOOD):
                    {
                        cosmetic_menu.flame_color_text = "FLAME SKIN BLOOD";
                        break;
                    }
                    case(FLAME_SKIN_ICE):
                    {
                        cosmetic_menu.flame_color_text = "FLAME SKIN ICE";
                        break;
                    }
                    case(FLAME_SKIN_JADE):
                    {
                        cosmetic_menu.flame_color_text = "FLAME SKIN JADE";
                        break;
                    }
                    case(FLAME_SKIN_GHOST):
                    {
                        cosmetic_menu.flame_color_text = "FLAME SKIN GHOST";
                        break;
                    }
                }
            }

            else if(cosmetic_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    sparx_color_index = (sparx_color_index + 1) % 9;
                    should_load_sparx_color = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && sparx_color_index > 0)
                {
                    sparx_color_index--;
                    should_load_sparx_color = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && sparx_color_index == 0)
                {
                    sparx_color_index = 8;
                    should_load_sparx_color = true;
                }

                switch(sparx_color_index)
                {
                    case(SPARX_SKIN_DEFAULT):
                    {
                        cosmetic_menu.sparx_color_text = "SPARX SKIN DEFAULT";
                        break;
                    }
                    case(SPARX_SKIN_RED):
                    {
                        cosmetic_menu.sparx_color_text = "SPARX SKIN RED";
                        break;
                    }
                    case(SPARX_SKIN_PURPLE):
                    {
                        cosmetic_menu.sparx_color_text = "SPARX SKIN PURPLE";
                        break;
                    }
                    case(SPARX_SKIN_ORANGE):
                    {
                        cosmetic_menu.sparx_color_text = "SPARX SKIN ORANGE";
                        break;
                    }
                    case(SPARX_SKIN_DARK_BLUE):
                    {
                        cosmetic_menu.sparx_color_text = "SPARX SKIN DARK BLUE";
                        break;
                    }
                    case(SPARX_SKIN_PINK):
                    {
                        cosmetic_menu.sparx_color_text = "SPARX SKIN PINK";
                        break;
                    }
                    case(SPARX_SKIN_TURQUOISE):
                    {
                        cosmetic_menu.sparx_color_text = "SPARX SKIN TURQUOISE";
                        break;
                    }
                    case(SPARX_SKIN_SILVER):
                    {
                        cosmetic_menu.sparx_color_text = "SPARX SKIN SILVER";
                        break;
                    }
                    case(SPARX_SKIN_BLACK):
                    {
                        cosmetic_menu.sparx_color_text = "SPARX SKIN BLACK";
                        break;
                    }
                }
            }

            // if(misc_menu.selection == 4)
            // {
            //     if (_currentButtonOneFrame == RIGHT_BUTTON)
            //     {
            //         counter_mode = (counter_mode + 1) % 2;
            //     }
            //     else if (_currentButtonOneFrame == LEFT_BUTTON && counter_mode > 0)
            //     {
            //         counter_mode;
            //     }

            //     if(counter_mode == 0)
            //     {
            //         counter_mode_text = "COUNTER OFF";
            //     }
            //     else if(counter_mode == 1)
            //     {
            //         counter_mode_text = "COUNTER ON";
            //     }
            // }
        }
    }

    if(((timer_menu.timer_display_mode == TIMER_ALWAYS || il_menu.il_timer_display_mode == IL_TIMER_ALWAYS || timerState == TIMER_DISPLAYING || menu_state == MENU_DISPLAYING || ShouldDisplayLandingTime()) && _gameState == GAMESTATE_GAMEPLAY)
    || (il_timer_state == IL_DISPLAYING && _gameState == GAMESTATE_LOADING) 
    || ((il_menu.display_on_dragon == TRUE || misc_menu.show_dragon_touch == TRUE) && _gameState == GAMESTATE_DRAGON_STATE))
    {
        //printf("RENDERING\n");
        RenderShadedMobyQueue();
    }
}