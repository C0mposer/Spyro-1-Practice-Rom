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

void ActivateLevelSelect()
{

	DetermineButton();


	//Pause to prepare for level leave
	if(levelSelectState == 1)
	{
		levelSelectState = 2;
	}

	//Level Level
	if (levelSelectState == 2)
	{
		_spyro.state = 0;
		_gameState = 0xA;
		_pausedTimer = 0;
		levelSelectState = 3;
	}

	//Set Level ID and Animation to values detemined in HOPEFULLY A FUNC
	if(_movementSubState == MOVEMENT_SUBSTATE_LOADING && levelSelectState == 3)
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

typedef struct InstaLoadState{
	int ready;
	int timer;
}InstaLoadState;

InstaLoadState ilState;

void InstaLoad(){
	
	if(ilState.ready){
		ResetCollectables();
        _flightWingsAnimation = 0;
        _loadingScreenTimer = 0;
        _isLoading = 1;									//Set to 0 to immidiately start fly in
        _canFlyIn = 1;
        _gameState = GAMESTATE_LOADING;
        _levelLoadState = 0xB;
        _flyInAnimation = flyInArray[_levelIDIndex];
        _cameraLockingRelated = 0x80000012;				// 0x80000012 is not an address it is just the value it is expecting for level loads
        _musicState = 0x40;
		_spyro.health = 3;
		ilState.ready = 0;
		ilState.timer = 3 * 30;
    }

	if(_currentButton == (L2_BUTTON + R2_BUTTON + TRIANGLE_BUTTON + UP_BUTTON)){		//Hack to load the terrain near fly in before it is initiated
        _spyro.position.x = *_ptr_levelSpawn;
		_spyro.position.y = *(_ptr_levelSpawn + 1);
		_spyro.position.z = *(_ptr_levelSpawn + 2);
		ilState.ready = 1;
	}

	if(ilState.timer){
		ilState.timer--;
		if(!ilState.timer){
			_isLoading = 0;
		}
	}
}