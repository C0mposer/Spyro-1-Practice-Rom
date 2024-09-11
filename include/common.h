#ifndef COMMON_H
#define COMMON_H


#include "custom_types.h"
#include "vector.h"

//~~~~~~~~~
//Constants
//~~~~~~~~~

#define REDUX 0
#define PS1 1
#define PS2_DECKARD 2
#define PS2_IOP 3

#define MOD_VERSION_STRING "V3.0"

#if BUILD == 0
    #define STARTING_EXTRA_MEM 0x80300000
#elif BUILD == 2
    #define STARTING_EXTRA_MEM 0x80A10000
    #define EXTRA_DECKARD_CODE_REGION 0x80A49000
#endif

#if BUILD == PS1 || BUILD == PS2_IOP || BUILD == REDUX
    #define FLAME_SECTOR 265765
    #define SPARX_SECTOR 265766
    #define SKIN_SECTOR 265767
#elif BUILD == PS2_DECKARD
    #define FLAME_SECTOR 265767
    #define SPARX_SECTOR 265768
    #define SKIN_SECTOR 265769
#endif


#define SCREEN_LEFT_EDGE 0x0
#define SCREEN_RIGHT_EDGE 0x200
#define SCREEN_TOP_EDGE 0x0
#define SCREEN_BOTTOM_EDGE 0xE8

#define SECONDS 30

#define NUMBER_TO_ASCII(x) x + '0'

typedef enum Direction
{
    LEFT,
    RIGHT
}Direction;

typedef enum SwitchButton
{
    OFF,
    ON
}SwitchButton;

enum Buttons
{
    X_BUTTON = 0x0040,
    SQUARE_BUTTON = 0x0080,
    CIRCLE_BUTTON = 0x0020,
    TRIANGLE_BUTTON = 0x0010,

    UP_BUTTON = 0x1000,
    RIGHT_BUTTON = 0x2000,
    DOWN_BUTTON = 0x4000,
    LEFT_BUTTON = 0x8000,

    SELECT_BUTTON = 0x0100,
    START_BUTTON = 0x0800,

    R1_BUTTON = 0x0008,
    L1_BUTTON = 0x0004,
    R2_BUTTON = 0x0002,
    L2_BUTTON = 0x0001,

    L3_BUTTON = 0x0200,
    R3_BUTTON = 0x0400
};

enum GameState
{
    GAMESTATE_GAMEPLAY = 0,
    GAMESTATE_LOADING = 0x1,
    GAMESTATE_PAUSED = 0x2,
    GAMESTATE_INVENTORY = 0x3,
    GAMESTATE_DEATH = 0x4,
    GAMESTATE_GAME_OVER = 0x5,
    GAMESTATE_BETA_DRAGON_TEXTBOX_MAYBE = 0x6,
    GAMESTATE_FLIGHT_LEVEL_MENU = 0x7,
    GAMESTATE_DRAGON_STATE = 0x8,
    GAMESTATE_FLY_IN = 0x9,
    GAMESTATE_EXITING_LEVEL = 0xA,
    GAMESTATE_DRAGON_FAIRY_TEXTBOX = 0xB,
    GAMESTATE_BALLOONIST = 0xC,
    GAMESTATE_TITLE_SCREEN = 0xD,
    GAMESTATE_CUTSCENE = 0xE,
    GAMESTATE_CREDITS = 0xF
};

enum MovementSubStates
{
    MOVEMENT_SUBSTATE_LOADING = 0x8,
    MOVEMENT_SUBSTATE_EXIT_PORTAL = 0x9,
    MOVEMENT_SUBSTATE_FLY_IN_LOOP = 0xA,
    MOVEMENT_SUBSTATE_FLY_IN_CAMERA_180 = 0xA,
    MOVEMENT_SUBSTATE_FLY_IN_TREE_TOPS = 0xB
};

