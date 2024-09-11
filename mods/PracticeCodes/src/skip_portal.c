#include <common.h>
extern int levelSelectState;

SkipPortalEntry(){
    if(_levelLoadState || levelSelectState || (_levelIDPortalExit % 10)){
        LevelTransitionCalls();
    }
    else{
        _gameState = GAMESTATE_GAMEPLAY;
        _levelLoadState = -1;
        _isLoading = 0;
        _levelID = _levelIDPortalExit;
        _canFlyIn = 0;

        #if BUILD == 2 || BUILD == 0
            LoadStateTest();
        #elif BUILD == 1 || BUILD == 3
            ReloadSpyroAndCamera(false);
        #endif
    }
}