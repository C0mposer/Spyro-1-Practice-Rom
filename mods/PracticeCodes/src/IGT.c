#include <common.h>
#include <custom_text.h>
#include <sound.h>
#include <bg_colors.h>

#define BASELINE 526
#define TIMER_TO_FRAME 17   //17*30 is 526

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


typedef struct Menu
{
    int selection;
    TimerMode timer_mode;
    char* timer_mode_text;
    FPSMode fps_mode;
    char* fps_mode_text;
    bool il_loop_mode;
    char* il_mode_text;
    bool sparx_mode;
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
    int displaySeconds;
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

Timer mainTimer = {0};
TimerState timer_state = TIMER_STOPPED;

Timer ilTimer = {0};
TimerState il_timer_state = TIMER_STOPPED;
int framesSpentLoading = 0;
char loadlessAscii[10];
extern short flyInArray[36];

Menu custom_menu = {0};
MenuState menu_state = MENU_HIDDEN;

BackgroundColor bg_color_index;
bool should_update_bg_color = TRUE;

FPS_t fps_data = {0};

CapitalTextInfo timer_text_info = {0};
CapitalTextInfo fps_text_info = {0};
CapitalTextInfo menu_text_info[6] = {{0}};

void InGameTimerHook()
{
   //! Timer & FPS
    {

        if(timer_state != TIMER_STOPPED)
        {
            //Main Timer Checks/Loop

            //Button Checks
            if(_currentButtonOneFrame == R3_BUTTON)
            {   
                mainTimer.timer_at_reset = _globalTimer;
                mainTimer.timer = 0;
                timer_state = custom_menu.timer_mode;
            }

            if(_currentButtonOneFrame == START_BUTTON)
            {
                timer_state = TIMER_DISPLAYING;
            }
            

            //Show the running timer
            if(timer_state == TIMER_RUNNING || timer_state == TIMER_RUNNING_NO_DISPLAY)
            {
                //Math for 29.91hz
                mainTimer.timer = _globalTimer - mainTimer.timer_at_reset;
                mainTimer.minutes = (mainTimer.timer * 10) / 35892;
                mainTimer.secondsTensPlace = ((mainTimer.timer * 10) % 35892) / 5982;
                mainTimer.secondsOnesPlace = ((mainTimer.timer * 100) % 59820) / 5982;
                mainTimer.milisecondsTenthsPlace = ((mainTimer.timer * 1000) % 59820) / 5982;
                mainTimer.milisecondsHundrethsPlace = ((mainTimer.timer * 10000) % 59820) / 5982;

                //Seconds
                if(mainTimer.minutes == 0)
                {
                    sprintf(mainTimer.ascii, "%d%d.%d%d", mainTimer.secondsTensPlace, mainTimer.secondsOnesPlace, mainTimer.milisecondsTenthsPlace, mainTimer.milisecondsHundrethsPlace);
                }

                //Minutes
                else if(mainTimer.minutes >= 1)
                {
                    sprintf(mainTimer.ascii, "%d.%d%d.%d%d", mainTimer.minutes, mainTimer.secondsTensPlace, mainTimer.secondsOnesPlace, mainTimer.milisecondsTenthsPlace, mainTimer.milisecondsHundrethsPlace);
                }

                timer_text_info.x = SCREEN_RIGHT_EDGE - 0x80;
                timer_text_info.y = SCREEN_BOTTOM_EDGE - 0xA;
                timer_text_info.size = DEFAULT_SIZE;

                if(timer_state == TIMER_RUNNING)
                    DrawTextCapitals(mainTimer.ascii, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);

            }

            //Show the saved timer
            if(timer_state == TIMER_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
            {
                timer_text_info.x = SCREEN_RIGHT_EDGE - 0x80;
                timer_text_info.y = SCREEN_BOTTOM_EDGE - 0xA;
                timer_text_info.size = DEFAULT_SIZE;
                DrawTextCapitals(mainTimer.ascii, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);                
            }

        }
        
        //! IL Timer Stuffs
        {
            if(il_timer_state == IL_STOPPED && (_gameState == GAMESTATE_FLY_IN || _gameState == GAMESTATE_LOADING)){
                il_timer_state = IL_FLYING_IN;
            }
            else if(il_timer_state == IL_FLYING_IN && _gameState == GAMESTATE_GAMEPLAY){
                il_timer_state = IL_STARTED;
                ilTimer.timer_at_reset = _levelTimer_60fps;
                framesSpentLoading = 0;
            }
            else if(il_timer_state == IL_STARTED && (_dragonState == 2 || _dragonState == 6)){ //state 2 is after spyro has finished walking but the cd load is still going and state 6 is for the cd load after the dragon cut scene
                framesSpentLoading++;
            }
            else if(il_timer_state == IL_STARTED && _gameState == GAMESTATE_LOADING){
                ilTimer.timer = _globalTimer - ilTimer.timer_at_reset;
                ilTimer.minutes = (ilTimer.timer * 10) / 35892;
                ilTimer.secondsTensPlace = ((ilTimer.timer * 10) % 35892) / 5982;
                ilTimer.secondsOnesPlace = ((ilTimer.timer * 100) % 59820) / 5982;
                ilTimer.milisecondsTenthsPlace = ((ilTimer.timer * 1000) % 59820) / 5982;
                ilTimer.milisecondsHundrethsPlace = ((ilTimer.timer * 10000) % 59820) / 5982;

                sprintf(ilTimer.ascii, "%d.%d%d.%d%d\n", ilTimer.minutes, ilTimer.secondsTensPlace, ilTimer.secondsTensPlace, ilTimer.milisecondsTenthsPlace, ilTimer.milisecondsHundrethsPlace);

                ilTimer.timer = ilTimer.timer - (2 * framesSpentLoading);
                ilTimer.minutes = (ilTimer.timer * 10) / 35892;
                ilTimer.secondsTensPlace = ((ilTimer.timer * 10) % 35892) / 5982;
                ilTimer.secondsOnesPlace = ((ilTimer.timer * 100) % 59820) / 5982;
                ilTimer.milisecondsTenthsPlace = ((ilTimer.timer * 1000) % 59820) / 5982;
                ilTimer.milisecondsHundrethsPlace = ((ilTimer.timer * 10000) % 59820) / 5982;

                sprintf(loadlessAscii, "%d.%d%d.%d%d\n", ilTimer.minutes, ilTimer.secondsTensPlace, ilTimer.secondsTensPlace, ilTimer.milisecondsTenthsPlace, ilTimer.milisecondsHundrethsPlace);

                il_timer_state = IL_DISPLAYING;

                //! IL Looping
                if((custom_menu.il_loop_mode == TRUE && _spyro.timer_framesInAir != 1 && _portalNumber == -1)){
                    _levelID = _portalToExitFromInHW;
                    _portalToExitFromInHW = 0;
                    _flyInAnimation = flyInArray[_levelIDIndex];
                }
            }
            
            //DISPLAY
            if(il_timer_state == IL_DISPLAYING){
                
                //Do display shit here :)
                //Both times on the left of the screen
                //Regular time larger and then loadless time below it slightly smaller
                
                //!We ran out of space by 400 bytes L boser lol -Composer

                if(_levelLoadState >= 0xB){
                    il_timer_state = IL_STOPPED;
                }
            }
        }
    
        //! Show the FPS
        if(custom_menu.fps_mode != FPS_OFF)
        {
            char buffer[10] = {0};
            fps_data.difference = _hBlankTimer - fps_data.compareTimer; 
            fps_data.compareTimer = _hBlankTimer;
            fps_data.difference_from_baseline = fps_data.difference - BASELINE;
            fps_data.fps = 30 - (fps_data.difference_from_baseline / TIMER_TO_FRAME / 2);
                
            sprintf(buffer, "FPS %d", fps_data.fps);
                
            if(fps_data.fps < 100)
            {
                if(custom_menu.fps_mode == FPS_ALWAYS || (custom_menu.fps_mode == FPS_ONLY_DROPPED && fps_data.fps <= 29))
                {
                    fps_text_info.x = SCREEN_LEFT_EDGE + 0x10;
                    fps_text_info.y = 20;
                    fps_text_info.size = DEFAULT_SIZE;
                    DrawTextCapitals(buffer, &fps_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                }
            }
            
        }
    }

    //! Menu
    {
        // Open Menu
        if(_currentButton == L2_BUTTON + R2_BUTTON + TRIANGLE_BUTTON && !has_toggled_menu)
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
            // Easy Exit
            if(_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                menu_state = MENU_HIDDEN;
                _spyro.isMovementLocked = FALSE;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            char buffer[6][20] = {0};

            _spyro.isMovementLocked = TRUE;

            DrawTextBox(0x30, 0x1D0, 0x30, 0xC0);

            menu_text_info[0].x = SCREEN_LEFT_EDGE + 0x50;
            menu_text_info[0].y = 70;
            menu_text_info[0].size = DEFAULT_SIZE;

            menu_text_info[1].x = SCREEN_LEFT_EDGE + 0x50;
            menu_text_info[1].y = 90;
            menu_text_info[1].size = DEFAULT_SIZE;

            menu_text_info[2].x = SCREEN_LEFT_EDGE + 0x50;
            menu_text_info[2].y = 110;
            menu_text_info[2].size = DEFAULT_SIZE;

            menu_text_info[3].x = SCREEN_LEFT_EDGE + 0x50;
            menu_text_info[3].y = 130;
            menu_text_info[3].size = DEFAULT_SIZE;
            
            menu_text_info[4].x = SCREEN_LEFT_EDGE + 0x50;
            menu_text_info[4].y = 150;
            menu_text_info[4].size = DEFAULT_SIZE;

            menu_text_info[5].x = SCREEN_LEFT_EDGE + 0x50;
            menu_text_info[5].y = 170;
            menu_text_info[5].size = DEFAULT_SIZE;


            sprintf(&buffer, "%s", custom_menu.timer_mode_text);
            sprintf(&buffer[1], "%s", custom_menu.fps_mode_text);
            sprintf(&buffer[2], "%s", custom_menu.il_mode_text);
            sprintf(&buffer[3], "%s", custom_menu.sparx_mode_text);
            sprintf(&buffer[4], "%s", custom_menu.quick_goop_text);
            sprintf(&buffer[5], "%s", custom_menu.bg_color_text);

            //Fix later?
            if(custom_menu.selection == 0)
            {
                DrawTextCapitals(buffer, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
                DrawTextCapitals(buffer[1], &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[2], &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[3], &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[4], &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[5], &menu_text_info[5], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            else if(custom_menu.selection == 1)
            {
                DrawTextCapitals(buffer, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[1], &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
                DrawTextCapitals(buffer[2], &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[3], &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[4], &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[5], &menu_text_info[5], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            else if(custom_menu.selection == 2)
            {
                DrawTextCapitals(buffer, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[1], &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[2], &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
                DrawTextCapitals(buffer[3], &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[4], &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[5], &menu_text_info[5], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            else if(custom_menu.selection == 3)
            {
                DrawTextCapitals(buffer, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[1], &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[2], &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[3], &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
                DrawTextCapitals(buffer[4], &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[5], &menu_text_info[5], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            else if(custom_menu.selection == 4)
            {
                DrawTextCapitals(buffer, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[1], &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[2], &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[3], &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[4], &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_GOLD);
                DrawTextCapitals(buffer[5], &menu_text_info[5], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            else if(custom_menu.selection == 5)
            {
                DrawTextCapitals(buffer, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[1], &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[2], &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[3], &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[4], &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
                DrawTextCapitals(buffer[5], &menu_text_info[5], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }

            // Fill text with defaults if NULL
            if(custom_menu.fps_mode_text == NULL)
            {
                custom_menu.fps_mode_text = "FPS OFF";
                custom_menu.il_mode_text = "IL LOOPING OFF";
                custom_menu.sparx_mode_text = "PERMA SPARX OFF";
                custom_menu.quick_goop_text = "QUICK GOOP OFF";
                custom_menu.bg_color_text = "BG PINK";
            }

            // Change Selection
            if(_currentButtonOneFrame == DOWN_BUTTON)
            {
                custom_menu.selection = (custom_menu.selection + 1) % 6;
            }
            if(_currentButtonOneFrame == UP_BUTTON && custom_menu.selection != 0)
            {
                custom_menu.selection = (custom_menu.selection - 1) % 6;
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
                    mainTimer.timer_at_reset = _globalTimer;
                    mainTimer.timer = 0;
                }
                if(_currentButtonOneFrame == LEFT_BUTTON)
                {
                    custom_menu.timer_mode = (custom_menu.timer_mode - 1) % 3;
                    mainTimer.timer_at_reset = _globalTimer;
                    mainTimer.timer = 0;
                }

                if(custom_menu.timer_mode == TIMER_OFF)
                {
                    custom_menu.timer_mode_text = "TIMER OFF";
                    timer_state = TIMER_OFF;
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

            // FPS Selection
            else if(custom_menu.selection == 1)
            {

                if(_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    custom_menu.fps_mode = (custom_menu.fps_mode + 1) % 3;
                }
                if(_currentButtonOneFrame == LEFT_BUTTON)
                {
                    custom_menu.fps_mode = (custom_menu.fps_mode - 1) % 3;
                }

                if(custom_menu.fps_mode == FPS_OFF)
                {
                    custom_menu.fps_mode_text = "FPS OFF";

                }
                if(custom_menu.fps_mode == FPS_ONLY_DROPPED)
                {
                    custom_menu.fps_mode_text = "FPS ONLY DROPS";

                }
                if(custom_menu.fps_mode == FPS_ALWAYS)
                {
                    custom_menu.fps_mode_text = "FPS ALWAYS";

                }

            }

            // IL Looping Selection
            else if(custom_menu.selection == 2)
            {

                if(_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    custom_menu.il_loop_mode = (custom_menu.il_loop_mode + 1) % 2;
                }
                if(_currentButtonOneFrame == LEFT_BUTTON)
                {
                    custom_menu.il_loop_mode = (custom_menu.il_loop_mode - 1) % 2;
                }

                if(custom_menu.il_loop_mode == FALSE)
                {
                    custom_menu.il_mode_text = "IL LOOPING OFF";

                }
                if(custom_menu.il_loop_mode == TRUE)
                {
                    custom_menu.il_mode_text = "IL LOOPING ON";

                }

            }

            // Sparx Selection
            else if(custom_menu.selection == 3)
            {
                if(_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    custom_menu.sparx_mode = (custom_menu.sparx_mode + 1) % 2;
                }
                if(_currentButtonOneFrame == LEFT_BUTTON)
                {
                    custom_menu.sparx_mode = (custom_menu.sparx_mode - 1) % 2;
                }

                if(custom_menu.sparx_mode == FALSE)
                {
                    custom_menu.sparx_mode_text = "PERMA SPARX OFF";

                }
                if(custom_menu.sparx_mode == TRUE)
                {
                    custom_menu.sparx_mode_text = "PERMA SPARX ON";

                }
            }

            // Quick Goop Selection
            else if(custom_menu.selection == 4)
            {
                if(_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    custom_menu.quick_goop_mode = (custom_menu.quick_goop_mode + 1) % 2;
                }
                if(_currentButtonOneFrame == LEFT_BUTTON)
                {
                    custom_menu.quick_goop_mode = (custom_menu.quick_goop_mode - 1) % 2;
                }

                if(custom_menu.quick_goop_mode == FALSE)
                {
                    custom_menu.quick_goop_text = "QUICK GOOP OFF";

                }
                if(custom_menu.quick_goop_mode == TRUE)
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
                if(_currentButtonOneFrame == LEFT_BUTTON && bg_color_index > 0)
                {
                    bg_color_index = (bg_color_index - 1) % 6;
                    should_update_bg_color = TRUE;
                }

                if(bg_color_index == BG_PINK)
                {
                    custom_menu.bg_color_text = "BG PINK";

                }
                if(bg_color_index == BG_YELLOW)
                {
                    custom_menu.bg_color_text = "BG YELLOW";

                }
                if(bg_color_index == BG_TEAL)
                {
                    custom_menu.bg_color_text = "BG TEAL";

                }
                if(bg_color_index == BG_SALMON)
                {
                    custom_menu.bg_color_text = "BG SALMON";

                }
                if(bg_color_index == BG_PURPLE)
                {
                    custom_menu.bg_color_text = "BG PURPLE";

                }
                if(bg_color_index == BG_BLUE)
                {
                    custom_menu.bg_color_text = "BG BLUE";

                }
            }
        }

        // Has Released Menu Button
        if(_currentButton != L2_BUTTON + R2_BUTTON + TRIANGLE_BUTTON)
        {
            has_toggled_menu = FALSE;
        }
    }

    //! Perma Sparx and Quick Goop
    {
        if(custom_menu.sparx_mode == TRUE)
        {
            _spyro.health = 3;
        }

        if(custom_menu.quick_goop_mode == TRUE)
        {
            if(_spyro.drownTimer > 100)
            {
                _spyro.drownTimer = 0x240;
            }
        }
    }

        // //Render the HUD Text
    if(((custom_menu.fps_mode != FPS_OFF || custom_menu.timer_mode != TIMER_OFF || menu_state == MENU_DISPLAYING) && _gameState == GAMESTATE_GAMEPLAY) || (il_timer_state == IL_DISPLAYING && _gameState == GAMESTATE_LOADING))
    {
        //printf("RENDERING\n");
        MobyRender();
    }

         printf("Timer: %u\n", _hBlankTimer);
         printf("FPS : %u\n", fps_data.fps);
         printf("Comparison from Last Frame : %u\n\n", fps_data.difference);
         printf("Comparison from Baseline : %u\n\n", fps_data.difference_from_baseline);
        // printf("X: %d Y: %d\n", menu_text_info[0].x, menu_text_info[0].y);
       

}