enum BalloonistStates
{
    BALLOONIST_STATE_NEVER_TALKED_TO_BALLOONIST = 0x0,
    BALLOONIST_STATE_TEXTBOX = 0x1,
    BALLOONIST_STATE_JUMPING_ONTO_BALLOON = 0x2,
    BALLOONIST_STATE_BALLOON_RISING = 0x3,
    BALLOONIST_STATE_BALLOON_LOADING_PART_1 = 0x4,
    BALLOONIST_STATE_BALLOON_LOADING_PART_2 = 0x5,
    BALLOONIST_STATE_BALLOON_LOWERING = 0x6,
    BALLOONIST_STATE_JUMPING_OUT_OF_BALLOON = 0x7

};

enum SpyroHealthStates
{
    YELLOW_SPARX = 3,
    BLUE_SPARX = 2,
    GREEN_SPARX = 1,
    NO_SPARX = 0
};

enum LevelIDS
{
    ARTISANS_ID = 0xA,
    STONE_HILL_ID = 0xB,
    TOWN_SQUARE_ID = 0xD,
    DARK_HALLOW_ID = 0xC,
    TOASTY_ID = 0xE,
    SUNNY_FLIGHT_ID = 0xF,

    PEACE_KEEPERS_ID = 0x14,
    DRY_CANYON_ID = 0x15,
    CLIFF_TOWN_ID = 0x16,
    ICE_CAVERN_ID = 0x17,
    SHEMP_ID = 0x18,
    NIGHT_FLIGHT_ID = 0x19,

    MAGIC_CRAFTERS_ID = 0x1E,
    ALPINE_RIDGE_ID = 0x1F,
    HIGH_CAVES_ID = 0x20,
    WIZARD_PEAK_ID = 0x21,
    BLOWHARD_ID = 0x22,
    CRYSTAL_FLIGHT_ID = 0x23,

    BEAST_MAKERS_ID = 0x28,
    TERRACE_VILLAGE_ID = 0x29,
    MISTY_BOG_ID = 0x2A,
    TREE_TOPS_ID = 0x2B,
    METALHEAD_ID = 0x2C,
    WILD_FLIGHT_ID = 0x2C,

    DREAM_WEAVERS_ID = 0x32,
    HAUNTED_TOWERS_ID = 0x35,
    DARK_PASSAGE_ID = 0x33,
    LOFTY_CASTLE_ID = 0x34,
    JACQUES_ID = 0x36,
    ICY_FLIGHT_ID = 0x37,

    GNASTYS_WORLD_ID = 0x3C,
    GNORC_COVE_ID = 0x3D,
    TWILIGHT_HARBOR_ID = 0x3E,
    GNASTY_GNORC_ID = 0x3F,
    GNASTYS_LOOT_ID = 0x40
};

enum LevelIndex
{
    INDEX_ARTISANS = 0,
    INDEX_STONE_HILL = 1,
    INDEX_TOWN_SQUARE = 2,
    INDEX_DARK_HALLOW = 3,
    INDEX_TOASTY = 4,
    INDEX_SUNNY_FLIGHT = 5,

    INDEX_PEACE_KEEPERS = 6,
    INDEX_DRY_CANYON = 7,
    INDEX_CLIFF_TOWN = 8,
    INDEX_ICE_CAVERN = 9,
    INDEX_SHEMP = 10,
    INDEX_NIGHT_FLIGHT = 11,

    INDEX_MAGIC_CRAFTERS = 12,
    INDEX_ALPINE_RIDGE = 13,
    INDEX_HIGH_CAVES = 14,
    INDEX_WIZARD_PEAK = 15,
    INDEX_BLOWHARD = 16,
    INDEX_CRYSTAL_FLIGHT = 17,

    INDEX_BEAST_MAKERS = 18,
    INDEX_TERRACE_VILLAGE = 19,
    INDEX_MISTY_BOG = 20,
    INDEX_TREE_TOPS = 21,
    INDEX_METALHEAD = 22,
    INDEX_WILD_FLIGHT = 23,

