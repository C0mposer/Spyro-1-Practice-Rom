#include <common.h>
#include <custom_text.h>
#include <sound.h>
#include <shared_funcs.h>
#include <cosmetic.h>
#include <igt.h>
#include <moby.h>
#include <draw_hud_moby.h>
#include <landing_timer.h>

typedef enum ILTimerState
{
    IL_FLYING_IN,
    IL_STARTED,
    IL_DISPLAYING,
    IL_STOPPED

}ILTimerState;

typedef enum ILTimerDisplayMode
{
    IL_TIMER_AT_END,
    IL_TIMER_ALWAYS

}ILTimerDisplayMode;

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

typedef struct Timer
{
    int timer;
    int secondsOnesPlace;
    int secondsTensPlace;
    int milisecondsTenthsPlace;
    int milisecondsHundrethsPlace;
    int minutes;

}Timer;

ILTimerState il_timer_state = IL_STOPPED;
int ilTimerStart = 0;
int framesSpentLoading = 0;
char ilAscii[10];
char loadlessAscii[10];

extern ILMenu il_menu;

extern const short flyInArray[36];

extern int savestate_button_index;
extern int loadstate_button_index;

extern const short SAVESTATE_BUTTONS[2];
extern const short LOADSTATE_BUTTONS[3];

extern savestate_selection;

int il_timer_offset[3] = {0};




void ILUpdate(){
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
        else if(il_timer_state == IL_STARTED){
            if(_dragonState == 2 || _dragonState == 6){ //State 2 is after spyro has finished walking but the cd load is still going and state 6 is for the cd load after the dragon cut scene
                framesSpentLoading++;
            }
            if(_currentButtonOneFrame == SAVESTATE_BUTTONS[savestate_button_index] && _gameState == GAMESTATE_GAMEPLAY){
                il_timer_offset[savestate_selection] = _globalTimer - ilTimerStart;
            }
            if(_currentButtonOneFrame == LOADSTATE_BUTTONS[loadstate_button_index] && _gameState == GAMESTATE_GAMEPLAY){
                ilTimerStart = _globalTimer - il_timer_offset[savestate_selection];
            }
            if(il_menu.il_timer_display_mode == IL_TIMER_ALWAYS && _gameState == GAMESTATE_GAMEPLAY){
                Timer ilTimer;
                ilTimer.timer = _globalTimer - ilTimerStart;
                FramesToTimer(&ilTimer);
                LoadAscii(&ilTimer, ilAscii);
                CapitalTextInfo timer_text_info = {0};
                timer_text_info.x = SCREEN_LEFT_EDGE + 0x10;
                timer_text_info.y = SCREEN_BOTTOM_EDGE - 0xA;
                timer_text_info.size = DEFAULT_SIZE;
                DrawTextCapitals(ilAscii, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
            if(_gameState == GAMESTATE_LOADING){
                Timer ilTimer;
                ilTimer.timer = _globalTimer - ilTimerStart;
                FramesToTimer(&ilTimer);
                LoadAscii(&ilTimer, ilAscii);

                ilTimer.timer = ilTimer.timer - (2 * framesSpentLoading);
                FramesToTimer(&ilTimer);
                LoadAscii(&ilTimer, loadlessAscii);

                il_timer_state = IL_DISPLAYING;
            }
        }

        // IL DRAGON CHECKPOINT TIMES
        if(il_menu.display_on_dragon){
            if(_dragonState == 2){
                Timer ilTimer;
                ilTimer.timer = _globalTimer - ilTimerStart;
                FramesToTimer(&ilTimer);
                LoadAscii(&ilTimer, ilAscii);
            }
            else if(_dragonState > 2 && _dragonState < 7){
                CapitalTextInfo timer_text_info = {0};
                timer_text_info.x = SCREEN_LEFT_EDGE + 0x26;
                timer_text_info.y = SCREEN_TOP_EDGE + 0x20;
                timer_text_info.size = DEFAULT_SIZE;
                DrawTextCapitals(ilAscii, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_GREEN);

                MyHudMoby dragon_logo = {.position.x = SCREEN_LEFT_EDGE + 0x12, .position.y = SCREEN_TOP_EDGE + 0x27, .position.z = 3900};
                CustomDrawMoby(MOBY_ID_DRAGON_FIGURINE, &dragon_logo, MOBY_COLOR_GREEN);
                
            }
        }

        //IL LANDING CHECKPOINT TIMES
        if(il_menu.display_on_land && il_menu.il_timer_display_mode != IL_TIMER_ALWAYS){
            if(ShouldSaveLandingTime()){
                Timer ilTimer;
                ilTimer.timer = _globalTimer - ilTimerStart;
                FramesToTimer(&ilTimer);
                LoadAscii(&ilTimer, ilAscii);
            }
            if(ShouldDisplayLandingTime() && _gameState == GAMESTATE_GAMEPLAY)
            {
                CapitalTextInfo timer_text_info = {0};
                timer_text_info.x = SCREEN_LEFT_EDGE + 0x10;
                timer_text_info.y = SCREEN_BOTTOM_EDGE - 0xA;
                timer_text_info.size = DEFAULT_SIZE;
                DrawTextCapitals(ilAscii, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_PURPLE);   
            }
        }

        // IL LOOPING //!TEST THIS, SHOULD WORK
        if(il_menu.dont_loop_level == false)
        {
            if((_portalToExitFromInHW && _portalNumber == -1))
            {
                _levelID = _portalToExitFromInHW;
                _portalToExitFromInHW = 0;
                _flyInAnimation = flyInArray[_levelIDIndex];
                
                _spyro.health = 3;
            }
        }
        
        //DISPLAY
        if((il_timer_state == IL_DISPLAYING))
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