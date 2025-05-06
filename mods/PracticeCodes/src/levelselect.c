#include <common.h>
#include <levelselect.h>
#include <shared_funcs.h>
#include <multitap.h>

typedef enum LevelSelectState
{
	READY,
	LEVEL_CHOSEN,
	STARTING_LEVEL_LOAD
}LevelSelectState;

typedef enum LevelSelectButtonSelections
{
	LEVEL_SELECT_L3,
	LEVEL_SELECT_L1,
	LEVEL_SELECT_L2,
	LEVEL_SELECT_R1,
	LEVEL_SELECT_R2,
	LEVEL_SELECT_R3
}LevelSelectButtonSelections;

LevelSelectState levelSelectState = READY;
bool instaLoadReady = FALSE;
LevelSelectButtonSelections buttonSelection = -1;
int balloonLoadTimer = 0;

int instaLoadNopFixTimer = 0;

//Storing the fly in animation for all the levels to be iterated through using the levelID at 0x80075964
const short flyInArray[36] = { FACING_LEFT, FACING_LEFT, FACING_FORWARD, FACING_LEFT, FACING_LEFT, RETURNING_HOME,
						FACING_LEFT, FACING_LEFT, FACING_RIGHT, FACING_RIGHT, FACING_LEFT, FACING_RIGHT,
						FACING_LEFT, FACING_LEFT, FACING_BACKWARDS, FACING_DIAGONAL, FACING_RIGHT, FACING_LEFT,
						FACING_LEFT, FACING_LEFT, FACING_LEFT, FACING_BACKWARDS, FACING_BACKWARDS, FACING_LEFT,
						FACING_LEFT, FACING_BACKWARDS, FACING_LEFT, FACING_LEFT, FACING_RIGHT, FACING_LEFT,
						FACING_LEFT, FACING_BACKWARDS, FACING_LEFT, FACING_LEFT, FACING_LEFT, FACING_LEFT };



void DetermineButton()
{
	//if you're in the inventory, and have yet to pick a level
	if (_gameState == GAMESTATE_INVENTORY && _pausedTimer > 7 && levelSelectState == READY)
	{
		levelSelectState = LEVEL_CHOSEN;	//Assume button has been pressed, until default case. To avoid repeating in each case.

		switch (_currentButton)
		{
			case(L3_BUTTON):
			{
				buttonSelection = LEVEL_SELECT_L3;
				break;
			}

			case(L1_BUTTON):
			{
				buttonSelection = LEVEL_SELECT_L1;
				break;
			}

			case(L2_BUTTON):
			{
				buttonSelection = LEVEL_SELECT_L2;
				break;
			}

			case(R1_BUTTON):
			{
				buttonSelection = LEVEL_SELECT_R1;
				break;
			}

			case(R2_BUTTON):
			{
				buttonSelection = LEVEL_SELECT_R2;
				break;
			}

			case(R3_BUTTON):
			{
				buttonSelection = LEVEL_SELECT_R3;
				break;
			}

			default:
			{
				levelSelectState = READY;	//Set state back to ready. This won't run if a relavent button is pressed.
				break;
			}
		}

		// Digital Users
		if (CheckButtonMultiTap(UP_BUTTON, 2))
		{
			buttonSelection = LEVEL_SELECT_L3;
			levelSelectState = LEVEL_CHOSEN;
		}
		else if (CheckButtonMultiTap(DOWN_BUTTON, 2))
		{
			buttonSelection = LEVEL_SELECT_R3;
			levelSelectState = LEVEL_CHOSEN;
		}

	}

}

void LevelSelectUpdate()
{

	DetermineButton();

	//Prepare Level Select
	if (levelSelectState == LEVEL_CHOSEN)
	{
		ResetLevelCollectables();
		_globalGems = 0;

		_gameState = GAMESTATE_EXITING_LEVEL;
		_spyro.state = 0;
		_pausedTimer = 0;
		_spyro.timer_framesInAir = 1;			//For Tree Tops
		_spyro.health = 3;

		levelSelectState = STARTING_LEVEL_LOAD;
	}

	//Set Level ID and Animation to correct values for level fly in
	if (_movementSubState == MOVEMENT_SUBSTATE_LOADING && levelSelectState == STARTING_LEVEL_LOAD)
	{
		//If selected homeworld, just spawn in
		if (buttonSelection == LEVEL_SELECT_L3) {
			_canFlyIn = false;
		}
		_flightWingsAnimation = 0;
		_portalToExitFromInHW = 0;
		_levelID = ((_selectMenuOption + 1) * 0xA + buttonSelection < 0x41 ? (_selectMenuOption + 1) * 0xA + buttonSelection : 0x3C);
		_flyInAnimation = flyInArray[_selectMenuOption * 6 + buttonSelection];

		levelSelectState = READY;
	}

	//BalloonUpdate();

}

