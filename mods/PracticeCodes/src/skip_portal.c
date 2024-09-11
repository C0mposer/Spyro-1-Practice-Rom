#include <common.h>
extern int levelSelectState;
bool shouldContinue;

SkipPortalEntry(){
    if(_levelLoadState || levelSelectState || (_levelIDPortalExit % 10)){
        LevelTransitionCalls();
    }
    else{
        _gameState = GAMESTATE_GAMEPLAY;
        _levelLoadState = -1;
        _isLoading = 0;
        _levelID = _levelIDPortalExit;
        ReloadSpyroAndCamera(0);
    }
}