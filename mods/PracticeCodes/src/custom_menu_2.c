#include <common.h>
#include <custom_text.h>
#include <sound.h>
#include <shared_funcs.h>
#include <main_updates.h>
#include <bg_colors.h>
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
    char* bg_color_text;
    char* spyro_color_text;
    int counter_mode;
    char* counter_mode_text;
}MiscMenu;
extern MiscMenu misc_menu;
bool counter_mode; // removed from struct

typedef enum CurrentMenu
{
    MAIN_MENU,
    IL_MENU,
    TIMER_MENU,
    SAVESTATE_MENU,
    MISC_MENU
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
extern BackgroundColor bg_color_index;
extern SpyroColor spyro_color_index;
extern bool should_update_bg_color;
extern bool should_load_spyro_color;
extern bool should_loadstate_gems;
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


void CustomMenuUpdate2(){
    if(menu_state == MENU_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
    {

        if (current_menu == MISC_MENU)
        {       
            CapitalTextInfo menu_text_info[5] = {{0}};

            // Easy Exit
            if(_currentButtonOneFrame == CIRCLE_BUTTON || _currentButtonOneFrame == TRIANGLE_BUTTON)
            {
                current_menu = MAIN_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            DrawTextBox(0x30, 0x1D0, 0x30, 0xA8);
            
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

            if(misc_menu.selection == 3)
            {
                DrawTextCapitals(misc_menu.bg_color_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(misc_menu.bg_color_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if(misc_menu.selection == 4)
            {
                DrawTextCapitals(misc_menu.spyro_color_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(misc_menu.spyro_color_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }


            // Fill text with defaults if NULL
            if(misc_menu.sparx_mode_text == NULL)
            {
                misc_menu.sparx_mode_text = "SPARX MODE NORMAL";
                misc_menu.show_dragon_touch_text = "SHOW DRAGON TOUCH OFF";
                misc_menu.quick_goop_text = "QUICK GOOP OFF";
                misc_menu.bg_color_text = "BG BLUE";
                misc_menu.spyro_color_text = "SKIN DEFAULT";
            }

            // Change Selection
            if(_currentButtonOneFrame == DOWN_BUTTON)
            {
                misc_menu.selection = (misc_menu.selection + 1) % 5;
            }
            else if(_currentButtonOneFrame == UP_BUTTON && misc_menu.selection != 0)
            {
                misc_menu.selection = misc_menu.selection - 1;
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
                    misc_menu.sparx_mode;
                }

                if(misc_menu.sparx_mode == 0)
                {
                    misc_menu.sparx_mode_text = "SPARX NORMAL";
                }
                else if(misc_menu.sparx_mode == 1)
                {
                    misc_menu.sparx_mode_text = "PERMA SPARX";
                }
                else if(misc_menu.sparx_mode == 2)
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
                else if (misc_menu.show_dragon_touch == TRUE)
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
                else if (misc_menu.quick_goop_mode == true)
                {
                    misc_menu.quick_goop_text = "QUICK GOOP ON";
                }
            }


            else if(misc_menu.selection == 3)
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
                        misc_menu.bg_color_text = "BG BLUE";
                        break;
                    }
                    case(BG_PURPLE):
                    {
                        misc_menu.bg_color_text = "BG PURPLE";
                        break;
                    }
                    case(BG_TEAL):
                    {
                        misc_menu.bg_color_text = "BG TEAL";
                        break;
                    }
                    case(BG_GREY):
                    {
                        misc_menu.bg_color_text = "BG GREY";
                        break;
                    }
                    case(BG_PINK):
                    {
                        misc_menu.bg_color_text = "BG PINK";
                        break;
                    }
                    case(BG_ORANGE):
                    {
                        misc_menu.bg_color_text = "BG ORANGE";
                        break;    
                    }
                    case(BG_YELLOW):
                    {
                        misc_menu.bg_color_text = "BG YELLOW";
                        break;
                    }
                }
            }

            // Spyro Color
            else if(misc_menu.selection == 4)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    spyro_color_index = (spyro_color_index + 1) % 15;
                    should_load_spyro_color = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && spyro_color_index > 0)
                {
                    spyro_color_index--;
                    should_load_spyro_color = true;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON && spyro_color_index == 0)
                {
                    spyro_color_index = 14;
                    should_load_spyro_color = true;
                }

                switch(spyro_color_index)
                {
                    case(SKIN_DEFAULT):
                    {
                        misc_menu.spyro_color_text = "SKIN DEFAULT";
                        break;
                    }
                    case(SKIN_DARK_RED):
                    {
                        misc_menu.spyro_color_text = "SKIN DARK RED";
                        break;
                    }
                    case(SKIN_ICY):
                    {
                        misc_menu.spyro_color_text = "SKIN ICY";
                        break;
                    }
                    case(SKIN_CORAL_BLUE):
                    {
                        misc_menu.spyro_color_text = "SKIN CORAL BLUE";
                        break;
                    }
                    case(SKIN_EMERALD):
                    {
                        misc_menu.spyro_color_text = "SKIN EMERALD";
                        break;
                    }
                    case(SKIN_GOLD):
                    {
                        misc_menu.spyro_color_text = "SKIN GOLD";
                        break;
                    }
                    case(SKIN_EMBER):
                    {
                        misc_menu.spyro_color_text = "SKIN EMBER";
                        break;
                    }
                    case(SKIN_ZERA):
                    {
                        misc_menu.spyro_color_text = "SKIN ZERA";
                        break;
                    }
                    case(SKIN_BERRY):
                    {
                        misc_menu.spyro_color_text = "SKIN BERRY";
                        break;
                    }
                    case(SKIN_PIXIE):
                    {
                        misc_menu.spyro_color_text = "SKIN PIXIE";
                        break;
                    }
                    case(SKIN_JAYO):
                    {
                        misc_menu.spyro_color_text = "SKIN JAYO";
                        break;
                    }
                    case(SKIN_TRANS):
                    {
                        misc_menu.spyro_color_text = "SKIN TRANS";
                        break;
                    }
                    case(SKIN_DITTO):
                    {
                        misc_menu.spyro_color_text = "SKIN DITTO";
                        break;
                    }
                    case(SKIN_GOTH):
                    {
                        misc_menu.spyro_color_text = "SKIN GOTH";
                        break;
                    }
                    case(SKIN_CYNDER):
                    {
                        misc_menu.spyro_color_text = "SKIN CYNDER";
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
        MobyRender();
    }
}