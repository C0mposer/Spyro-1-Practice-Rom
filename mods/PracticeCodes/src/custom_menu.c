#include <common.h>
#include <custom_menu.h>
#include <custom_text.h>
#include <sound.h>
#include <shared_funcs.h>
#include <main_updates.h>
#include <cosmetic.h>
#include <igt.h>
#include <moby.h>
#include <cd.h>

typedef enum MenuState
{
    MENU_HIDDEN,
    MENU_DISPLAYING

} MenuState;

typedef enum TimerDisplayMode
{
    TIMER_ONLY_PAUSE,
    TIMER_ALWAYS

} TimerDisplayMode;

typedef enum ILTimerDisplayMode
{
    IL_TIMER_AT_END,
    IL_TIMER_ALWAYS

} ILTimerDisplayMode;

typedef enum SparxMode
{
    PERMA_SPARX_OFF,
    PERMA_SPARX_ON,
    SPARXLESS

} SparxMode;

typedef struct Menu
{
    int selection;
    char *il_menu_text;
    char *timer_menu_text;
    char *savestate_menu_text;
    char *misc_menu_text;
    char *cosmetic_menu_text;
} Menu;
Menu custom_menu = {0};

typedef struct ILMenu
{
    int selection;
    bool il_state;
    char *il_mode_text;
    ILTimerDisplayMode il_timer_display_mode;
    char *il_timer_display_mode_text;
    bool display_on_dragon;
    char *display_on_dragon_text;
    bool display_on_land;
    char *display_on_land_text;
    bool dont_loop_level;
    char *loop_level_text;

} ILMenu;
ILMenu il_menu = {0};
typedef struct TimerMenu
{
    int selection;
    bool timer_state;
    char *timer_state_text;
    TimerDisplayMode timer_display_mode;
    char *timer_display_mode_text;
    short stop_timer_button_index;
    char *stop_timer_button_text;
    short reset_timer_mode;
    char *reset_timer_button_text;

} TimerMenu;
TimerMenu timer_menu = {0};


#if BUILD == 2 || BUILD == 0
    typedef struct SavestateMenu
    {
        int selection;
        char *stateslot_text;
        char *savestate_button_text;
        char *loadstate_button_text;
        char *switch_state_button_text;

    } SavestateMenu;
    SavestateMenu savestate_menu = {0};
#elif BUILD == 1 || BUILD == 3 // GREY OUT OPTION FOR OTHER PLATFORMS
    typedef struct SavestateMenu
    {
        int selection;
        char *stateslot_text;
        char *savestate_button_text;
        char *loadstate_button_text;
        char *respawn_on_loadstate_text;

    } SavestateMenu;
    SavestateMenu savestate_menu = {0};
#endif


typedef struct MiscMenu
{
    int selection;
    SparxMode sparx_mode;
    char *sparx_mode_text;
    bool show_dragon_touch;
    char *show_dragon_touch_text;
    char *disable_portal_entry_text;
    bool quick_goop_mode;
    char *quick_goop_text;
    char *consitency_tracker_text;
    char *show_sparx_range_text;
} MiscMenu;
MiscMenu misc_menu = {0};
extern bool show_sparx_range_mode;

typedef enum CurrentMenu
{
    MAIN_MENU,
    IL_MENU,
    TIMER_MENU,
    SAVESTATE_MENU,
    MISC_MENU,
    COSMETIC_MENU
} CurrentMenu;

typedef struct FPS_t
{
    int compareTimer;
    int difference;
    int fps;
    int difference_from_baseline;
} FPS_t;

// Globals
bool has_toggled_menu = FALSE;

MenuState menu_state = MENU_HIDDEN;
int current_menu = MAIN_MENU;

int savestate_selection = 0;

bool isHeld = false;

const short STOP_TIMER_BUTTONS[2] = {START_BUTTON, START_BUTTON};
const short RESET_TIMER_BUTTONS[2] = {R3_BUTTON, SELECT_BUTTON};

