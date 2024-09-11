#include <common.h>

bool hasUpdatedPortalTimer = true;  // bool used in manual_timer.c to pause the timer

extern int levelSelectState;
extern int timerState;
extern bool disable_portal_entry;

SkipPortalEntry()
{
    if(_levelLoadState || levelSelectState || (_levelIDPortalExit % 10) != 0 || !disable_portal_entry)
    {          
        LevelTransition();
    }
    else if (disable_portal_entry)
    {
        _gameState = GAMESTATE_GAMEPLAY;
        _levelLoadState = -1;
        _isLoading = 0;
        _levelID = _levelIDPortalExit;
        _canFlyIn = 0;
        hasUpdatedPortalTimer = false;
        

        #if BUILD == 2 || BUILD == 0
            LoadStateTest();
        #elif BUILD == 1 || BUILD == 3
            ReloadSpyroAndCamera(false);
        #endif
    }
}