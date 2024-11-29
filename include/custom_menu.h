#ifndef CUSTOM_MENU_H
#define CUSTOM_MENU_H

//! Enums
typedef enum MenuState
{
    MENU_HIDDEN,
    MENU_DISPLAYING

} MenuState;

typedef enum CurrentMenu
{
    MAIN_MENU,
    IL_MENU,
    TIMER_MENU,
    SAVESTATE_MENU,
    MISC_MENU,
    COSMETIC_MENU
} CurrentMenu;

typedef enum TimerState
{
    TIMER_RUNNING,
    TIMER_DISPLAYING

} TimerState;

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

typedef enum ConsistencyTrackerMode
{
    CONSISTENCY_TRACKER_OFF,
    CONSISTENCY_TRACKER_ON_CHANGED,
    CONSISTENCY_TRACKER_ALWAYS

} ConsistencyTrackerMode;

typedef enum SuperflyMode
{
    SUPERFLY_NOT_SET,
    SUPERFLY_TURNED_ON,
    SUPERFLY_TURNED_OFF
} SuperflyMode;

//! Structs
typedef struct Menu
{
    int selection;
    char* il_menu_text;
    char* timer_menu_text;
    char* savestate_menu_text;
    char* misc_menu_text;
    char* cosmetic_menu_text;
} Menu;

typedef struct ILMenu
{
    int selection;
    bool il_state;
    char* il_mode_text;
    ILTimerDisplayMode il_timer_display_mode;
    char* il_timer_display_mode_text;
    bool display_on_dragon;
    char* display_on_dragon_text;
    bool display_on_land;
    char* display_on_land_text;
    bool dont_loop_level;
    char* loop_level_text;

} ILMenu;
typedef struct TimerMenu
{
    int selection;
    bool timer_state;
    char* timer_state_text;
    TimerDisplayMode timer_display_mode;
    char* timer_display_mode_text;
    short stop_timer_button_index;
    char* stop_timer_button_text;
    short reset_timer_mode;
    char* reset_timer_button_text;

} TimerMenu;

#if BUILD == 2 || BUILD == 0
typedef struct SavestateMenu
{
    int selection;
    char* stateslot_text;
    char* savestate_button_text;
    char* loadstate_button_text;
    char* switch_state_button_text;

} SavestateMenu;

#elif BUILD == 1 || BUILD == 3 // GREY OUT OPTION FOR OTHER PLATFORMS
typedef struct SavestateMenu
{
    int selection;
    char* stateslot_text;
    char* savestate_button_text;
    char* loadstate_button_text;
    char* respawn_on_loadstate_text;

} SavestateMenu;
#endif

typedef struct MiscMenu
{
    int selection;
    SparxMode sparx_mode;
    char* sparx_mode_text;
    bool show_dragon_touch;
    char* show_dragon_touch_text;
    char* disable_portal_entry_text;
    bool super_mode;
    char* super_text;
    char* consitency_tracker_text;
    char* show_sparx_range_text;
} MiscMenu;

typedef struct CosmeticMenu
{
    int selection;
    char* bg_color_text;
    char* spyro_color_text;
    char* flame_color_text;
    char* sparx_color_text;
} CosmeticMenu;

//! Prototypes
void CustomMenuUpdate(void);

#endif //CUSTOM_MENU_H