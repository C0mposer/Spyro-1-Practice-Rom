#include <common.h>
#include <custom_text.h>
#include <sound.h>
#include <shared_funcs.h>
#include <cosmetic.h>
#include <igt.h>

typedef enum TimerState
{
    TIMER_RUNNING,
    TIMER_DISPLAYING,

}TimerState;

typedef enum TimerDisplayMode
{
    TIMER_ONLY_PAUSE,
    TIMER_ALWAYS

}TimerDisplayMode;

typedef struct Timer
{
    int timer;
    int secondsOnesPlace;
    int secondsTensPlace;
    int milisecondsTenthsPlace;
    int milisecondsHundrethsPlace;
    int minutes;

}Timer;

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
TimerState timerState = TIMER_RUNNING;
Timer mainTimer;
int mainTimerAtReset;
char mainTimerAscii[10];
extern bool isHeld;

extern int savestate_button_index;
extern int loadstate_button_index;
extern int switch_state_button_index;

extern const short STOP_TIMER_BUTTONS[2];

extern const short SAVESTATE_BUTTONS[2];
extern const short LOADSTATE_BUTTONS[3];

extern bool hasUpdatedPortalTimer;

//Math for 29.91hz
void FramesToTimer(Timer* ptr_timer)
{
    ptr_timer->minutes = (ptr_timer->timer * 10) / 35892;
    ptr_timer->secondsTensPlace = ((ptr_timer->timer * 10) % 35892) / 5982;
    ptr_timer->secondsOnesPlace = ((ptr_timer->timer * 100) % 59820) / 5982;
    ptr_timer->milisecondsTenthsPlace = ((ptr_timer->timer * 1000) % 59820) / 5982;
    ptr_timer->milisecondsHundrethsPlace = ((ptr_timer->timer * 10000) % 59820) / 5982;
}

void LoadAscii(Timer* ptr_timer, char* ascii) {
    if (ptr_timer->minutes == 0) {
        sprintf(ascii, "%d%d.%d%d", ptr_timer->secondsTensPlace, ptr_timer->secondsOnesPlace, ptr_timer->milisecondsTenthsPlace, ptr_timer->milisecondsHundrethsPlace);
    }
    else if (ptr_timer->minutes >= 1) {
        sprintf(ascii, "%d.%d%d.%d%d", ptr_timer->minutes, ptr_timer->secondsTensPlace, ptr_timer->secondsOnesPlace, ptr_timer->milisecondsTenthsPlace, ptr_timer->milisecondsHundrethsPlace);
    }
}

//! Every Frame Update
void TimerUpdate() {

    if (timer_menu.timer_state)
    {
        //Main Timer Checks/Loop

        //Button Checks
        if (_currentButtonOneFrame == LOADSTATE_BUTTONS[loadstate_button_index])
        {
            mainTimerAtReset = _globalTimer;
            timerState = TIMER_RUNNING;
        }
        if (_currentButton == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON && !isHeld && timer_menu.reset_timer_mode == 0)
        {
            mainTimerAtReset = _globalTimer;  //Resets timer to 0 by syncing up to the global timer
            timerState = TIMER_RUNNING;
            isHeld = true;
        }

        //Show the running timer
        if (timerState == TIMER_RUNNING)
        {
            if (_currentButtonOneFrame == STOP_TIMER_BUTTONS[timer_menu.stop_timer_button_index] || timer_menu.timer_display_mode == TIMER_ALWAYS || !hasUpdatedPortalTimer) {
                mainTimer.timer = _globalTimer - mainTimerAtReset;
                FramesToTimer(&mainTimer);
                LoadAscii(&mainTimer, mainTimerAscii);
            }
        }

        if (_currentButtonOneFrame == STOP_TIMER_BUTTONS[timer_menu.stop_timer_button_index] || !hasUpdatedPortalTimer)
        {
            timerState = TIMER_DISPLAYING;
            hasUpdatedPortalTimer = true;
        }

        //Show the saved timer
        if ((timerState == TIMER_DISPLAYING || timer_menu.timer_display_mode == TIMER_ALWAYS) && _gameState == GAMESTATE_GAMEPLAY)
        {
            CapitalTextInfo timer_text_info = { 0 };
            if (mainTimer.minutes == 0)
            {
                timer_text_info.x = SCREEN_RIGHT_EDGE - 0x5A;
                timer_text_info.y = SCREEN_BOTTOM_EDGE - 0xA;
                timer_text_info.size = DEFAULT_SIZE;
            }
            else
            {
                timer_text_info.x = SCREEN_RIGHT_EDGE - 0x7D;
                timer_text_info.y = SCREEN_BOTTOM_EDGE - 0xA;
                timer_text_info.size = DEFAULT_SIZE;
            }
            DrawTextCapitals(mainTimerAscii, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_BLUE);
        }
    }
}