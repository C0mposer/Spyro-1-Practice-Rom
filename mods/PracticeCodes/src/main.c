#include <common.h>

bool hasSkippedIntro = FALSE;
bool hasUnlockedLevels = FALSE;

void UnlockAllLevels()
{
    for (int i = 0; i < 33; i++)
    {
    if(i < 6){
    *(char *)(0x800758D0 + i) = 2;
    }

    *(char *)(0x80078E78 + i) = 1;
    }
}

void SkipIntro()
{
    _isPastTitleScreen = TRUE;                            //? This flag is checked by TheAdventureBeings() before it runs, so we must set this flag to TRUE.
    TheAdventureBegins();                                 //? Call The Adventure Begins cutscene sequence      
}

void ResetCollectables()
{
    int* reset = 0x8003B99C;
    *reset = 0x34030000; 
}

void SavePosition()
{
    _freeSpace[0] = _spyro.position.x;
    _freeSpace[1] = _spyro.position.y;
    _freeSpace[2] = _spyro.position.z;

    _freeSpace[3] = _spyro.angle.pitch;
    _freeSpace[4] = _spyro.angle.yaw;
    _freeSpace[5] = _spyro.angle.roll;

    _freeSpace[6] = _cameraPosition.x;
    _freeSpace[7] = _cameraPosition.y;
    _freeSpace[8] = _cameraPosition.z;

    _freeSpace[9] = _cameraAngle.pitch;
    _freeSpace[0xA] = _cameraAngle.yaw;
    _freeSpace[0xB] = _cameraAngle.roll;
}

void ReloadPosition()
{
        _spyro.position.x = _freeSpace[0];
        _spyro.position.y = _freeSpace[1];
        _spyro.position.z = _freeSpace[2];

        _spyro.angle.pitch = _freeSpace[3];
        _spyro.angle.yaw = _freeSpace[4];
        _spyro.angle.roll = _freeSpace[5];

        _cameraPosition.x = _freeSpace[6];
        _cameraPosition.y = _freeSpace[7];
        _cameraPosition.z = _freeSpace[8];

        _cameraAngle.pitch = _freeSpace[9];
        _cameraAngle.yaw = _freeSpace[0xA];
        _cameraAngle.roll = _freeSpace[0xB];
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

void SetTitleScreenColor()
{
    *(short*)(0x8001A674) = 0x70;
    *(short*)(0x8001A67C) = 0x0;
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
        SetTitleScreenColor();
    }
    if(_currentButtonOneFrame == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON)
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
    if(_currentButtonOneFrame == L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON || _movementSubState == MOVEMENT_SUBSTATE_LOADING || _gameState == GAMESTATE_DEATH)
    {
        ResetLevelGems();
    }

}