    INDEX_DREAM_WEAVERS = 24,
    INDEX_HAUNTED_TOWERS = 25,
    INDEX_DARK_PASSAGE = 26,
    INDEX_LOFTY_CASTLE = 27,
    INDEX_JACQUES = 28,
    INDEX_ICY_FLIGHT = 29,

    INDEX_GNASTYS_WORLD = 30,
    INDEX_GNORC_COVE = 31,
    INDEX_TWILIGHT_HARBOR = 32,
    INDEX_GNASTY_GNORC = 33,
    INDEX_GNASTYS_LOOT = 34
};

enum LevelFlyInAnimations
{
    FACING_LEFT = 0xCA6C,
    FACING_RIGHT = 0xCA3C,
    FACING_FORWARD = 0xCA24,
    FACING_BACKWARDS = 0xCA84,
    FACING_DIAGONAL = 0xCA9C, // wizard peak
    RETURNING_HOME = 0xCA54
};

enum InventoryMenuSelections
{
    ARTISANS_MENU = 0,
    PEACE_KEEPERS_MENU = 1,
    MAGIC_CRAFTERS_MENU = 2,
    BEAST_MAKERS_MENU = 3,
    DREAM_WEAVERS_MENU = 4,
    GNASTYS_WORLD_MENU = 5
};

enum StartMenuSelections
{
    STARTMENU_CONTINUE = 0,
    STARTMENU_OPTIONS = 1,
    STARTMENU_INVENTORY = 2,
    STARTMENU_EXIT_LEVEL = 3,
    STARTMENU_QUIT_GAME = 3

};

typedef struct RECT
{
    short x;
    short y;
    short w;
    short h;
}RECT;
typedef struct u8RGBA
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char opacity;
}u8RGBA;

typedef struct RGB
{
    unsigned int R;
    unsigned int G;
    unsigned int B;
}RGB;

typedef struct u8RGB
{
    unsigned char R;
    unsigned char G;
    unsigned char B;
}u8RGB;

typedef struct u8BGR
{
    unsigned char B;
    unsigned char G;
    unsigned char R;
}u8BGR;

typedef struct NOPHexCode
{
    int hexCode1;
    int hexCode2;
}NOPHexCode;

typedef struct RanOnceBitFlags
{
    bool hasRanOnceLoadSpyroReset:1;


}RanOnceBitFlags;

typedef struct AnalogStick
{
    u8 x_axis;
    u8 y_axis;
}AnalogStick;


//*~~~~~~~~~~~~~~~~
//*Custom Functions 
//*~~~~~~~~~~~~~~~~

#include "spyro.h" //? Including here because spyro.h needs acess to the structs above here, lol...


//*~~~~~~~~~~~~~~~~~~~~~~~~
//*        SYMBOLS
//*~~~~~~~~~~~~~~~~~~~~~~~~



//*~~~~~~~~~~~~~~~~~~
//*In game Functions
//*~~~~~~~~~~~~~~~~~~


//PSY-Q
int printf (const char * format, ...);
int sprintf(char *output, const char *format, ...);
void memset(void *str, int c, int n);
void* memcpy(void* destination, const void* source, int num);
int rand();
void srand(int seed);

//Drawing Functions
int DrawTextCapitals(char* text, void* TextInfo, int spacing, char color);                               //? This function draws text with all capitcal letters.
int DrawTextAll(char *text, int *CapitalTextInfo, int *LowercaseTextInfo, int spacing, char colour);    //? This function draws text with 1 capital letter at the beginning of each word.
int DrawTextBox(int leftBoundaryDistanceFromLeft, int RightBoundaryDistanceFromLeft, int TopBoundaryDistanceFromTop, int BottomBoundaryDistanceFromTop); //? This function draws a textbox with gold lines around it.
void DrawArrow(int *MobyInfo, unsigned int timer, int arrowDirection);                                  //? This function draws an arrow either to the left or rigt.
void DrawLine(int point1X, int point1Y, int point2X, int point2Y);                                      //? This function draws a yellow line.
void FillScreenColor(int colorSpace, char r, char g, char b);                                           //? Fills the screen with a specific color.
void DrawPrimative(int param_1);

