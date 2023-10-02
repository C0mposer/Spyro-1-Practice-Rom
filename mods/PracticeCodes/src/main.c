#include <common.h>
#include "levelselect.h"

//Quick Memcpy Implementation
void MyMemCopy(void *dest, void *src, int n) 
{ 
// Typecast src and dest addresses to (char *) 
char *csrc = (char *)src; 
char *cdest = (char *)dest; 
  
//Copy contents of src[] to dest[] 
for (int i=0; i<n; i++) 
    cdest[i] = csrc[i]; 
} 

bool hasSkippedIntro = FALSE;
bool hasUnlockedLevels = FALSE;

//Shows all levels in inventory menu, and automatically unlocks homeworlds for balloonists
void UnlockAllLevels()
{
    for (int i = 0; i < 33; i++)
    {
        if(i < 6)
        {
            _balloonist_requirements_state[i] = 2;
        }

        _entered_level_flags[i] = 1;
    }
}

void SkipIntro()
{
    _isPastTitleScreen = TRUE;                            //? This flag is checked by TheAdventureBeings() before it runs, so we must set this flag to TRUE.
    TheAdventureBegins();                                 //? Call The Adventure Begins cutscene sequence      
}


void SaveSpyroAndCamera()
{  
    //Copying The Spyro struct and most of the camera struct
    MyMemCopy(_freeSpace, &_spyro, sizeof(_spyro));
    MyMemCopy((byte*)_freeSpace + 0x370, &_cameraStart, 0xFF);

}

void ReloadSpyroAndCamera()
{
    //Reloading The Spyro struct and most of the camera struct
    MyMemCopy(&_spyro, _freeSpace, sizeof(_spyro));
    MyMemCopy(&_cameraStart, (byte*)_freeSpace + 0x370, 0xFF);

}

void RespawnSpyro()
{
    _spyro.position.z = 0;
    _shouldRespawnAtCheckpoint = FALSE;
}

void ResetLevelGems()
{
    for (int i = 0; i < 35; i++)
    {
        _levelGemsCollectedArray[i] = 0;
    }
}

//Changing asm instructions for pause menu RGB. Cannot change B, as the value is in a shared register.
void SetTitleScreenColor(byte r, byte g)
{
    *(short*)(0x8001A674) = r;
    *(short*)(0x8001A67C) = g;
}


//*
//* ~ MAIN HOOK ~
//*
void MainFunc()
{
    _globalLives = 99;

    ActivateLevelSelect();

    //Run once upon starting
    if(!hasSkippedIntro && _globalTimer > 20)                //? If the code hasn't ran once yet, and the global timer is greater than 20. Checking global timer because I have to wait a few frames to call The Adventure Begins
    {
        SkipIntro();
        hasSkippedIntro = TRUE;
    }

    //Run once upon skipping intro
    if(_isPastTitleScreen && !hasUnlockedLevels)
    {
        UnlockAllLevels();
        SetTitleScreenColor(70, 0);
        hasUnlockedLevels = TRUE;
    }
    
    //Save/Load spyro & camera information
    if(_currentButtonOneFrame == L3_BUTTON)
    {
        SaveSpyroAndCamera();
    }
    if(_currentButtonOneFrame == R3_BUTTON)
    {
        ReloadSpyroAndCamera();
    }

    //Respawn spyro & reset level gems
    if(_currentButton == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON)
    {
        RespawnSpyro();
    }
    if(_currentButton == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON || _movementSubState == MOVEMENT_SUBSTATE_LOADING || _gameState == GAMESTATE_DEATH)
    {
        ResetLevelGems();
    }

    //Make Nestor Skippable
    if(_levelID == ARTISANS_ID)
    {
        _isNestorUnskipable = FALSE;
    }

    //Moonjump
    if(_currentButton == L1_BUTTON + R1_BUTTON + X_BUTTON)
    {
        _spyro.position.z += 500;
    }

}
