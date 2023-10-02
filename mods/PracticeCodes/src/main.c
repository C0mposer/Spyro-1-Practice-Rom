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

//NOP-ing an asm instruction that handles filling the collectables array
void ResetCollectables()
{
    int* reset = 0x8003B99C;
    *reset = 0x34030000; 
}

void SavePosition()
{  
    //Copying The Spyro struct and most of the camera struct
    MyMemCopy(_freeSpace, &_spyro, sizeof(_spyro));
    MyMemCopy((char*)_freeSpace + 0x370, &_cameraStart, 0x70);

}

void ReloadPosition()
{
    //Reloading The Spyro struct and most of the camera struct
    MyMemCopy(&_spyro, _freeSpace, sizeof(_spyro));
    MyMemCopy(&_cameraStart, (char*)_freeSpace + 0x370, 0x70);

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
    static i;
    *(short*)(0x8001A674) = r;
    *(short*)(0x8001A67C) = g;
    i++;
}


//*
//* ~ MAIN HOOK ~
//*
void MainFunc()
{
    _globalLives = 99;

    ActivateLevelSelect();
    ResetCollectables();

    if(!hasSkippedIntro && _globalTimer > 20)                //? If the code hasn't ran once yet, and the global timer is greater than 20. Checking global timer because I have to wait a few frames to call The Adventure Begins
    {
        SkipIntro();
        hasSkippedIntro = TRUE;
    }

    if(_isPastTitleScreen)
    {
        UnlockAllLevels();
        SetTitleScreenColor(70, 0);
    }
    if(_currentButton == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON)
    {
        RespawnSpyro();
    }
    if(_currentButtonOneFrame == L3_BUTTON)
    {
        SavePosition();
    }
    if(_currentButtonOneFrame == R3_BUTTON)
    {
        ReloadPosition();
    }
    if(_currentButton == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON || _movementSubState == MOVEMENT_SUBSTATE_LOADING || _gameState == GAMESTATE_DEATH)
    {
        ResetLevelGems();
    }

}