//Pre-Reqs for other functions
void CopyHudToShaded();
void DrawShadedMobys();
void unk2();
void RenderThing(int a, int b, int c);
void DrawMobys();
void RenderObjs();
void DrawShapePreReq(void* param_1, int param_2, int param_3, short colorSpace, int param_5);


void PlayIntroCutscenes();                                                                              //? This function is related to the intro cutscenes.

void TheAdventureBegins();                                                                              //? This function calls the "The Adventure Begins" start sequence in the game.
void TheAdventureContinues();

void UpdateMobyCollision(int param_1, unsigned int param_2);                                            //? Research this more.

void LockCameraToSpyro(void);

//Particle Related Reversing
void Vec3IntToShortDiv4(short *Vec3ShortPTR,int *Vec3PTR);                                              //? This function takes an int vector, divides it by 4, then stores it as a short. Some things in spyro need to refer to position as a short rather than an int, like particles, this is where you'd convert them.

char* GetNextParticleSlot(char param_1);                                                                //param_1 Might be storing its subtype. It affects the way the particle looks ratio wise? It also stores the param value you pass into param_1 into the 1st element of the particle struct. NOT THE 0th.

void CreateParticle(int param_1, int param_2, int **ptrToMoby, int *param_4);                           //? This function creates a particle. param_1 amount of particles, param_2 Is the Particle ID, param_3 is a Vec3 to its Initial Spawn Position, param_4 Vec3 of Amount of Units to Travel from Inital POS

void UpdateTerrain(unsigned int terrainNumber, char param_2, char param_3);                             //? This function updates moveable terrain
void TerrainSkipToKeyFrame(unsigned int terrainNumber, char keyFrame, int param_3);                     //? This function jumps moveable terrain to a particular animation key frame
int TerrainCurrentKeyFrame(int terrainNumber);                                                          //? This function returns the current key frame for the supplied movable terrain

short LoadClut2(unsigned int* clut, int x, int y);
int DrawSync(int moode);
int VSync(int mode);
int LoadImage(RECT* rect, int* ram_location);
void PauseMusicAndSFX(unsigned int param_1);
void PlayMusic(int track_number, int flags);
void LevelTransition(void);


//*~~~~~~~~~~~~~~~~~
//*In Game Variables
//*~~~~~~~~~~~~~~~~~

//Booleans
extern bool _isPastTitleScreen; //0x8007566c                 //? 1 If has started new game/continue game. Basically after you leave the title screen this is always 1.
extern bool _isLoading; //0x800756B0                         //? 1 If is in loading screen.
extern bool _isInsideOptionsMenu; //0x800757C8               //? 1 If inside options menu in start menu.
extern bool _canFlyIn; //0x800756D0                          //? 1 If can fly in to level. If set to 0, will just fade in instead of flying in. I assume this is a leftover from prototypes?
extern bool _flightWingsAnimation; //0x80078CA4
extern bool _shouldRespawnAtCheckpoint; //0x80077888

//Timers
extern int _globalTimer; //0x800749e0                        //? This is the global timer. It is running at all times from game boot up past BIOS.                
extern int _pausedTimer; //0x800758B8                        //? This is the pause/inventory menu timer. It is only running in those menus.
extern int _levelTimerWhenActive; //0x8007572C               //? This is a level specific timer. This timer is running when when you are able to control spyro, and doesn't reset until you enter a new level.
extern int _levelTimer_60fps; //0x800758C8                   //? This is a level specific timer that runs at 60fps. This timer is running at all times, and doesn't reset until you enter a new level.
extern int _vSyncTimer_60fps; //0x800749E0                   //? This is a global timer at 60fps. It is running at all times, and controls the vSync. If frozen, video freezes. Can be unfrozen.
extern int _wobbleAndOpacityTimer; //0x800770f4              //? This is a timer that is often used for changing the opacity of things like lines, the shimmer of text, etc.
extern int _loadingScreenTimer; //0x800756ac                 //? This is a timer that is used for the loading screen. It need to be set to 0 when triggering a level load

