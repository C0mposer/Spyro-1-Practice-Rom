#include <common.h>

extern int levelSelectState;
extern int timerState;
bool hasUpdatedPortalTimer = TRUE;

SkipPortalEntry(){
    if(_levelLoadState || levelSelectState || (_levelIDPortalExit % 10)){           //ADD MENU BOOL TO THIS STATEMENT, IF SHOULD BE TRUE BY DEFAULT
        LevelTransitionCalls();
    }
    else{
        _gameState = GAMESTATE_GAMEPLAY;
        _levelLoadState = -1;
        _isLoading = 0;
        _levelID = _levelIDPortalExit;
        _canFlyIn = 0;
        hasUpdatedPortalTimer = FALSE;
        

        #if BUILD == 2 || BUILD == 0
            LoadStateTest();
        #elif BUILD == 1 || BUILD == 3
            ReloadSpyroAndCamera(false);
        #endif
    }
}