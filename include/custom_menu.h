#ifndef CUSTOM_MENU_H
#define CUSTOM_MENU_H
#include <custom_types.h>

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
    MANUAL_TIMER_MENU,
    SAVESTATE_MENU,
    MISC_MENU,
    COSMETIC_MENU,
    IL_DISPLAY_OPTIONS_MENU,
    SKIN_EDITOR_MENU,
    GHOST_MENU
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
    NORMAL_SPARX,
    SPARXLESS,
    PERMA_SPARX_ON

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
    bool dont_loop_level;
    char* loop_level_text;
    char* display_modes_text;
    char* ghost_menu_text;

} ILMenu;

typedef struct ILDisplayMenu
{
    int selection;

    bool il_display_dragon : 1;
    bool il_display_landing : 1;
    bool il_display_glide : 1;
    bool il_display_flame : 1;
    bool il_display_gem : 1;
    bool il_display_flight : 1;
    bool il_display_whirlwind : 1;
    bool il_display_bonk : 1;
    bool il_has_enabled_all;
    bool il_display_always;

    char* il_display_enable_all_text;
    char* il_display_dragon_text;
    char* il_display_landing_text;
    char* il_display_glide_text;
    char* il_display_flame_text;
    char* il_display_gem_text;
    char* il_display_flight_text;
    char* il_display_whirlwind_text;
    char* il_display_bonk_text;
    char* il_display_always_text;

} ILDisplayMenu;


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

#if BUILD == 2 || BUILD == 0 || BUILD == 5
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
#endif /* CUSTOM_MENU_H */

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
    char* skin_editor_menu_text;
} CosmeticMenu;
typedef struct SkinEditorMenu
{
    int selection;
    int skin_section;
    int hue_level;
    int saturation_level;
    int lightness_level;
    char* skin_section_text;
    char* hue_text;
    char* saturation_text;
    char* value_text;
    char* save_text;
    int* index_ptr;
    int index_array_size;

}SkinEditorMenu;

typedef enum SpyroSections
{
    SECTION_BODY,
    SECTION_BELLY,
    SECTION_WING,
    SECTION_WINGBONE,
    SECTION_HORNS,
    SECTION_CREST,
    SECTION_EYES,
}SpyroSection;

typedef struct GhostMenu
{
    int selection;
    bool ghosts_enabled;
    int ghosts_visual;
    char* ghost_enabled_text;
    char* ghost_visual_text;

}GhostMenu;

enum
{
    VISUAL_SPYRO,
    VISUAL_POLYGON
};

//! Prototypes
void CustomMenuUpdate(void);

#endif //CUSTOM_MENU_H