//Important Stuff
extern char _gameState; //0x800757D8                         //? Main gamestate. Gamestate values are stored in the GameState enums.
extern unsigned short _currentButton; //0x80077380           //? Value coorsponding the the current button being pressed.
extern unsigned short _currentButtonOneFrame; //0x80077378   //? Value coorsponding the the current button being pressed, then goes back to 0 after 1 frame.
extern int _secondController; //0x80078E50                   //! STILL RESEARCHING
//Analog Sticks in analog_sticks.h
extern AnalogStick _rightAnalogStick;

extern Spyro _spyro; //0x80078A58                            //? Start of the Spyro Struct
extern int _movementSubState; //0x80078AD4                   //! Research this more. 0xA is the loop for example.
extern int _isInPortal; //0x80078C70                         //? 0x5 for in portal, 0xF for loading afterwards. Stays on 0xF until next portal.
                                                             //! MIGHT BE PART OF SPYTO STRUCT. RESEARCH PLZ
extern short _globalGems; //0x80075860                       //? Total amount of global gems collected
extern int _globalLives; //0x8007582C                        //? Total amount of global lives
extern int _globalDragons; //0x80075750                      //? Total amount of gloabl dragons
extern int _globalEggs; //0x80075810                         //? Total amount of global eggs
extern int _levelGemsCollectedArray[35]; //0x80077420        //? Total amount of gems collected in each level
extern int _levelDragonCounts[35]; //0x800772d8              //? Total amount of dragons collected in each level

extern char _levelID; //0x800758B4                           //? Level ID according to the LevelIDs enum. This LevelID is used to determine which level will be loaded when in a loading screen, amung other things.
extern char _portalToExitFromInHW; //0x800758AC              //? Same as the Level Id for the level. Also if 0, means entering level for fly in, not returning home
extern int _levelIDIndex; //0x80075964                       //? Level ID that is used for indexing through level arrays.

extern int _portalNumber; //0x8007576c                       //? Number for the portal that spyro should fly out of in the homeworld

extern short _flyInAnimation; //0x80076EA8                   //? This is what determines the fly in animation for the level, determined by the LevelFlyInAnimations enum's.

extern Vec3 _cameraPosition; //0x80076DF8                    //? Start of Camera Position Vector
extern CameraAngle _cameraAngle; //0x80076E1C                //? Start of Camera Angle Vector

extern int _shouldCameraFollowSpyro; //0x80033b4c            //? This is actually just a call to the SpyroCamera Function. So this is why its not simply true or false, it is nopping the call to the function when we set it to 0.
#define DONT_FOLLOW_SPYRO 0                                  //? This is the hex representation of nopping the call to LockCameraToSpyro in MIPS. When we set it to this, we are nopping the function call.
#define FOLLOW_SPYRO 0x0C00DEF5                              //? This is the hex representation of the call to LockCameraToSpyro in MIPS. When we set it to this, we are un-nopping the function call.

extern char _mainMenuState; //0x80078D88                     //? This is the current state of the main menu. Should make an enum for this
extern int _ballonistState; //0x800777e8                     //? The Sub State for the balloonist. Should make an enum for this.
extern unsigned int _levelLoadState; //0x80075864            //? The sub state for level loads.
extern int _dragonState; //0x80077058                        //? The sub state for dragon cracking.
extern int _keyState; //0x80075830            
extern int _doesHaveSuperflame;
extern int _superflameTimer; 
extern int _globalFlightLevelTimer;

extern int _levelGemsCollectedArray[35];

extern int _globalFlightLevelCollectedItemsArray[4];

extern int _startMenuSelection; //0x80075720                 //? This is the current option selected in the main start menu, according to the StartMenuSelections enum's.
extern int _selectMenuOption; //0x80075744                   //? This is the current homeworld menu in the inventory menu according to the InventoryMenuSelections enum's.

