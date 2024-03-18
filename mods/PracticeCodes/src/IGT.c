#include <common.h>
#include <custom_text.h>
#include <sound.h>
#include <shared_funcs.h>
#include <bg_colors.h>

typedef enum TimerState
{
    TIMER_STOPPED,
    TIMER_RUNNING_NO_DISPLAY,
    TIMER_RUNNING,
    TIMER_DISPLAYING

}TimerState;

typedef enum ILTimerState
{
    IL_FLYING_IN,
    IL_STARTED,
    IL_DISPLAYING,
    IL_STOPPED

}ILTimerState;

typedef enum MenuState
{
    MENU_HIDDEN,
    MENU_DISPLAYING

}MenuState;

typedef enum TimerMode
{
    TIMER_OFF,
    TIMER_ONLY_PAUSE,
    TIMER_ALWAYS

}TimerMode;

typedef enum FPSMode
{
    FPS_OFF,
    FPS_ONLY_DROPPED,
    FPS_ALWAYS

}FPSMode;

typedef enum SparxMode
{
    PERMA_SPARX_OFF,
    PERMA_SPARX_ON,
    SPARXLESS

}SparxMode;


typedef struct Menu
{
    int selection;
    TimerMode timer_mode;
    char* timer_mode_text;
    bool il_mode;
    char* il_mode_text;
    int loadstate_mode;
    char* loadstate_mode_text;
    SparxMode sparx_mode;
    char* sparx_mode_text;
    bool quick_goop_mode;
    char* quick_goop_text;
    char* bg_color_text;

}Menu;


typedef struct Timer
{
    int timer;
    int timer_at_reset;
    int secondsOnesPlace;
    int secondsTensPlace;
    int milisecondsTenthsPlace;
    int milisecondsHundrethsPlace;
    int minutes;
    char ascii[10];

}Timer;

typedef struct FPS_t
{
    int compareTimer;
    int difference;
    int fps;
    int difference_from_baseline;
}FPS_t;

// Globals
bool has_toggled_menu = FALSE;

//Timer mainTimer = {0};                    // Most of the Timer Struct doesn't need to be global
int mainTimerAtReset;                       // so I moved the timer_at_reset and ascii elements to be individual globals
char mainTimerAscii[10];                    // The structs can be reworked to not include these anymore if this change sticks
TimerState timer_state = TIMER_STOPPED;     // The timer struct is still being used but it just initialized on the stack now

ILTimerState il_timer_state = IL_STOPPED;
int framesSpentLoading = 0;
int ilTimerStart = 0;
char ilAscii[10];
char loadlessAscii[10];

Menu custom_menu = {0};
MenuState menu_state = MENU_HIDDEN;

// Externing elsewhere
BackgroundColor bg_color_index;
bool should_update_bg_color = true;
bool should_loadstate_gems = false;

// Externed from elsewhere
extern const short flyInArray[36];


//Math for 29.91hz
void FramesToTimer(Timer* ptr_timer)
{
    ptr_timer->minutes = (ptr_timer->timer * 10) / 35892;
    ptr_timer->secondsTensPlace = ((ptr_timer->timer * 10) % 35892) / 5982;
    ptr_timer->secondsOnesPlace = ((ptr_timer->timer * 100) % 59820) / 5982;
    ptr_timer->milisecondsTenthsPlace = ((ptr_timer->timer * 1000) % 59820) / 5982;
    ptr_timer->milisecondsHundrethsPlace = ((ptr_timer->timer * 10000) % 59820) / 5982;
}

void LoadAscii(Timer* ptr_timer, char* ascii){
    sprintf(ascii, "%d.%d%d.%d%d", ptr_timer->minutes, ptr_timer->secondsTensPlace, ptr_timer->secondsOnesPlace, ptr_timer->milisecondsTenthsPlace, ptr_timer->milisecondsHundrethsPlace);
}