// Called in gamestate_check.s, to avoid updating any mobys for cycle accuracy
void BalloonUpdate()
{
	maybe_SFXProcessing(); // Calling because we are hooking onto this function call in GameStateCheck.

	// Display option to change loop level option when flying into HW
	if (buttonSelection == LEVEL_SELECT_L3 && _gameState == GAMESTATE_LOADING && _loadingScreenTimer > 5 && _loadingScreenTimer < 60 * 5)
	{
		LoopLevelChoiceFlyIn();
	}

	// Balloon
	if (buttonSelection == LEVEL_SELECT_L3 && _gameState == GAMESTATE_GAMEPLAY)
	{
		_gameState = GAMESTATE_BALLOONIST;
		_balloonistState = 0x6;
		_loadingScreenTimer = 0;
		_flightWingsAnimation = 0;
		_portalToExitFromInHW = 0;
		_effect_ScreenFadeIn = 0;

		balloonLoadTimer = 1;
		buttonSelection = -1;

	}

	if (balloonLoadTimer == 2)
	{
		LoadBalloonData(_selectMenuOption);
		_spyro.currentAnim = IDLE_STANDING;
		_spyro.nextAnim = IDLE_STANDING;

		balloonLoadTimer = 0;
		//printf("test\n");
	}

	if (balloonLoadTimer > 0)
		balloonLoadTimer++;

}

//Custom Function for instantly flying back into current level
void InstaLoadUpdate() {

	//Save Camera and Spyro when it is stopped
	if (_levelLoadState == 0xB && _portalToExitFromInHW == 0)
	{
		_spyro.timer_framesInAir = 1;						// For Tree Tops, setting before the save
		SaveSpyroAndCamera(TRUE);
	}

	//If the instaload has been set up, reload the Camera and Spyro
	if (instaLoadReady == TRUE)
	{
		ReloadSpyroAndCamera(TRUE);
		_spyro.health = YELLOW_SPARX;	// Setting before the save since it will load his struct

		//Fix Flight Wiggle
		_flightTargetVerticalRotation = 0;
		_flightTargetHorizontalRotation = 0;
		_flightRoll = 0;

		//Once the level load state is C, reset the bool to allow for another instaload
		if (_levelLoadState == 0xC)
		{
			instaLoadReady = FALSE;
		}
	}

	// Bring back function calls from fixes
	if (_levelLoadState == 0xD)
	{
		// Bring back CameraRotation
		*(int*)0x80033f08 = 0x27BDFFD8;
		*(int*)0x80033f0C = 0x00802821;
	}
	if (instaLoadNopFixTimer == 10) // Waiting 10 frames to bring back the SFX Vec3Length call, because of dark passage crash
	{
		*(int*)0x80056528 = 0x0C005C7F;						// Putting Vec3Length sfx call back after insta-load (0x0C005C7F is the asm opcode bytes for the function call)
	}

//If pressed hotkey for instaload
	if (_currentButton == (L1_BUTTON + R1_BUTTON + TRIANGLE_BUTTON) && _gameState == GAMESTATE_GAMEPLAY && _levelID % 10 != 0)
	{
		ResetLevelCollectables();

		//Pre-reqs for insta-load
		{
			_levelLoadState = 0xB;
			_gameState = GAMESTATE_LOADING;
			_canFlyIn = TRUE;
			_loadingScreenTimer = 0;
			_flightWingsAnimation = 0;
			_portalToExitFromInHW = 0;
			_portalNumber = 0xffffffff;
			_flyInAnimation = flyInArray[_levelIDIndex];
			_cameraLockingRelated = 0x80000012;					// 0x80000012 is not an address it is just the value it is expecting for level loads
			_musicState = 0x40;

			// Fixes
			*(int*)0x80056528 = 0x00000000;					// NOP-ing the Vec3Length call in the SFX proccessing function. This fixes a weird bug with some specific sound sources crashing right after the insta-load?

			// JR RA CameraRotation for strange crash in whirlwind, notably in tree tops
			*(int*)0x80033f08 = 0x03e00008;
			*(int*)0x80033f0C = 0x00000000;
		}

		instaLoadNopFixTimer = 1;
		instaLoadReady = TRUE;
	}

	if (instaLoadNopFixTimer > 0)
	{
		instaLoadNopFixTimer++;
	}
}