const short SAVESTATE_BUTTONS[2] = {L3_BUTTON, START_BUTTON};
const short LOADSTATE_BUTTONS[3] = {R3_BUTTON, SELECT_BUTTON, (L2_BUTTON + R2_BUTTON + CIRCLE_BUTTON)};

int savestate_button_index;
int loadstate_button_index;

#if BUILD == 2 || BUILD == 0
    int switch_state_button_index;
#elif BUILD == 1 || BUILD == 3
    bool respawn_on_loadstate = TRUE;
#endif

CdlLOC oldCdLocation;

// Externed from elsewhere
typedef enum ILTimerState
{
    IL_FLYING_IN,
    IL_STARTED,
    IL_DISPLAYING,
    IL_STOPPED
} ILTimerState;
extern ILTimerState il_timer_state;
extern int mainTimerAtReset;

//! Every Frame Update
void CustomMenuUpdate(void)
{
    // Open Menu
    if (_currentButton == L2_BUTTON + R2_BUTTON + TRIANGLE_BUTTON && !has_toggled_menu && _movementSubState != MOVEMENT_SUBSTATE_FLY_IN_TREE_TOPS)
    {
        menu_state = !menu_state;
        has_toggled_menu = TRUE;
        PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);

        if (menu_state == MENU_HIDDEN)
        {
            _spyro.isMovementLocked = FALSE;
            current_menu = MAIN_MENU;
            CdControlB(CDL_PRIMITIVE_SEEKL, (void *)&oldCdLocation, NULL);
            PlayMusic(_currentMusicTrack, 8);
        }
    }

    // When Displaying Menu
    if (menu_state == MENU_DISPLAYING && _gameState == GAMESTATE_GAMEPLAY)
    {
        if (current_menu == MAIN_MENU)
        {
            CapitalTextInfo menu_text_info[5] = {{0}};

            // Easy Exit
            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                menu_state = MENU_HIDDEN;
                _spyro.isMovementLocked = FALSE;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            DrawTextBox(0x30, 0x1D0, 0x30, 0xAA);

            menu_text_info[0].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[0].y = 70;
            menu_text_info[0].size = DEFAULT_SIZE;

            menu_text_info[1].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[1].y = 90;
            menu_text_info[1].size = DEFAULT_SIZE;

            menu_text_info[2].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[2].y = 110;
            menu_text_info[2].size = DEFAULT_SIZE;

            menu_text_info[3].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[3].y = 130;
            menu_text_info[3].size = DEFAULT_SIZE;

            menu_text_info[4].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[4].y = 150;
            menu_text_info[4].size = DEFAULT_SIZE;

            _spyro.isMovementLocked = TRUE;

            if (custom_menu.selection == 0)
            {
                DrawTextCapitals(custom_menu.il_menu_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(custom_menu.il_menu_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (custom_menu.selection == 1)
            {
                DrawTextCapitals(custom_menu.timer_menu_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(custom_menu.timer_menu_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (custom_menu.selection == 2)
            {
                DrawTextCapitals(custom_menu.savestate_menu_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(custom_menu.savestate_menu_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (custom_menu.selection == 3)
            {
                DrawTextCapitals(custom_menu.misc_menu_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(custom_menu.misc_menu_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (custom_menu.selection == 4)
            {
                DrawTextCapitals(custom_menu.cosmetic_menu_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(custom_menu.cosmetic_menu_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            // Fill text with defaults if NULL
            if (custom_menu.il_menu_text == NULL)
            {
                custom_menu.il_menu_text = "IL SETTINGS";
                custom_menu.timer_menu_text = "MANUAL TIMER SETTINGS";
                custom_menu.savestate_menu_text = "SAVESTATE SETTINGS";
                custom_menu.misc_menu_text = "MISC SETTINGS";
                custom_menu.cosmetic_menu_text = "COSMETIC SETTINGS";
            }

            // Change Selection
            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                custom_menu.selection = (custom_menu.selection + 1) % 5;
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                custom_menu.selection = (custom_menu.selection + 4) % 5;
            }

            // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == X_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Timer Selection
            if (custom_menu.selection == 0)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = IL_MENU;
                }
            }

            else if (custom_menu.selection == 1)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = TIMER_MENU;
                }
            }

            // Savestate Selection
            else if (custom_menu.selection == 2)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = SAVESTATE_MENU;
                }
            }

            // Sparx Selection
            else if (custom_menu.selection == 3)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = MISC_MENU;
                }
            }

            else if (custom_menu.selection == 4)
            {
                if (_currentButtonOneFrame == X_BUTTON)
                {
                    current_menu = COSMETIC_MENU;
                    CdControlB(CDL_PRIMITIVE_GETlocL, NULL, (void *)&oldCdLocation);
                }
            }
        }

        if (current_menu == IL_MENU)
        {
            CapitalTextInfo menu_text_info[5] = {{0}};

            // Easy Exit
            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                current_menu = MAIN_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            DrawTextBox(0x30, 0x1F0, 0x30, 0xA8);

            menu_text_info[0].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[0].y = 70;
            menu_text_info[0].size = DEFAULT_SIZE;

            menu_text_info[1].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[1].y = 90;
            menu_text_info[1].size = DEFAULT_SIZE;

            menu_text_info[2].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[2].y = 110;
            menu_text_info[2].size = DEFAULT_SIZE;

            menu_text_info[3].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[3].y = 130;
            menu_text_info[3].size = DEFAULT_SIZE;

            menu_text_info[4].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[4].y = 150;
            menu_text_info[4].size = DEFAULT_SIZE;

            _spyro.isMovementLocked = TRUE;

            if (il_menu.selection == 0)
            {
                DrawTextCapitals(il_menu.il_mode_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_menu.il_mode_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (il_menu.il_state != true)
            {
                DrawTextCapitals(il_menu.il_timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if (il_menu.selection == 1)
            {
                DrawTextCapitals(il_menu.il_timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_menu.il_timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (il_menu.il_state != true)
            {
                DrawTextCapitals(il_menu.display_on_dragon_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if (il_menu.selection == 2)
            {
                DrawTextCapitals(il_menu.display_on_dragon_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_menu.display_on_dragon_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (il_menu.il_state != true)
            {
                DrawTextCapitals(il_menu.display_on_land_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if (il_menu.selection == 3)
            {
                DrawTextCapitals(il_menu.display_on_land_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_menu.display_on_land_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (il_menu.il_state != true)
            {
                DrawTextCapitals(il_menu.loop_level_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if (il_menu.selection == 4)
            {
                DrawTextCapitals(il_menu.loop_level_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(il_menu.loop_level_text, &menu_text_info[4], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            // Fill text with defaults if NULL
            if (il_menu.il_mode_text == NULL)
            {
                il_menu.il_mode_text = "IL MODE OFF";
                il_menu.il_timer_display_mode_text = "IL TIMER DISPLAY AT END";
                il_menu.display_on_dragon_text = "DISPLAY AT DRAGON OFF";
                il_menu.display_on_land_text = "DISPLAY LANDING OFF";
                il_menu.loop_level_text = "LOOP LEVEL ON";
            }

            // Change Selection
            if(il_menu.il_state == true){
                if (_currentButtonOneFrame == DOWN_BUTTON)
                {
                    il_menu.selection = (il_menu.selection + 1) % 5;
                }
                else if (_currentButtonOneFrame == UP_BUTTON)
                {
                    il_menu.selection = (il_menu.selection + 4) % 5;
                }
            }

            // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Timer Selection
            if (il_menu.selection == 0)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_menu.il_state = (il_menu.il_state + 1) % 2;
                }

                if (il_menu.il_state == FALSE)
                {
                    il_menu.il_mode_text = "IL MODE OFF";
                }
                else
                {
                    il_menu.il_mode_text = "IL MODE ON";
                }
            }

            else if (il_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_menu.il_timer_display_mode = (il_menu.il_timer_display_mode + 1) % 2;
                }

                if (il_menu.il_timer_display_mode == IL_TIMER_AT_END)
                {
                    il_menu.il_timer_display_mode_text = "IL TIMER DISPLAY AT END";
                }
                else
                {
                    il_menu.il_timer_display_mode_text = "IL TIMER DISPLAY ALWAYS";
                }
            }

            else if (il_menu.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_menu.display_on_dragon = (il_menu.display_on_dragon + 1) % 2;
                }

                if (il_menu.display_on_dragon == FALSE)
                {
                    il_menu.display_on_dragon_text = "DISPLAY AT DRAGON OFF";
                }
                else
                {
                    il_menu.display_on_dragon_text = "DISPLAY AT DRAGON ON";
                }
            }

            else if (il_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_menu.display_on_land = (il_menu.display_on_land + 1) % 2;
                }

                if (il_menu.display_on_land == FALSE)
                {
                    il_menu.display_on_land_text = "DISPLAY LANDING OFF";
                }
                else
                {
                    il_menu.display_on_land_text = "DISPLAY LANDING ON";
                }
            }
            else if (il_menu.selection == 4)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    il_menu.dont_loop_level = (il_menu.dont_loop_level + 1) % 2;
                }

                if (il_menu.dont_loop_level == 0)
                {
                    il_menu.loop_level_text = "LOOP LEVEL ON";
                }
                else
                {
                    il_menu.loop_level_text = "LOOP LEVEL OFF";
                }
            }
        }

        if (current_menu == TIMER_MENU)
        {
            CapitalTextInfo menu_text_info[4] = {{0}};

            // Easy Exit
            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                current_menu = MAIN_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            DrawTextBox(0x30, 0x1F4, 0x30, 0x98);

            menu_text_info[0].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[0].y = 70;
            menu_text_info[0].size = DEFAULT_SIZE;

            menu_text_info[1].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[1].y = 90;
            menu_text_info[1].size = DEFAULT_SIZE;

            menu_text_info[2].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[2].y = 110;
            menu_text_info[2].size = DEFAULT_SIZE;

            menu_text_info[3].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[3].y = 130;
            menu_text_info[3].size = DEFAULT_SIZE;

            _spyro.isMovementLocked = TRUE;

            if (timer_menu.selection == 0)
            {
                DrawTextCapitals(timer_menu.timer_state_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(timer_menu.timer_state_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (timer_menu.timer_state != true)
            {
                DrawTextCapitals(timer_menu.timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if (timer_menu.selection == 1)
            {
                DrawTextCapitals(timer_menu.timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(timer_menu.timer_display_mode_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (timer_menu.timer_state != true)
            {
                DrawTextCapitals(timer_menu.stop_timer_button_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if (timer_menu.selection == 2)
            {
                DrawTextCapitals(timer_menu.stop_timer_button_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(timer_menu.stop_timer_button_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (timer_menu.timer_state != true)
            {
                DrawTextCapitals(timer_menu.reset_timer_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);
            }
            else if (timer_menu.selection == 3)
            {
                DrawTextCapitals(timer_menu.reset_timer_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(timer_menu.reset_timer_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            // Fill text with defaults if NULL
            if (timer_menu.timer_state_text == NULL)
            {
                timer_menu.timer_state_text = "MANUAL TIMER OFF";
                timer_menu.timer_display_mode_text = "DISPLAY ON STOP";
                timer_menu.stop_timer_button_text = "STOP BUTTON START";
                timer_menu.reset_timer_button_text = "RESET ON LOAD AND RESET";
            }

            // Change Selection
            if(timer_menu.timer_state == true){
                if (_currentButtonOneFrame == DOWN_BUTTON)
                {
                    timer_menu.selection = (timer_menu.selection + 1) % 4;
                }
                else if (_currentButtonOneFrame == UP_BUTTON)
                {
                    timer_menu.selection = (timer_menu.selection + 3) % 4;
                }
            }

            // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            // Timer Selection
            if (timer_menu.selection == 0)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    timer_menu.timer_state = (timer_menu.timer_state + 1) % 2;
                }

                if (timer_menu.timer_state == FALSE)
                {
                    timer_menu.timer_state_text = "MANUAL TIMER OFF";
                }
                else
                {
                    timer_menu.timer_state_text = "MANUAL TIMER ON";
                }
            }

            else if (timer_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    timer_menu.timer_display_mode = (timer_menu.timer_display_mode + 1) % 2;
                    mainTimerAtReset = _globalTimer;
                }

                if (timer_menu.timer_display_mode == IL_TIMER_AT_END)
                {
                    timer_menu.timer_display_mode_text = "DISPLAY ON STOP";
                }
                else
                {
                    timer_menu.timer_display_mode_text = "DISPLAY ALWAYS";
                }
            }

            else if (timer_menu.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    timer_menu.stop_timer_button_index = (timer_menu.stop_timer_button_index + 1) % 2;
                }

                if (timer_menu.stop_timer_button_index == 0)
                {
                    timer_menu.stop_timer_button_text = "STOP BUTTON START";
                }
                else
                {
                    timer_menu.stop_timer_button_text = "STOP BUTTON START";
                }
            }

            else if (timer_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    timer_menu.reset_timer_mode = (timer_menu.reset_timer_mode + 1) % 2;
                }

                if (timer_menu.reset_timer_mode == 0)
                {
                    timer_menu.reset_timer_button_text = "RESET ON LOAD AND RESET";
                }
                else
                {
                    timer_menu.reset_timer_button_text = "RESET ON LOAD ONLY";
                }
            }
        }

        if (current_menu == SAVESTATE_MENU)
        {
// Set menu to 1 by default if not on DECKARD
#if BUILD == 1 || BUILD == 3
            if (savestate_menu.selection == 0)
                savestate_menu.selection = 1;
#endif

            CapitalTextInfo menu_text_info[4] = {{0}};

            // Easy Exit
            if (_currentButtonOneFrame == CIRCLE_BUTTON)
            {
                current_menu = MAIN_MENU;
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }
            DrawTextBox(0x30, 0x1F4, 0x30, 0x98);

            menu_text_info[0].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[0].y = 70;
            menu_text_info[0].size = DEFAULT_SIZE;

            menu_text_info[1].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[1].y = 90;
            menu_text_info[1].size = DEFAULT_SIZE;

            menu_text_info[2].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[2].y = 110;
            menu_text_info[2].size = DEFAULT_SIZE;

            menu_text_info[3].x = SCREEN_LEFT_EDGE + 0x4A;
            menu_text_info[3].y = 130;
            menu_text_info[3].size = DEFAULT_SIZE;

            _spyro.isMovementLocked = TRUE;

#if BUILD == 2 || BUILD == 0 // DECKARD HAS SAVESTATE SLOT
            if (savestate_menu.selection == 0)
            {
                DrawTextCapitals(savestate_menu.stateslot_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(savestate_menu.stateslot_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
#elif BUILD == 1 || BUILD == 3 // GREY OUT OPTION FOR OTHER PLATFORMS

            DrawTextCapitals(savestate_menu.stateslot_text, &menu_text_info[0], DEFAULT_SPACING, MOBY_COLOR_TRANSPARENT);

#endif

            if (savestate_menu.selection == 1)
            {
                DrawTextCapitals(savestate_menu.savestate_button_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(savestate_menu.savestate_button_text, &menu_text_info[1], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

            if (savestate_menu.selection == 2)
            {
                DrawTextCapitals(savestate_menu.loadstate_button_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(savestate_menu.loadstate_button_text, &menu_text_info[2], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }

#if BUILD == 2 || BUILD == 0
            if (savestate_menu.selection == 3)
            {
                DrawTextCapitals(savestate_menu.switch_state_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(savestate_menu.switch_state_button_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
#elif BUILD == 1 || BUILD == 3
            if (savestate_menu.selection == 3)
            {
                DrawTextCapitals(savestate_menu.respawn_on_loadstate_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_GOLD);
            }
            else
            {
                DrawTextCapitals(savestate_menu.respawn_on_loadstate_text, &menu_text_info[3], DEFAULT_SPACING, MOBY_COLOR_PURPLE);
            }
#endif

            // Fill text with defaults if NULL
#if BUILD == 2 || BUILD == 0
            if (savestate_menu.stateslot_text == NULL)
            {
                savestate_menu.stateslot_text = "CURRENT SLOT 1";
                savestate_menu.savestate_button_text = "SAVE BUTTON L3";
                savestate_menu.loadstate_button_text = "LOAD BUTTON R3";
                savestate_menu.switch_state_button_text = "SWITCH SLOT RSTICK";
            }
#elif BUILD == 1 || BUILD == 3
            if (savestate_menu.stateslot_text == NULL)
            {
                savestate_menu.stateslot_text = "CURRENT SLOT 1";
                savestate_menu.savestate_button_text = "SAVE BUTTON L3";
                savestate_menu.loadstate_button_text = "LOAD BUTTON R3";
                savestate_menu.respawn_on_loadstate_text = "RESPAWN ON LOADSTATE ON";
            }
#endif

            // Change Selection
#if BUILD == 2 || BUILD == 0
            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                savestate_menu.selection = (savestate_menu.selection + 1) % 4;
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                savestate_menu.selection = (savestate_menu.selection + 3) % 4;      // +3 because it's the same as -1 in mod 4 math
            }
#elif BUILD == 1 || BUILD == 3
            if (_currentButtonOneFrame == DOWN_BUTTON)
            {
                savestate_menu.selection = savestate_menu.selection % 3 + 1;        // +1 outside the mod operator to avoid it ever being 0
            }
            else if (_currentButtonOneFrame == UP_BUTTON)
            {
                savestate_menu.selection = (savestate_menu.selection + 1) % 3 + 1;  // +1 outside the mod operator to avoid it ever being 0 and +1 inside to make it effectively +2 overal to make it the same as -1
            }
#endif

            // Play Sound Effect
            if (_currentButtonOneFrame == UP_BUTTON || _currentButtonOneFrame == DOWN_BUTTON || _currentButtonOneFrame == LEFT_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON)
            {
                PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
            }

            if (savestate_menu.selection == 0)
            {
#if BUILD == 2 || BUILD == 0
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    savestate_selection = (savestate_selection + 1) % 3;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    savestate_selection = (savestate_selection + 2) % 3;
                }

                if (savestate_selection == 0)
                {
                    savestate_menu.stateslot_text = "CURRENT SLOT 1";
                }
                else if (savestate_selection == 1)
                {
                    savestate_menu.stateslot_text = "CURRENT SLOT 2";
                }
                else
                {
                    savestate_menu.stateslot_text = "CURRENT SLOT 3";
                }
#endif
            }

            else if (savestate_menu.selection == 1)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON)
                {
                    savestate_button_index = (savestate_button_index + 1) % 3;
                }
                else if (_currentButtonOneFrame == LEFT_BUTTON)
                {
                    savestate_button_index = (savestate_button_index + 2) % 3;
                }

                if (savestate_button_index == 0)
                {
                    savestate_menu.savestate_button_text = "SAVE BUTTON L3";
                }
                else if (savestate_button_index == 1)
                {
                    savestate_menu.savestate_button_text = "SAVE BUTTON START";
                }
                else
                {
                    savestate_menu.savestate_button_text = "SAVE BUTTON L3 X2";
                }
            }

            else if (savestate_menu.selection == 2)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    loadstate_button_index = (loadstate_button_index + 1) % 2;
                }

                if (loadstate_button_index == 0)
                {
                    savestate_menu.loadstate_button_text = "LOAD BUTTON R3";
                    ChangeInventoryMenu(ON);
                }
                else
                {
                    savestate_menu.loadstate_button_text = "LOAD BUTTON SELECT";
                    ChangeInventoryMenu(OFF);
                }
            }

#if BUILD == 2 || BUILD == 0
            else if (savestate_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    switch_state_button_index = (switch_state_button_index + 1) % 2;
                }

                if (switch_state_button_index == 0)
                {
                    savestate_menu.switch_state_button_text = "SWITCH SLOT RSTICK";
                }
                else
                {
                    savestate_menu.switch_state_button_text = "SWITCH SLOT L1 R1 DPAD";
                }
            }
#elif BUILD == 1 || BUILD == 3
            else if (savestate_menu.selection == 3)
            {
                if (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
                {
                    respawn_on_loadstate = (respawn_on_loadstate + 1) % 2;
                }

                if (respawn_on_loadstate == 1)
                {
                    savestate_menu.respawn_on_loadstate_text = "RESPAWN ON LOADSTATE ON";
                }
                else
                {
                    savestate_menu.respawn_on_loadstate_text = "RESPAWN ON LOADSTATE OFF";
                }
            }
#endif
        }
    }

    // Has Released Menu Button
    if (!(_currentButton & L2_BUTTON + R2_BUTTON + TRIANGLE_BUTTON))
    {
        has_toggled_menu = FALSE;
    }

    // Has Released Button
    if (!(_currentButton & L1_BUTTON + R1_BUTTON + CIRCLE_BUTTON))
    {
        isHeld = FALSE;
    }
    // Has Released Button
    if (!(_currentButton & L1_BUTTON + R1_BUTTON + TRIANGLE_BUTTON))
    {
        isHeld = FALSE;
    }

    //! Checks
    {
        if (misc_menu.sparx_mode == PERMA_SPARX_ON)
        {
            if (_spyro.health > 0)
            {
                _spyro.health = 3;
            }
        }
        else if (misc_menu.sparx_mode == SPARXLESS)
        {
            if (_spyro.health > 0)
            {
                _spyro.health = 0;
            }
        }

        if (misc_menu.quick_goop_mode == TRUE)
        {
            if (_spyro.drownTimer > 100)
            {
                _spyro.drownTimer = 0x240;
            }
        }

        if (show_sparx_range_mode == true)
        {
            DrawSparxRange();
        }
    }

    //! DRAGON TOUCH
    if (misc_menu.show_dragon_touch && _dragonState > 2 && _dragonState < 7)
    {
        CapitalTextInfo timer_text_info = {0};
        timer_text_info.x = SCREEN_RIGHT_EDGE - 0x40;
        timer_text_info.y = SCREEN_TOP_EDGE + 0x20;
        timer_text_info.size = DEFAULT_SIZE;
        char temp[4];
        sprintf(temp, "%dF", (_dragonWalkTime - 64) / 2);
        DrawTextCapitals(temp, &timer_text_info, DEFAULT_SPACING, MOBY_COLOR_GOLD);

        MyHudMoby dragon_logo = {.position.x = SCREEN_RIGHT_EDGE - 0x54, .position.y = SCREEN_TOP_EDGE + 0x27, .position.z = 3900};
        CustomDrawMoby(MOBY_ID_DRAGON_FIGURINE, &dragon_logo, MOBY_COLOR_GOLD);
    }

    // if(((timer_menu.timer_display_mode == TIMER_ALWAYS || il_menu.il_timer_display_mode == IL_TIMER_ALWAYS || menu_state == MENU_DISPLAYING) && _gameState == GAMESTATE_GAMEPLAY) || (il_timer_state == IL_DISPLAYING && _gameState == GAMESTATE_LOADING) || ((il_menu.display_on_dragon == TRUE || misc_menu.show_dragon_touch == TRUE) && _gameState == GAMESTATE_DRAGON_STATE))
    // {
    //     //printf("RENDERING\n");
    //     RenderShadedMobyQueue();
    // }
}