void InGameTimerUpdate()
{
   //! Timer
    {

        if(timer_state != TIMER_STOPPED)
        {
            //Main Timer Checks/Loop

            //Button Checks
            if(_currentButtonOneFrame == R3_BUTTON)
            {   
                mainTimerAtReset = _globalTimer;
                timer_state = (TimerState)custom_menu.timer_mode;
            }

            if(_currentButtonOneFrame == START_BUTTON)
            {
                timer_state = TIMER_DISPLAYING;
            }
            

            //Show the running timer
            if(timer_state == TIMER_RUNNING || timer_state == TIMER_RUNNING_NO_DISPLAY)
            {
                Timer mainTimer;
                mainTimer.timer = _globalTimer - mainTimerAtReset;
                FramesToTimer(&mainTimer);

                //Seconds
                if(mainTimer.minutes == 0)
                {
                    sprintf(mainTimerAscii, "%d%d.%d%d", mainTimer.secondsTensPlace, mainTimer.secondsOnesPlace, mainTimer.milisecondsTenthsPlace, mainTimer.milisecondsHundrethsPlace);
                }

                //Minutes
                else if(mainTimer.minutes >= 1)
                {
                    LoadAscii(&mainTimer, mainTimerAscii);
                }

                CapitalTextInfo timer_text_info = {0};
                timer_text_info.x = SCREEN_RIGHT_EDGE - 0x80;
                timer_text_info.y = SCREEN_BOTTOM_EDGE - 0xA;
                timer_text_info.size = DEFAULT_SIZE;

                if(timer_state == TIMER_RUNNING && _gameState == GAMESTATE_GAMEPLAY)
                    DrawTextCapitals(mainTimerAscii, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);

            }

            //Show the saved timer
            if(timer_state == TIMER_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
            {
                CapitalTextInfo timer_text_info = {0};
                timer_text_info.x = SCREEN_RIGHT_EDGE - 0x80;
                timer_text_info.y = SCREEN_BOTTOM_EDGE - 0xA;
                timer_text_info.size = DEFAULT_SIZE;
                DrawTextCapitals(mainTimerAscii, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);                
            }

        }
        
        //! IL Timer Stuffs
        {
            if(custom_menu.il_mode)
            {
                if(il_timer_state == IL_STOPPED && (_gameState == GAMESTATE_FLY_IN || _gameState == GAMESTATE_LOADING))
                {
                    il_timer_state = IL_FLYING_IN;
                }
                else if(il_timer_state == IL_FLYING_IN && _gameState == GAMESTATE_GAMEPLAY)
                {
                    il_timer_state = IL_STARTED;
                    ilTimerStart = _globalTimer;
                    framesSpentLoading = 0;
                }
                else if(il_timer_state == IL_STARTED && (_dragonState == 2 || _dragonState == 6)){ //State 2 is after spyro has finished walking but the cd load is still going and state 6 is for the cd load after the dragon cut scene
                    framesSpentLoading++;
                }
                else if(il_timer_state == IL_STARTED && _gameState == GAMESTATE_LOADING)
                {
                    Timer ilTimer;
                    ilTimer.timer = _globalTimer - ilTimerStart;
                    FramesToTimer(&ilTimer);
                    LoadAscii(&ilTimer, ilAscii);

                    ilTimer.timer = ilTimer.timer - (2 * framesSpentLoading);
                    FramesToTimer(&ilTimer);
                    LoadAscii(&ilTimer, loadlessAscii);

                    il_timer_state = IL_DISPLAYING;

                    // IL Looping
                    if((_spyro.timer_framesInAir != 1 && _portalNumber == -1))
                    {
                        _levelID = _portalToExitFromInHW;
                        _portalToExitFromInHW = 0;
                        _flyInAnimation = flyInArray[_levelIDIndex];
                    }
                }
                
                //DISPLAY
                if(il_timer_state == IL_DISPLAYING)
                {
                    CapitalTextInfo il_text_info = {SCREEN_LEFT_EDGE + 0x10, 50, 0x1400};
                    CapitalTextInfo il2_text_info = {SCREEN_LEFT_EDGE + 0x10, 65, 0x1800};
                    DrawTextCapitals(ilAscii, &il_text_info, 0xF, MOBY_COLOR_PURPLE);
                    DrawTextCapitals(loadlessAscii, &il2_text_info, 0xB, MOBY_COLOR_PURPLE);

                    if(_levelLoadState >= 0xB)
                    {
                        il_timer_state = IL_STOPPED;
                        ResetLevelCollectables();
                    }
                }
            }
        }
    }
}

void CustomMenuUpdate(void)
{ 
    // Open Menu
    if(_currentButton == L2_BUTTON + R2_BUTTON + TRIANGLE_BUTTON && !has_toggled_menu && _movementSubState != MOVEMENT_SUBSTATE_FLY_IN_TREE_TOPS)
    {
        menu_state = !menu_state;
        has_toggled_menu = TRUE;
        PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);

        if(menu_state == MENU_HIDDEN)
            _spyro.isMovementLocked = FALSE;
    }

    // When Displaying Menu
    if(menu_state == MENU_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
    {
        CapitalTextInfo menu_text_info[6] = {{0}};

        // Easy Exit
        if(_currentButtonOneFrame == CIRCLE_BUTTON)
        {
            menu_state = MENU_HIDDEN;
            _spyro.isMovementLocked = FALSE;
            PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
        }

        DrawTextBox(0x30, 0x1D0, 0x30, 0xBE);
        
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

        menu_text_info[5].x = SCREEN_LEFT_EDGE + 0x4A;
        menu_text_info[5].y = 170;
        menu_text_info[5].size = DEFAULT_SIZE;

        _spyro.isMovementLocked = TRUE;

        if(custom_menu.selection == 0)
        {
            DrawTextCapitals(custom_menu.timer_mode_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
        }
        else{
            DrawTextCapitals(custom_menu.timer_mode_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
        }
        
        if(custom_menu.selection == 1)
        {
            DrawTextCapitals(custom_menu.il_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
        }
        else{
            DrawTextCapitals(custom_menu.il_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
        }

        if(custom_menu.selection == 2)
        {
            DrawTextCapitals(custom_menu.loadstate_mode_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
        }
        else{
            DrawTextCapitals(custom_menu.loadstate_mode_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
        }

        if(custom_menu.selection == 3)
        {
            DrawTextCapitals(custom_menu.sparx_mode_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
        }
        else{
            DrawTextCapitals(custom_menu.sparx_mode_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
        }
        
        if(custom_menu.selection == 4)
        {
            DrawTextCapitals(custom_menu.quick_goop_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_GOLD);
        }
        else{
            DrawTextCapitals(custom_menu.quick_goop_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
        }
        
        if(custom_menu.selection == 5)
        {
            DrawTextCapitals(custom_menu.bg_color_text, &menu_text_info[5], DEFAULT_SPACING, MOBY_COLOR_GOLD);
        }
        else{
            DrawTextCapitals(custom_menu.bg_color_text, &menu_text_info[5], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
        }


        // Fill text with defaults if NULL
        if(custom_menu.loadstate_mode_text == NULL)
        {
            custom_menu.il_mode_text = "IL MODE OFF";
            custom_menu.loadstate_mode_text = "RESET ALL WITH R3 OFF";
            custom_menu.sparx_mode_text = "SPARX NORMAL";
            custom_menu.quick_goop_text = "QUICK GOOP OFF";
            custom_menu.bg_color_text = "BG PINK";
        }

        // Change Selection
        if(_currentButtonOneFrame == DOWN_BUTTON)
        {
            custom_menu.selection = (custom_menu.selection + 1) % 6;
        }
        else if(_currentButtonOneFrame == UP_BUTTON && custom_menu.selection != 0)
        {
            custom_menu.selection = custom_menu.selection - 1;
        }
        
        // Play Sound Effect
        if(_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
        {
            PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
        }

        // Timer Selection
        if(custom_menu.selection == 0)
        {
            if(_currentButtonOneFrame == RIGHT_BUTTON)
            {
                custom_menu.timer_mode = (custom_menu.timer_mode + 1) % 3;
                mainTimerAtReset = _globalTimer;
            }
            if(_currentButtonOneFrame == LEFT_BUTTON)
            {
                custom_menu.timer_mode = (custom_menu.timer_mode - 1) % 3;
                mainTimerAtReset = _globalTimer;
            }

            if(custom_menu.timer_mode == TIMER_OFF)
            {
                custom_menu.timer_mode_text = "TIMER OFF";
                timer_state = (TimerState)TIMER_OFF;
            }
            else if(custom_menu.timer_mode == TIMER_ONLY_PAUSE)
            {
                custom_menu.timer_mode_text = "TIMER ONLY STOPPED";

                if(timer_state != TIMER_DISPLAYING)
                    timer_state = TIMER_RUNNING_NO_DISPLAY;
            }
            else if(custom_menu.timer_mode == TIMER_ALWAYS)
            {
                custom_menu.timer_mode_text = "TIMER ALWAYS ON";

                if(timer_state != TIMER_DISPLAYING)
                    timer_state = TIMER_RUNNING;
            }
        }

        // IL Looping Selection
        else if(custom_menu.selection == 1)
        {
            if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
            {
                custom_menu.il_mode = (custom_menu.il_mode + 1) % 2;
            }

            if(custom_menu.il_mode == FALSE)
            {
                custom_menu.il_mode_text = "IL MODE OFF";

            }
            else if(custom_menu.il_mode == TRUE)
            {
                custom_menu.il_mode_text = "IL MODE ON";

            }

        }

        // Reset Gems W R3 Selection
        else if(custom_menu.selection == 2)
        {

            if(_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
            {
                custom_menu.loadstate_mode = (custom_menu.loadstate_mode + 1) % 2;
            }

            //Flipped for sake of on being first option
            if(custom_menu.loadstate_mode == 0)
            {
                custom_menu.loadstate_mode_text = "RESET ALL WITH R3 OFF";
                should_loadstate_gems = false;

            }
            else if(custom_menu.loadstate_mode == 1)
            {
                custom_menu.loadstate_mode_text = "RESET ALL WITH R3 ON";
                should_loadstate_gems = true;

            }

        }

        // Sparx Selection
        else if(custom_menu.selection == 3)
        {
            if(_currentButtonOneFrame == RIGHT_BUTTON)
            {
                custom_menu.sparx_mode = (custom_menu.sparx_mode + 1) % 3;
            }
            else if(_currentButtonOneFrame == LEFT_BUTTON)
            {
                custom_menu.sparx_mode = (custom_menu.sparx_mode - 1) % 3;
            }

            if(custom_menu.sparx_mode == PERMA_SPARX_OFF)
            {
                custom_menu.sparx_mode_text = "SPARX NORMAL";

            }
            else if(custom_menu.sparx_mode == PERMA_SPARX_ON)
            {
                custom_menu.sparx_mode_text = "PERMA SPARX ON";

            }
            else if(custom_menu.sparx_mode == SPARXLESS)
            {
                custom_menu.sparx_mode_text = "SPARXLESS ON";

            }
        }

        // Quick Goop Selection
        else if(custom_menu.selection == 4)
        {
            if(_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
            {
                custom_menu.quick_goop_mode = (custom_menu.quick_goop_mode + 1) % 2;
            }

            if(custom_menu.quick_goop_mode == FALSE)
            {
                custom_menu.quick_goop_text = "QUICK GOOP OFF";

            }
            else if(custom_menu.quick_goop_mode == TRUE)
            {
                custom_menu.quick_goop_text = "QUICK GOOP ON";

            }
        }

        // BG Color Selection
        else if(custom_menu.selection == 5)
        {
            if(_currentButtonOneFrame == RIGHT_BUTTON)
            {
                bg_color_index = (bg_color_index + 1) % 6;
                should_update_bg_color = TRUE;
            }
            else if(_currentButtonOneFrame == LEFT_BUTTON && bg_color_index > 0)
            {
                bg_color_index = (bg_color_index - 1) % 6;
                should_update_bg_color = TRUE;
            }

            switch(bg_color_index)
            {
                case(BG_PINK):
                {
                    custom_menu.bg_color_text = "BG PINK";
                    break;
                }
                case(BG_YELLOW):
                {
                    custom_menu.bg_color_text = "BG YELLOW";
                    break;
                }
                case(BG_TEAL):
                {
                    custom_menu.bg_color_text = "BG TEAL";
                    break;
                }
                case(BG_PURPLE):
                {
                    custom_menu.bg_color_text = "BG PURPLE";
                    break;
                }
                case(BG_BLUE):
                {
                    custom_menu.bg_color_text = "BG BLUE";
                    break;
                }
                case(BG_GREY):
                {
                    custom_menu.bg_color_text = "BG GREY";
                    break;
                }
                default:
                {
                    custom_menu.bg_color_text = "BG GREY";
                    break;
                }
            }
    
        }

    }
    
    // Has Released Menu Button
    if(_currentButton != L2_BUTTON + R2_BUTTON + TRIANGLE_BUTTON)
    {
        has_toggled_menu = FALSE;
    }


    //! Sparx and Quick Goop
    {
        if(custom_menu.sparx_mode == PERMA_SPARX_ON)
        {
            _spyro.health = 3;
        }
        else if(custom_menu.sparx_mode == SPARXLESS)
        {
            if (_spyro.health > 0)
            {
                _spyro.health = 0;
            }
        }

        if(custom_menu.quick_goop_mode == TRUE)
        {
            if(_spyro.drownTimer > 100)
            {
                _spyro.drownTimer = 0x240;
            }
        }
    }

    if(((custom_menu.timer_mode != TIMER_OFF || menu_state == MENU_DISPLAYING) && _gameState == GAMESTATE_GAMEPLAY) || (il_timer_state == IL_DISPLAYING && _gameState == GAMESTATE_LOADING))
    {
        //printf("RENDERING\n");
        MobyRender();
    }
}