#include <common.h>
#include <custom_text.h>
#include <sound.h>
#include <shared_funcs.h>
#include <bg_colors.h>
#include <igt.h>

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

typedef enum TimerDisplayMode
{
    TIMER_OFF,
    TIMER_ONLY_PAUSE,
    TIMER_ALWAYS

}TimerDisplayMode;

typedef enum ILTimerDisplayMode
{
    IL_TIMER_AT_END,
    IL_TIMER_ALWAYS,
    IL_TIMER_OFF

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
Menu custom_menu = {0};

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
ILMenu il_menu = {0};
typedef struct TimerMenu
{
    int selection;
    bool timer_state;
    char* timer_state_text;
    TimerDisplayMode timer_display_mode;
    char* timer_display_mode_text;
    short stop_timer_button_index;
    char* stop_timer_button_text;
    short reset_timer_button_index;
    char* reset_timer_button_text;

}TimerMenu;
TimerMenu timer_menu = {0};

typedef struct SavestateMenu
{
    int selection;
    //
    char* stateslot_text;
    int savestate_button_index;
    char* savestate_button_text;
    int loadstate_button_index;
    char* loadstate_button_text;
    int switch_state_button_index;
    char* switch_state_button_text;

}SavestateMenu;
SavestateMenu savestate_menu = {0};

typedef struct MiscMenu
{
    int selection;
    SparxMode sparx_mode;
    char* sparx_mode_text;
    bool quick_goop_mode;
    char* quick_goop_text;
    char* bg_color_text;
}MiscMenu;
MiscMenu misc_menu = {0};

typedef enum CurrentMenu
{
    MAIN_MENU,
    IL_MENU,
    TIMER_MENU,
    SAVESTATE_MENU,
    MISC_MENU
}CurrentMenu;

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

MenuState menu_state = MENU_HIDDEN;
int current_menu = MAIN_MENU;

int savestate_selection = 0;

bool isHeld = false;

const short STOP_TIMER_BUTTONS[1] = {START_BUTTON, START_BUTTON};
const short RESET_TIMER_BUTTONS[2] = {R3_BUTTON, SELECT_BUTTON};

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
            if(_currentButtonOneFrame == RESET_TIMER_BUTTONS[timer_menu.reset_timer_button_index])
            {   
                mainTimerAtReset = _globalTimer;
                timer_state = (TimerState)timer_menu.timer_display_mode;
            }
            if(_currentButton == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON && !isHeld)
            {
                mainTimerAtReset = _globalTimer;  //Resets timer to 0 by syncing up to the global timer
                timer_state = (TimerState)timer_menu.timer_display_mode;
                isHeld = true;
            }
            if(_currentButton == L1_BUTTON + R1_BUTTON + TRIANGLE_BUTTON && !isHeld)
            {
                mainTimerAtReset = _globalTimer;  //Resets timer to 0 by syncing up to the global timer
                timer_state = (TimerState)timer_menu.timer_display_mode;
                isHeld = true;

            }

            //Show the running timer
            if(timer_state == TIMER_RUNNING || (timer_state == TIMER_RUNNING_NO_DISPLAY && _currentButtonOneFrame == START_BUTTON))
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
            }

            if(_currentButtonOneFrame == STOP_TIMER_BUTTONS[timer_menu.stop_timer_button_index])
            {
                timer_state = TIMER_DISPLAYING;
            }

            //Show the saved timer
            if(timer_state != TIMER_RUNNING_NO_DISPLAY && _gameState == GAMESTATE_GAMEPLAY)
            {
                CapitalTextInfo timer_text_info = {0};
                timer_text_info.x = SCREEN_RIGHT_EDGE - 0x80;
                timer_text_info.y = SCREEN_BOTTOM_EDGE - 0xA;
                timer_text_info.size = DEFAULT_SIZE;
                DrawTextCapitals(mainTimerAscii, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);                
            }

            //Teehee
            if(_dragonState == 2){
                Timer ilTimer;
                ilTimer.timer = _globalTimer - ilTimerStart;
                FramesToTimer(&ilTimer);
                LoadAscii(&ilTimer, ilAscii);
            }
            else if(_dragonState > 2 && _dragonState < 7){
                CapitalTextInfo timer_text_info = {0};
                timer_text_info.x = SCREEN_RIGHT_EDGE - 0x40;
                timer_text_info.y = SCREEN_TOP_EDGE + 0x20;
                timer_text_info.size = DEFAULT_SIZE;
                char temp[3];
                sprintf(temp, "%X", _dragonWalkTime);
                DrawTextCapitals(temp, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                timer_text_info.x = SCREEN_LEFT_EDGE + 0x20;
                DrawTextCapitals(ilAscii, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                MobyRender();
            }
        }
        
        //! IL Timer Stuffs
        {
            if(il_menu.il_state)
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
                    if(il_menu.dont_loop_level == false)
                    {
                        if((_portalToExitFromInHW && _portalNumber == -1))
                        {
                            _levelID = _portalToExitFromInHW;
                            _portalToExitFromInHW = 0;
                            _flyInAnimation = flyInArray[_levelIDIndex];
                        }
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
        {
            _spyro.isMovementLocked = FALSE;
            current_menu = MAIN_MENU;
        }
    }

    // When Displaying Menu
    if(menu_state == MENU_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
    {
        if (current_menu == MAIN_MENU)
        {       
            CapitalTextInfo menu_text_info[4] = {{0}};

            // Easy Exit
            if(_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                menu_state = MENU_HIDDEN;
                _spyro.isMovementLocked = FALSE;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            DrawTextBox(0x30, 0x1D0, 0x30, 0x98);
            
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

            if(custom_menu.selection == 0)
            {
                DrawTextCapitals(custom_menu.il_menu_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(custom_menu.il_menu_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            
            if(custom_menu.selection == 1)
            {
                DrawTextCapitals(custom_menu.timer_menu_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(custom_menu.timer_menu_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if(custom_menu.selection == 2)
            {
                DrawTextCapitals(custom_menu.savestate_menu_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(custom_menu.savestate_menu_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if(custom_menu.selection == 3)
            {
                DrawTextCapitals(custom_menu.misc_menu_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(custom_menu.misc_menu_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }


            // Fill text with defaults if NULL
            if(custom_menu.il_menu_text == NULL)
            {
                custom_menu.il_menu_text = "IL SETTINGS";
                custom_menu.timer_menu_text = "MANUAL TIMER SETTINGS";
                custom_menu.savestate_menu_text = "SAVESTATE SETTINGS";
                custom_menu.misc_menu_text = "MISC SETTINGS";
            }

            // Change Selection
            if(_currentButtonOneFrame == DOWN_BUTTON)
            {
                custom_menu.selection = (custom_menu.selection + 1) % 4;
            }
            else if(_currentButtonOneFrame == UP_BUTTON && custom_menu.selection != 0)
            {
                custom_menu.selection = custom_menu.selection - 1;
            }
            
            // Play Sound Effect
            if(_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == X_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Timer Selection
            if(custom_menu.selection == 0)
            {
                if(_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = IL_MENU;
                }
            }
            // IL Looping Selection
            else if(custom_menu.selection == 1)
            {
                if(_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = TIMER_MENU;
                }
            }

            // Savestate Selection
            else if(custom_menu.selection == 2)
            {
                if(_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = SAVESTATE_MENU;
                }
            }

            // Sparx Selection
            else if(custom_menu.selection == 3)
            {
                if(_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = MISC_MENU;
                }
            }

        }

        if (current_menu == IL_MENU)
        {       
            CapitalTextInfo menu_text_info[5] = {{0}};

            // Easy Exit
            if(_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                current_menu = MAIN_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            DrawTextBox(0x30, 0x1F0, 0x30, 0xA8);
            
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

            if(il_menu.selection == 0)
            {
                DrawTextCapitals(il_menu.il_mode_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(il_menu.il_mode_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            
            if (il_menu.il_state != true)
            {
                DrawTextCapitals(il_menu.il_timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if(il_menu.selection == 1)
            {
                DrawTextCapitals(il_menu.il_timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_menu.il_timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            

            if (il_menu.il_state != true)
            {
                DrawTextCapitals(il_menu.display_on_dragon_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if(il_menu.selection == 2)
            {
                DrawTextCapitals(il_menu.display_on_dragon_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(il_menu.display_on_dragon_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (il_menu.il_state != true)
            {
                DrawTextCapitals(il_menu.display_on_land_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if(il_menu.selection == 3)
            {
                DrawTextCapitals(il_menu.display_on_land_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(il_menu.display_on_land_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            
            if (il_menu.il_state != true)
            {
                DrawTextCapitals(il_menu.loop_level_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if(il_menu.selection == 4)
            {
                DrawTextCapitals(il_menu.loop_level_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(il_menu.loop_level_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }


            // Fill text with defaults if NULL
            if(il_menu.il_mode_text == NULL)
            {
                il_menu.il_mode_text = "IL MODE OFF";
                il_menu.il_timer_display_mode_text = "IL TIMER DISPLAY AT END";
                il_menu.display_on_dragon_text = "DISPLAY AT DRAGON OFF";
                il_menu.display_on_land_text = "DISPLAY LANDING OFF";
                il_menu.loop_level_text = "LOOP LEVEL ON";
            }

            // Change Selection
            if(_currentButtonOneFrame == DOWN_BUTTON && il_menu.il_state == true)
            {
                il_menu.selection = (il_menu.selection + 1) % 5;
            }
            else if(_currentButtonOneFrame == UP_BUTTON && il_menu.selection != 0)
            {
                il_menu.selection = il_menu.selection - 1;
            }
            
            // Play Sound Effect
            if(_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Timer Selection
            if(il_menu.selection == 0)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_menu.il_state = (il_menu.il_state + 1) % 2;
                }

                if(il_menu.il_state == FALSE)
                {
                    il_menu.il_mode_text = "IL MODE OFF";

                }
                else if(il_menu.il_state == TRUE)
                {
                    il_menu.il_mode_text = "IL MODE ON";

                }
            }
            // IL Looping Selection
            else if(il_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    il_menu.il_timer_display_mode = (il_menu.il_timer_display_mode + 1) % 3;
                }
                
                if (_currentButtonOneFrame == LEFT_BUTTON && il_menu.il_timer_display_mode > 0)
                {
                    il_menu.il_timer_display_mode--;
                }

                if(il_menu.il_timer_display_mode == IL_TIMER_AT_END)
                {
                    il_menu.il_timer_display_mode_text = "IL TIMER DISPLAY AT END";

                }
                else if(il_menu.il_timer_display_mode == IL_TIMER_ALWAYS)
                {
                    il_menu.il_timer_display_mode_text = "IL TIMER DISPLAY ALWAYS ON";

                }
                else if(il_menu.il_timer_display_mode == IL_TIMER_OFF)
                {
                    il_menu.il_timer_display_mode_text = "IL TIMER DISPLAY OFF";

                }
            }


            else if(il_menu.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_menu.display_on_dragon = (il_menu.display_on_dragon + 1) % 3;
                }

                if(il_menu.display_on_dragon == TIMER_OFF)
                {
                    il_menu.display_on_dragon_text = "DISPLAY AT DRAGON OFF";

                }
                else if(il_menu.display_on_dragon == TIMER_ONLY_PAUSE)
                {
                    il_menu.display_on_dragon_text = "DISPLAY AT DRAGON ON";

                }
            }

            else if(il_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_menu.display_on_land = (il_menu.display_on_land + 1) % 3;
                }

                if(il_menu.display_on_land == TIMER_OFF)
                {
                    il_menu.display_on_land_text = "DISPLAY LANDING OFF";

                }
                else if(il_menu.display_on_land == TIMER_ONLY_PAUSE)
                {
                    il_menu.display_on_land_text = "DISPLAY LANDING ON";

                }
            }
            else if(il_menu.selection == 4)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_menu.dont_loop_level = (il_menu.dont_loop_level + 1) % 2;
                }

                if(il_menu.dont_loop_level == 0)
                {
                    il_menu.loop_level_text = "LOOP LEVEL ON";

                }
                else if(il_menu.dont_loop_level == 1)
                {
                    il_menu.loop_level_text = "LOOP LEVEL OFF";

                }
            }

        }

        if (current_menu == TIMER_MENU)
        {       
            CapitalTextInfo menu_text_info[4] = {{0}};

            // Easy Exit
            if(_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                current_menu = MAIN_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            DrawTextBox(0x30, 0x1D0, 0x30, 0x98);
            
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

            if(timer_menu.selection == 0)
            {
                DrawTextCapitals(timer_menu.timer_state_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(timer_menu.timer_state_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            

            if (timer_menu.timer_state != true)
            {
                DrawTextCapitals(timer_menu.timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if(timer_menu.selection == 1)
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
            else if(timer_menu.selection == 2)
            {
                DrawTextCapitals(timer_menu.stop_timer_button_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(timer_menu.stop_timer_button_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (timer_menu.timer_state != true)
            {
                DrawTextCapitals(timer_menu.reset_timer_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if(timer_menu.selection == 3)
            {
                DrawTextCapitals(timer_menu.reset_timer_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else{
                DrawTextCapitals(timer_menu.reset_timer_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }


            // Fill text with defaults if NULL
            if(timer_menu.timer_state_text == NULL)
            {
                timer_menu.timer_state_text = "MANUAL TIMER OFF";
                timer_menu.timer_display_mode_text = "DISPLAY ON STOP";
                timer_menu.stop_timer_button_text = "STOP BUTTON START";
                timer_menu.reset_timer_button_text = "RESET BUTTON R3";
            }

            // Change Selection
            if(_currentButtonOneFrame == DOWN_BUTTON && timer_menu.timer_state == true)
            {
                timer_menu.selection = (timer_menu.selection + 1) % 4;
            }
            else if(_currentButtonOneFrame == UP_BUTTON && timer_menu.selection != 0)
            {
                timer_menu.selection = timer_menu.selection - 1;
            }
            
            // Play Sound Effect
            if(_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Timer Selection
            if(timer_menu.selection == 0)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    timer_menu.timer_state = (timer_menu.timer_state + 1) % 2;
                }

                if(timer_menu.timer_state == FALSE)
                {
                    timer_menu.timer_state_text = "MANUAL TIMER OFF";

                }
                else if(timer_menu.timer_state == TRUE)
                {
                    timer_menu.timer_state_text = "MANUAL TIMER ON";

                }
            }
            // IL Looping Selection
            else if(timer_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    timer_menu.timer_display_mode = (timer_menu.timer_display_mode + 1) % 2;
                }

                if(timer_menu.timer_display_mode == IL_TIMER_AT_END)
                {
                    timer_menu.timer_display_mode_text = "DISPLAY ON STOP";

                }
                else if(timer_menu.timer_display_mode == IL_TIMER_ALWAYS)
                {
                    timer_menu.timer_display_mode_text = "DISPLAY ALWAYS";

                }
            }


            else if(timer_menu.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    timer_menu.stop_timer_button_index = (timer_menu.stop_timer_button_index + 1) % 2;
                }

                if(timer_menu.stop_timer_button_index == 0)
                {
                    timer_menu.stop_timer_button_text = "STOP BUTTON START";

                }
                else if(timer_menu.stop_timer_button_index == 1)
                {
                    timer_menu.stop_timer_button_text = "STOP BUTTON START";

                }
            }

            else if(timer_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    timer_menu.reset_timer_button_index = (timer_menu.reset_timer_button_index + 1) % 2;
                }

                if(timer_menu.reset_timer_button_index == 0)
                {
                    timer_menu.reset_timer_button_text = "RESET BUTTON R3";

                }
                else if(timer_menu.reset_timer_button_index == 1)
                {
                    timer_menu.reset_timer_button_text = "RESET BUTTON SELECT";

                }
            }

        }

    }
    
    // Has Released Menu Button
    if(!(_currentButton & L2_BUTTON + R2_BUTTON + TRIANGLE_BUTTON))
    {
        has_toggled_menu = FALSE;
    }

    // Has Released Button
    if(!(_currentButton & L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON))
    {
        isHeld = FALSE;
    }
    // Has Released Button
    if(!(_currentButton & L1_BUTTON + R1_BUTTON + TRIANGLE_BUTTON))
    {
        isHeld = FALSE;
    }

    //! Checks
    {
        if(misc_menu.sparx_mode == PERMA_SPARX_ON)
        {
            _spyro.health = 3;
        }
        else if(misc_menu.sparx_mode == SPARXLESS)
        {
            if (_spyro.health > 0)
            {
                _spyro.health = 0;
            }
        }

        if(misc_menu.quick_goop_mode == TRUE)
        {
            if(_spyro.drownTimer > 100)
            {
                _spyro.drownTimer = 0x240;
            }
        }
    }

    if(((timer_menu.timer_display_mode != TIMER_OFF || menu_state == MENU_DISPLAYING) && _gameState == GAMESTATE_GAMEPLAY) || (il_timer_state == IL_DISPLAYING && _gameState == GAMESTATE_LOADING))
    {
        //printf("RENDERING\n");
        MobyRender();
    }
}