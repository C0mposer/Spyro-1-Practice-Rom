#include <common.h>
#include <levelselect.h>

char levelSelectState = 0;
char selectedButton;

//Storing the fly in animation for all the levels to be iterated through using the levelID at 0x80075964
short flyInArray[36] = {FACING_LEFT, FACING_LEFT, FACING_FORWARD, FACING_LEFT, FACING_LEFT, RETURNING_HOME,
						FACING_LEFT, FACING_LEFT, FACING_RIGHT, FACING_RIGHT, FACING_LEFT, FACING_RIGHT,
						FACING_LEFT, FACING_LEFT, FACING_BACKWARDS, FACING_DIAGONAL, FACING_RIGHT, FACING_LEFT,
						FACING_LEFT, FACING_LEFT, FACING_LEFT, FACING_BACKWARDS, FACING_BACKWARDS, FACING_LEFT,
						FACING_LEFT, FACING_BACKWARDS, FACING_LEFT, FACING_LEFT, FACING_RIGHT, FACING_LEFT,
						FACING_LEFT, FACING_BACKWARDS, FACING_LEFT, FACING_LEFT, FACING_LEFT, FACING_LEFT};


bool instaLoadReady = FALSE;
signed int instaLoadLevelID = -1;

void DetermineButton()
{
	//if you're in the inventory, and have yet to pick a level
	if(_gameState == GAMESTATE_INVENTORY && levelSelectState == 0)
	{

		levelSelectState = 1;

		switch(_currentButton)
		{
			case(L3_BUTTON):
			{
				selectedButton = 0;
				break;
			}
			
			case(L1_BUTTON):
			{
				selectedButton = 1;
				break;
			}
			
			case(L2_BUTTON):
			{
				selectedButton = 2;
				break;
			}
			
			case(R1_BUTTON):
			{
				selectedButton = 3;
				break;
			}
			
			case(R2_BUTTON):
			{
				selectedButton = 4;
				break;
			}
			
			case(R3_BUTTON):
			{
				selectedButton = 5;
				break;
			}

			default:
			{
				levelSelectState = 0;
				break;
			}
		}
		
	}

}

void LevelSelect()
{

	DetermineButton();

	//Level Level
	if (levelSelectState == 1)
	{
		ResetLevelCollectables();
		_spyro.state = 0;
		_spyro.timer_framesInAir = 1;			//For Tree Tops
		_gameState = 0xA;
		_pausedTimer = 0;
		levelSelectState = 2;
		_spyro.health = 3;
	}

	//Set Level ID and Animation to values detemined in HOPEFULLY A FUNC
	if(_movementSubState == MOVEMENT_SUBSTATE_LOADING && levelSelectState == 2)
	{
		if(selectedButton == 0){
			_canFlyIn = 0;
		}
		_flightWingsAnimation = 0;
		_portalToExitFromInHW = 0;
		_levelID = ((_selectMenuOption + 1) * 0xA + selectedButton < 0x41 ? (_selectMenuOption + 1) * 0xA + selectedButton : 0x3C);
		_flyInAnimation = flyInArray[_selectMenuOption * 6 + selectedButton];
        levelSelectState = 0;
    }

}

//Custom Function for instantly flying back in into current level
void InstaLoad(){
	
	if(_levelLoadState == 0xB && _portalToExitFromInHW == 0)
	{
		//printf("Saving\n");
		SaveSpyroAndCamera();
		instaLoadLevelID = _levelIDIndex;
	}

	if(instaLoadReady == TRUE)
	{
		ReloadSpyroAndCamera();
		if(_levelLoadState == 0xC)
		{
			instaLoadReady = FALSE;
		}
    }

	if(_levelLoadState == 0xD)
	{
		*(int *)0x80056528 = 0x0C005C7F;						//Putting SFX proccessing Vec3Length back after insta-load
	}

	if(_currentButton == (L1_BUTTON + R1_BUTTON + TRIANGLE_BUTTON) && _gameState == GAMESTATE_GAMEPLAY)
	{
		ResetLevelCollectables();
        _flightWingsAnimation = 0;
        _loadingScreenTimer = 0;
        if(instaLoadLevelID == _levelIDIndex)
		{
			//printf("Starting\n");
			_levelLoadState = 0xB;
			_isLoading = 0;										//Set to 0 to immidiately start fly in. Is this needed?
			*(int *)0x80056528 = 0x00000000;					//NOP-ing SFX proccessing Vec3Length, because of weird bug with vortex
			instaLoadReady = TRUE;
		}
		else
		{
			_isLoading = 1;									
			_levelLoadState = 0x0;
			instaLoadReady = FALSE;
		}
		_canFlyIn = 1;
        _gameState = GAMESTATE_LOADING;
		_portalToExitFromInHW = 0;
		_portalNumber = 0xffffffff;
        _flyInAnimation = flyInArray[_levelIDIndex];
        _cameraLockingRelated = 0x80000012;				// 0x80000012 is not an address it is just the value it is expecting for level loads
		_musicState = 0x40;
        _spyro.timer_framesInAir = 1;					//For Tree Tops
		_spyro.health = 3;
	}

}