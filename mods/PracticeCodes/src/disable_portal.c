#include <common.h>

bool hasUpdatedPortalTimer = true;  // bool used in manual_timer.c to pause the timer

extern int levelSelectState;
extern int timerState;
extern bool disable_portal_entry;
extern int savestated_level_ids[3];
extern int savestate_selection;

void DisablePortalEntry(void)
{
    if (_levelLoadState || levelSelectState || (_levelIDPortalExit % 10) != 0 || !disable_portal_entry)
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

        bool does_savestate_already_exist_in_hw = savestated_level_ids[savestate_selection] == _levelID;

        if (does_savestate_already_exist_in_hw == false)
        {
            RespawnSpyro();
            return;
        }

        #if BUILD == 2 || BUILD == 0 || BUILD == 5
        FullLoadState();
        #elif BUILD == 1 || BUILD == 3
        ReloadSpyroAndCamera(false);
        #endif
    }
}