extern char _whichCutscene; //0x80078D94                     //? Which cutscene is currently being played

extern char _effect_ScreenFadeOut; //0x8007593c              //? This is what determines how much to fill the screen with black for the fade to black
extern char _effect_ScreenFadeIn; //0x80075918               //? This is what determines how much to fill the screen with a black fade in effect. Will automatically decrement the fade back to 0 unless set every frame.
extern char _effect_ScreenLetterBox; //0x800756C0            //? This is what determines how much to fill the screen with the top and bottom letterbox effect. Will automatically decrement the letterbox to 0 unless set every frame.

extern char _collectablesStateArray[1231]; //0x80077900      //? The array for what collectables you have collected. This is what the game references for loading gems/dragons into a level or not. When you load a game from a save file, this is immediately where the data gets stored.

extern int* _ptr_levelMobys; //0x80075828                    //? This is a pointer to the start of the level moby's array.
extern int* _ptr_dynamicLevelMobys; //0x8007573C             //? This is a pointer to the start of the dynamic level moby's array.

extern int* _ptr_levelMobyData; //0x80075930                 //? This is a pointer to the start of the level moby's data array.
#define _ptr_endDynamicMobys _ptr_levelMobyData //0x80075930 //? Just giving it another name, since it also could be used to indicate the end of dynamic mobys, instead of just being used to indicate the start of levelMobyData.

extern int* _ptr_sparx;
extern int* _ptr_sparx2;

extern int* _ptr_keyChestHostGem; //0x80075758                //? Points to the "Host Gem" for the last key chest that was opened.

extern int* _ptr_graphicsRelated; //0x800757b0                //? Not too sure.
extern int _ptrTextUnk; //0x800720f4                         //? Not too sure.
extern int* _ptr_HudMobyData; //0x80075930                    //? Not too sure. Think this is where the hud moby data lives?

extern char* _ptr_particleLinkedList; //0x80075738           //? This is a pointer to the next available particle slot.

extern int* _ptr_levelSpawn; //0x800785e4                    //? This is a pointer to the spawn coordinates for the current level.

extern int* _ptr_headControlMoby; //0x8015865C               //? This is a pointer to the gnorc gnexus heads control moby

extern int* _ptr_moving_texture_data; //0x80078584

extern int*  _ptr_moving_collision_data; //0x800785a4

extern int* _ptr_textures_array; //0x800785a8

extern int*  _ptr_ptr_moving_collision; //0x800785d4

extern int* _ptr_low_lod_texture_data; //0x80078574

extern int* _ptr_myst_texture_data; //0x8007856C

extern int _headAndChestSpinTimer; //0x80077fe4
extern int _unk_spinRelated; //0x8006cc78
extern char* _localSoundEffects; //0x800761D4

extern char _balloonist_requirements_state[6];

extern char _entered_level_flags[35];
extern char _vortexFlags[35]; //0x8007a6a8

extern RGB _paused_screen_color;

extern int _cameraLockingRelated; //0x80076e90

extern int _collectablesBitflags; //0x80077908

extern int _musicState; //0x800774B4                        //?

extern int _musicVolume; //0x80075748                       //? Music Volume in Pause Menu

extern int _currentMusicPos;

extern int _pauseMusic;

extern int _currentMusicTrack;

extern char* _cameraStart; //0x80076DD0                     //? Start of Camera Struct?

extern int _nestorMakeUnskipable;

extern int _hBlankTimer;

extern int _freeSpace[0xE5F]; //0x80073990                  //? This is almost 1kb of free space in the game. 0xE5F to be exact
extern byte _freeSpace2;

extern bool _isInInGameCutscene;

extern bool _spyroInvisible;

extern int _treeTopsThiefTimer;

extern int _dragonWalkTime;

extern int* _sparx_ptr;
extern u8RGBA _sparxGlowColor;
extern u8RGBA _sparxGlowColor2;
extern int _sparxGlowStrength;

extern int _levelIDPortalExit;


#endif /* COMMON_H */
