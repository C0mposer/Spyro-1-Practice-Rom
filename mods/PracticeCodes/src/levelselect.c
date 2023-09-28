#include <common.h>
#include <levelselect.h>

char levelSelectState = 0;
levelSelectData selectedData;
char selectedButton;

//Storing all the home world options in an array, and storing all the Level ID's in the respective homeworld and their fly in animation in an nested array
levelSelectData bakedData[6][6] =
{
	{	//Artisans
		{TOWN_SQUARE_ID,		FACING_LEFT},		//r1Button
		{STONE_HILL_ID,			FACING_LEFT},		//l1Button
		{TOASTY_ID,				FACING_LEFT},		//r2Button
		{DARK_HALLOW_ID,		FACING_FORWARD},	//l2Button
		{SUNNY_FLIGHT_ID,		RETURNING_HOME},	//r3Button
		{ARTISANS_ID,			FACING_LEFT},		//l3Button
	},

	{	//Peace Keepers
		{ICE_CAVERN_ID,			FACING_RIGHT},		//r1Button
		{DRY_CANYON_ID,			FACING_LEFT},		//l1Button
		{SHEMP_ID,				FACING_LEFT},		//r2Button
		{CLIFF_TOWN_ID,			FACING_RIGHT},		//l2Button
		{NIGHT_FLIGHT_ID,		FACING_RIGHT},		//r3Button
		{PEACE_KEEPERS_ID,		FACING_LEFT},		//l3Button
	},
 
	{	//Magic Crafters
		{WIZARD_PEAK_ID,		FACING_DIAGONAL},	//r1Button
		{ALPINE_RIDGE_ID,		FACING_LEFT},		//l1Button
		{BLOWHARD_ID,			FACING_RIGHT},		//r2Button
		{HIGH_CAVES_ID,			FACING_BACKWARDS},	//l2Button
		{CRYSTAL_FLIGHT_ID,		FACING_LEFT},		//r3Button
		{MAGIC_CRAFTERS_ID,		FACING_LEFT},		//l3Button
	},
 
	{	//Beast Makers
		{TREE_TOPS_ID,			FACING_BACKWARDS},	//r1Button
		{TERRACE_VILLAGE_ID,	FACING_LEFT},		//l1Button
		{METALHEAD_ID,			FACING_BACKWARDS},	//r2Button
		{MISTY_BOG_ID,			FACING_LEFT},		//l2Button
		{WILD_FLIGHT_ID,		FACING_LEFT},		//r3Button
		{BEAST_MAKERS_ID,		FACING_LEFT},		//l3Button
	},
 
	{	//Dream Weavers
		{HAUNTED_TOWERS_ID,		FACING_LEFT},		//r1Button
		{DARK_PASSAGE_ID,		FACING_BACKWARDS},	//l1Button
		{JACQUES_ID,			FACING_RIGHT},		//r2Button
		{LOFTY_CASTLE_ID,		FACING_LEFT},		//l2Button
		{ICY_FLIGHT_ID,			FACING_LEFT},		//r3Button
		{DREAM_WEAVERS_ID,		FACING_LEFT},		//l3Button
	},
 
	{	//Gnasty's World
		{GNASTY_GNORC_ID,		FACING_LEFT},		//r1Button
		{GNORC_COVE_ID,			FACING_BACKWARDS},	//l1Button
		{GNASTYS_LOOT_ID,		FACING_LEFT},		//r2Button
		{TWILIGHT_HARBOR_ID,	FACING_LEFT},		//l2Button
		{GNASTYS_WORLD_ID,		FACING_LEFT},		//r3Button
		{GNASTYS_WORLD_ID,		FACING_LEFT},		//l3Button
	},
};





void DetermineButton()
{
	//if you're in the inventory, and have yet to pick a level
	if(_gameState == GAMESTATE_INVENTORY && levelSelectState == 0)
	{

		levelSelectState = 1;

		switch(_currentButton)
		{
			case(R1_BUTTON):
			{
				selectedButton = 0;
				break;
			}
			
			case(L1_BUTTON):
			{
				selectedButton = 1;
				break;
			}
			
			case(R2_BUTTON):
			{
				selectedButton = 2;
				break;
			}
			
			case(L2_BUTTON):
			{
				selectedButton = 3;
				break;
			}
			
			case(R3_BUTTON):
			{
				selectedButton = 4;
				break;
			}
			
			case(L3_BUTTON):
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

	selectedData = bakedData[_selectMenuOption][selectedButton];

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
		_flightWingsAnimation = 0;
		_portalToExitFromInHW = 0;
		_levelID = selectedData.levelID;
		_flyInAnimation = selectedData.anim;
        levelSelectState = 4;
    }

	//Reset Level Select Flag, and levelSelectData
	if(levelSelectState == 4)
	{
		levelSelectState = 0;
		selectedData.levelID = 0;
		selectedData.anim = 0;
        levelSelectState = 0;
	}

}