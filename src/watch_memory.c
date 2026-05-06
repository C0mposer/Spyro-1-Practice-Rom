#include <common.h>
#include <deckard_strings.h>
#include <custom_menu.h>
#include <shared_funcs.h>
#include <sound.h>

extern void DrawTextSimple(const char* str, int x, int y);
extern void DrawTextSelected(const char* str, int x, int y, bool is_selected);
extern void StopDrawWorldAndObjects(void);
extern void RestartDrawWorldAndObjects(void);

extern MenuState menu_state;
extern CurrentMenu current_menu;
extern VisualizerMenu visualizer_menu;

#define MAX_MEMORY_WATCHES 10
#define ADDRESS_DIGITS_MAX 8
#define WATCH_LABEL_MAX 10
#define FIXED_12_20_SCALE 1048576

typedef enum MemoryWatchMenuState
{
    MEMORY_WATCH_CLOSED,
    MEMORY_WATCH_LIST,
    MEMORY_WATCH_SLOT,
    MEMORY_WATCH_ADDRESS,
    MEMORY_WATCH_LABEL
} MemoryWatchMenuState;

typedef enum MemoryWatchType
{
    WATCH_TYPE_U8,
    WATCH_TYPE_U16,
    WATCH_TYPE_U32,
    WATCH_TYPE_S8,
    WATCH_TYPE_S16,
    WATCH_TYPE_S32,
    WATCH_TYPE_FIXED_12_20,
    WATCH_TYPE_COUNT
} MemoryWatchType;

typedef struct MemoryWatch
{
    unsigned int address;
    char label[WATCH_LABEL_MAX + 1];
    MemoryWatchType type;
    bool show_hex;
    bool enabled;
} MemoryWatch;

static MemoryWatch watches[MAX_MEMORY_WATCHES] = { 0 };
static int watch_count = 0;
static MemoryWatchMenuState watch_menu_state = MEMORY_WATCH_CLOSED;
static int watch_menu_selection = 0;
static int selected_watch = 0;
static int slot_menu_selection = 0;
static int input_lockout = 0;
static bool adding_new_watch = false;

static char edit_digits[ADDRESS_DIGITS_MAX + 1] = { 0 };
static int edit_cursor = 0;
static unsigned int edit_original_address = 0;
static char label_edit[WATCH_LABEL_MAX + 1] = { 0 };
static int label_cursor = 0;
static const char WATCH_LABEL_CHARSET[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-";

static const char* WATCH_TYPE_NAMES[WATCH_TYPE_COUNT] = {
    deckard_str_watch_type_u8,
    deckard_str_watch_type_u16,
    deckard_str_watch_type_u32,
    deckard_str_watch_type_s8,
    deckard_str_watch_type_s16,
    deckard_str_watch_type_s32,
    deckard_str_watch_type_fixed
};

static bool IsUnsignedType(MemoryWatchType type)
{
    return type == WATCH_TYPE_U8 || type == WATCH_TYPE_U16 || type == WATCH_TYPE_U32;
}

static unsigned int AlignAddress(unsigned int address, MemoryWatchType type)
{
    if (type == WATCH_TYPE_U16 || type == WATCH_TYPE_S16)
    {
        return address & 0xFFFFFFFE;
    }
    if (type == WATCH_TYPE_U32 || type == WATCH_TYPE_S32 || type == WATCH_TYPE_FIXED_12_20)
    {
        return address & 0xFFFFFFFC;
    }
    return address;
}

static char HexChar(int value)
{
    value &= 0xF;
    if (value < 10) return '0' + value;
    return 'A' + (value - 10);
}

static int HexValue(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

static void FormatHexFixed(char* out, unsigned int value, int digits)
{
    int pos = 0;
    out[pos++] = '0';
    out[pos++] = 'x';
    for (int i = digits - 1; i >= 0; i--)
    {
        out[pos++] = HexChar(value >> (i * 4));
    }
    out[pos] = '\0';
}

static unsigned int ParseEditDigits(void)
{
    unsigned int value = 0;
    for (int i = 0; i < ADDRESS_DIGITS_MAX; i++)
    {
        value = (value << 4) | HexValue(edit_digits[i]);
    }
    return value;
}

static void CopyAddressToEditDigits(unsigned int address)
{
    for (int i = 7; i >= 0; i--)
    {
        int digit = (address >> (i * 4)) & 0xF;
        edit_digits[7 - i] = HexChar(digit);
    }
    edit_digits[ADDRESS_DIGITS_MAX] = '\0';
    edit_cursor = ADDRESS_DIGITS_MAX - 1;
}

static int LabelCharIndex(char c)
{
    int i;

    for (i = 0; WATCH_LABEL_CHARSET[i] != '\0'; i++)
    {
        if (WATCH_LABEL_CHARSET[i] == c) return i;
    }
    return 0;
}

static void NormalizeLabel(char* label)
{
    int end;

    label[WATCH_LABEL_MAX] = '\0';
    end = WATCH_LABEL_MAX - 1;
    while (end >= 0 && label[end] == ' ')
    {
        label[end] = '\0';
        end--;
    }
}

static bool WatchHasLabel(const MemoryWatch* watch)
{
    return watch->label[0] != '\0';
}

static const char* GetWatchDisplayName(const MemoryWatch* watch, char* fallback_address)
{
    if (WatchHasLabel(watch)) return watch->label;
    return fallback_address;
}

static void CopyLabelToEditor(const char* label)
{
    int i;

    for (i = 0; i < WATCH_LABEL_MAX; i++)
    {
        char c = label[i];
        if (c == '\0') break;
        label_edit[i] = c;
    }
    for (; i < WATCH_LABEL_MAX; i++)
    {
        label_edit[i] = ' ';
    }
    label_edit[WATCH_LABEL_MAX] = '\0';
    label_cursor = 0;
}

static void SaveEditorToLabel(char* label)
{
    int i;

    for (i = 0; i < WATCH_LABEL_MAX; i++)
    {
        label[i] = label_edit[i];
    }
    label[WATCH_LABEL_MAX] = '\0';
    NormalizeLabel(label);
}

static void ClearLabelEditor(void)
{
    int i;

    for (i = 0; i < WATCH_LABEL_MAX; i++)
    {
        label_edit[i] = ' ';
    }
    label_edit[WATCH_LABEL_MAX] = '\0';
    label_cursor = 0;
}

static void SetDefaultWatch(MemoryWatch* watch)
{
    watch->address = 0x80000000;
    watch->label[0] = '\0';
    watch->type = WATCH_TYPE_U32;
    watch->show_hex = true;
    watch->enabled = true;
    watch->address = AlignAddress(watch->address, watch->type);
}

static void RemoveWatch(int index)
{
    if (index < 0 || index >= watch_count) return;

    for (int i = index; i < watch_count - 1; i++)
    {
        watches[i] = watches[i + 1];
    }
    watch_count--;
    if (selected_watch >= watch_count) selected_watch = watch_count - 1;
    if (selected_watch < 0) selected_watch = 0;
    if (watch_menu_selection > watch_count + 1) watch_menu_selection = watch_count + 1;
}

static bool DuplicateWatch(int index)
{
    if (index < 0 || index >= watch_count) return false;
    if (watch_count >= MAX_MEMORY_WATCHES) return false;

    watches[watch_count] = watches[index];
    watch_count++;
    selected_watch = watch_count - 1;
    return true;
}

static int GetListItemCount(void)
{
    return watch_count + 3; // Add Watch, watches, Clear All, Back
}

static const char* GetFormatText(MemoryWatch* watch)
{
    if (!IsUnsignedType(watch->type)) return deckard_str_watch_fmt_dec;
    return watch->show_hex ? deckard_str_watch_fmt_hex : deckard_str_watch_fmt_dec;
}

static void FormatWatchSummary(char* out, int index)
{
    char address[12];
    MemoryWatch* watch = &watches[index];
    FormatHexFixed(address, watch->address, 8);
    sprintf(out, deckard_str_fmt_watch_summary, index + 1, GetWatchDisplayName(watch, address), WATCH_TYPE_NAMES[watch->type], GetFormatText(watch));
}

static void FormatWatchRawValue(char* value, MemoryWatch* watch)
{
    unsigned int addr = AlignAddress(watch->address, watch->type);

    switch (watch->type)
    {
        case WATCH_TYPE_U8:
        {
            unsigned int v = *(volatile unsigned char*)addr;
            if (watch->show_hex) FormatHexFixed(value, v, 2);
            else sprintf(value, deckard_str_fmt_u, v);
            break;
        }
        case WATCH_TYPE_U16:
        {
            unsigned int v = *(volatile unsigned short*)addr;
            if (watch->show_hex) FormatHexFixed(value, v, 4);
            else sprintf(value, deckard_str_fmt_u, v);
            break;
        }
        case WATCH_TYPE_U32:
        {
            unsigned int v = *(volatile unsigned int*)addr;
            if (watch->show_hex) FormatHexFixed(value, v, 8);
            else sprintf(value, deckard_str_fmt_u, v);
            break;
        }
        case WATCH_TYPE_S8:
        {
            int v = *(volatile signed char*)addr;
            sprintf(value, deckard_str_fmt_d, v);
            break;
        }
        case WATCH_TYPE_S16:
        {
            int v = *(volatile signed short*)addr;
            sprintf(value, deckard_str_fmt_d, v);
            break;
        }
        case WATCH_TYPE_S32:
        {
            int v = *(volatile int*)addr;
            sprintf(value, deckard_str_fmt_d, v);
            break;
        }
        default:
        {
            int raw = *(volatile int*)addr;
            int negative = raw < 0;
            unsigned int abs_raw = negative ? (unsigned int)(-raw) : (unsigned int)raw;
            unsigned int whole = abs_raw / FIXED_12_20_SCALE;
            unsigned int frac = ((abs_raw % FIXED_12_20_SCALE) * 1000) / FIXED_12_20_SCALE;
            if (negative) sprintf(value, deckard_str_fmt_neg_fixed, whole, frac);
            else sprintf(value, deckard_str_fmt_pos_fixed, whole, frac);
            break;
        }
    }
}

static void FormatWatchValue(char* out, MemoryWatch* watch)
{
    char address[12];
    char value[24];
    const char* name;
    unsigned int addr = AlignAddress(watch->address, watch->type);

    FormatHexFixed(address, addr, 8);
    name = GetWatchDisplayName(watch, address);
    FormatWatchRawValue(value, watch);

    sprintf(out, deckard_str_fmt_addr_value, name, value);
}

static void DrawGameplayWatches(void)
{
    int enabled_count = 0;
    for (int i = 0; i < watch_count; i++)
    {
        if (watches[i].enabled) enabled_count++;
    }
    if (enabled_count == 0) return;

    int y = 220 - (enabled_count * 9);
    if (y < 120) y = 120;

    for (int i = 0; i < watch_count; i++)
    {
        if (watches[i].enabled)
        {
            char line[48];
            FormatWatchValue(line, &watches[i]);
            DrawTextSimple(line, 10, y);
            y += 9;
        }
    }
}

void MemoryWatchResetMenuNav(void)
{
    watch_menu_selection = 0;
    slot_menu_selection = 0;
}

static void CloseMemoryWatchMenu(void)
{
    watch_menu_state = MEMORY_WATCH_CLOSED;
    menu_state = MENU_DISPLAYING;
    current_menu = MISC_MENU;
    _spyro.isMovementLocked = TRUE;
}

void MemoryWatchOpenMenu(void)
{
    watch_menu_state = MEMORY_WATCH_LIST;
    watch_menu_selection = 0;
    slot_menu_selection = 0;
    adding_new_watch = false;
    input_lockout = 1;
    menu_state = MENU_HIDDEN;
    current_menu = MISC_MENU;
    _spyro.isMovementLocked = TRUE;
    StopDrawWorldAndObjects();
}

static void DrawListMenu(void)
{
    int x = 10;
    int y = 20;
    int spacing = 9;
    int item_count = GetListItemCount();

    //DrawTextBox(0x30, 0x1D0, 0x30, 0xE0);
    DrawTextSimple(deckard_str_watch_title, x, y);
    y += 14;

    DrawTextSelected(deckard_str_watch_add, x, y, watch_menu_selection == 0);
    y += spacing;

    for (int i = 0; i < watch_count; i++)
    {
        char line[56];
        FormatWatchSummary(line, i);
        DrawTextSelected(line, x, y, watch_menu_selection == i + 1);
        y += spacing;
    }

    DrawTextSelected(deckard_str_watch_clear_all, x, y, watch_menu_selection == watch_count + 1);
    y += spacing;
    DrawTextSelected(deckard_str_watch_back, x, y, watch_menu_selection == watch_count + 2);
    DrawTextSimple(deckard_str_watch_controls_list, x, 206);

    if (input_lockout > 0)
    {
        input_lockout--;
        return;
    }

    if (_currentButtonOneFrame == DOWN_BUTTON)
    {
        watch_menu_selection = (watch_menu_selection + 1) % item_count;
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == UP_BUTTON)
    {
        watch_menu_selection = (watch_menu_selection + item_count - 1) % item_count;
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == CIRCLE_BUTTON)
    {
        PlayMenuSound();
        CloseMemoryWatchMenu();
    }
    else if (_currentButtonOneFrame == X_BUTTON)
    {
        PlayMenuSound();
        if (watch_menu_selection == 0)
        {
            if (watch_count < MAX_MEMORY_WATCHES)
            {
                SetDefaultWatch(&watches[watch_count]);
                selected_watch = watch_count;
                watch_count++;
                edit_original_address = watches[selected_watch].address;
                CopyAddressToEditDigits(watches[selected_watch].address);
                adding_new_watch = true;
                watch_menu_state = MEMORY_WATCH_ADDRESS;
            }
        }
        else if (watch_menu_selection <= watch_count)
        {
            selected_watch = watch_menu_selection - 1;
            slot_menu_selection = 0;
            watch_menu_state = MEMORY_WATCH_SLOT;
        }
        else if (watch_menu_selection == watch_count + 1)
        {
            watch_count = 0;
            watch_menu_selection = 0;
        }
        else
        {
            CloseMemoryWatchMenu();
        }
    }
}

static void DrawSlotMenu(void)
{
    MemoryWatch* watch = &watches[selected_watch];
    char address[12];
    char line[56];
    const char* label_text;
    int x = 10;
    int y = 40;
    int spacing = 9;
    int item_count = 8;

    //DrawTextBox(0x30, 0x1D0, 0x30, 0xE0);
    sprintf(line, deckard_str_fmt_watch_n, selected_watch + 1);
    DrawTextSimple(line, x, y);
    y += 14;

    FormatHexFixed(address, watch->address, 8);
    sprintf(line, deckard_str_fmt_address_colon_s, address);
    DrawTextSelected(line, x, y, slot_menu_selection == 0);
    y += spacing;

    label_text = WatchHasLabel(watch) ? watch->label : deckard_str_watch_label_none;
    sprintf(line, deckard_str_fmt_label_colon_s, label_text);
    DrawTextSelected(line, x, y, slot_menu_selection == 1);
    y += spacing;

    sprintf(line, deckard_str_fmt_type_colon_s, WATCH_TYPE_NAMES[watch->type]);
    DrawTextSelected(line, x, y, slot_menu_selection == 2);
    y += spacing;

    sprintf(line, deckard_str_fmt_display_colon_s, GetFormatText(watch));
    DrawTextSelected(line, x, y, slot_menu_selection == 3);
    y += spacing;

    {
        char value[24];
        FormatWatchRawValue(value, watch);
        sprintf(line, deckard_str_fmt_value_colon_s, value);
        DrawTextSimple(line, x, y);
        y += spacing;
    }

    DrawTextSelected(watch->enabled ? deckard_str_watch_enabled_on : deckard_str_watch_enabled_off, x, y, slot_menu_selection == 4);
    y += spacing;
    DrawTextSelected(deckard_str_watch_duplicate, x, y, slot_menu_selection == 5);
    y += spacing;
    DrawTextSelected(deckard_str_watch_remove, x, y, slot_menu_selection == 6);
    y += spacing;
    DrawTextSelected(deckard_str_watch_back, x, y, slot_menu_selection == 7);

    DrawTextSimple(deckard_str_watch_controls_slot, x, 196);
    DrawTextSimple(deckard_str_watch_controls_circle, x, 206);

    if (_currentButtonOneFrame == DOWN_BUTTON)
    {
        slot_menu_selection = (slot_menu_selection + 1) % item_count;
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == UP_BUTTON)
    {
        slot_menu_selection = (slot_menu_selection + item_count - 1) % item_count;
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == CIRCLE_BUTTON)
    {
        PlayMenuSound();
        watch_menu_state = MEMORY_WATCH_LIST;
        watch_menu_selection = selected_watch + 1;
    }
    else if (_currentButtonOneFrame == X_BUTTON || _currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON)
    {
        PlayMenuSound();
        if (slot_menu_selection == 0 && _currentButtonOneFrame == X_BUTTON)
        {
            edit_original_address = watch->address;
            CopyAddressToEditDigits(watch->address);
            adding_new_watch = false;
            watch_menu_state = MEMORY_WATCH_ADDRESS;
        }
        else if (slot_menu_selection == 1 && _currentButtonOneFrame == X_BUTTON)
        {
            CopyLabelToEditor(watch->label);
            watch_menu_state = MEMORY_WATCH_LABEL;
        }
        else if (slot_menu_selection == 2)
        {
            if (_currentButtonOneFrame == LEFT_BUTTON)
                watch->type = (watch->type + WATCH_TYPE_COUNT - 1) % WATCH_TYPE_COUNT;
            else
                watch->type = (watch->type + 1) % WATCH_TYPE_COUNT;

            if (!IsUnsignedType(watch->type)) watch->show_hex = false;
            watch->address = AlignAddress(watch->address, watch->type);
        }
        else if (slot_menu_selection == 3 && IsUnsignedType(watch->type))
        {
            watch->show_hex = !watch->show_hex;
        }
        else if (slot_menu_selection == 4)
        {
            watch->enabled = !watch->enabled;
        }
        else if (slot_menu_selection == 5 && _currentButtonOneFrame == X_BUTTON)
        {
            if (DuplicateWatch(selected_watch))
            {
                slot_menu_selection = 0;
                watch_menu_state = MEMORY_WATCH_SLOT;
            }
        }
        else if (slot_menu_selection == 6 && _currentButtonOneFrame == X_BUTTON)
        {
            RemoveWatch(selected_watch);
            watch_menu_state = MEMORY_WATCH_LIST;
            watch_menu_selection = selected_watch + 1;
            if (watch_menu_selection > watch_count) watch_menu_selection = watch_count;
        }
        else if (slot_menu_selection == 7 && _currentButtonOneFrame == X_BUTTON)
        {
            watch_menu_state = MEMORY_WATCH_LIST;
            watch_menu_selection = selected_watch + 1;
        }
    }
}

static void DrawAddressEditor(void)
{
    MemoryWatch* watch = &watches[selected_watch];
    char digit[2];
    int x = 10;
    int y = 50;

    //DrawTextBox(0x30, 0x1D0, 0x30, 0xE0);
    DrawTextSimple(deckard_str_watch_edit_title, x, 40);

    DrawTextSimple(deckard_str_watch_edit_0x, x, y);
    digit[1] = '\0';
    for (int i = 0; i < ADDRESS_DIGITS_MAX; i++)
    {
        digit[0] = edit_digits[i];
        DrawTextSelected(digit, x + ((i + 2) * 7), y, i == edit_cursor);
    }

    DrawTextSimple(deckard_str_watch_edit_controls, x, 176);
    DrawTextSimple(deckard_str_watch_x_save_circle_back, x, 186);

    if (_currentButtonOneFrame == UP_BUTTON)
    {
        edit_digits[edit_cursor] = HexChar(HexValue(edit_digits[edit_cursor]) + 1);
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == DOWN_BUTTON)
    {
        edit_digits[edit_cursor] = HexChar(HexValue(edit_digits[edit_cursor]) - 1);
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == LEFT_BUTTON)
    {
        if (edit_cursor > 0) edit_cursor--;
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == RIGHT_BUTTON)
    {
        if (edit_cursor < ADDRESS_DIGITS_MAX - 1) edit_cursor++;
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == X_BUTTON)
    {
        watch->address = AlignAddress(ParseEditDigits(), watch->type);
        adding_new_watch = false;
        watch_menu_state = MEMORY_WATCH_SLOT;
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == CIRCLE_BUTTON)
    {
        if (adding_new_watch)
        {
            RemoveWatch(selected_watch);
            watch_menu_state = MEMORY_WATCH_LIST;
            watch_menu_selection = 0;
            adding_new_watch = false;
        }
        else
        {
            watch->address = edit_original_address;
            watch_menu_state = MEMORY_WATCH_SLOT;
        }
        PlayMenuSound();
    }
}

static void DrawLabelEditor(void)
{
    MemoryWatch* watch = &watches[selected_watch];
    char glyph[2];
    int x = 10;
    int y = 50;
    int index;

    DrawTextSimple(deckard_str_watch_label_edit_title, x, 40);

    glyph[1] = '\0';
    for (int i = 0; i < WATCH_LABEL_MAX; i++)
    {
        glyph[0] = label_edit[i] == ' ' ? '.' : label_edit[i];
        DrawTextSelected(glyph, x + (i * 8), y, i == label_cursor);
    }

    DrawTextSimple(deckard_str_watch_edit_controls, x, 176);
    DrawTextSimple(deckard_str_watch_x_save_circle_back, x, 186);
    DrawTextSimple(deckard_str_watch_triangle_reset_label, x, 196);

    if (_currentButtonOneFrame == UP_BUTTON)
    {
        index = LabelCharIndex(label_edit[label_cursor]);
        label_edit[label_cursor] = WATCH_LABEL_CHARSET[(index + 1) % ((int)sizeof(WATCH_LABEL_CHARSET) - 1)];
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == DOWN_BUTTON)
    {
        index = LabelCharIndex(label_edit[label_cursor]);
        index--;
        if (index < 0) index = ((int)sizeof(WATCH_LABEL_CHARSET) - 2);
        label_edit[label_cursor] = WATCH_LABEL_CHARSET[index];
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == LEFT_BUTTON)
    {
        if (label_cursor > 0) label_cursor--;
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == RIGHT_BUTTON)
    {
        if (label_cursor < WATCH_LABEL_MAX - 1) label_cursor++;
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == X_BUTTON)
    {
        SaveEditorToLabel(watch->label);
        watch_menu_state = MEMORY_WATCH_SLOT;
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == TRIANGLE_BUTTON)
    {
        ClearLabelEditor();
        PlayMenuSound();
    }
    else if (_currentButtonOneFrame == CIRCLE_BUTTON)
    {
        watch_menu_state = MEMORY_WATCH_SLOT;
        PlayMenuSound();
    }
}

void MemoryWatchUpdate(void)
{
    if (_gameState != GAMESTATE_GAMEPLAY) return;

    if (watch_menu_state == MEMORY_WATCH_CLOSED)
    {
        if (menu_state == MENU_HIDDEN)
        {
            DrawGameplayWatches();
            if (visualizer_menu.hitbox_viewer)
            {
                HitboxViewerUpdate();
            }
        }
        return;
    }

    _spyro.isMovementLocked = TRUE;

    switch (watch_menu_state)
    {
        case MEMORY_WATCH_LIST:
        DrawListMenu();
        break;
        case MEMORY_WATCH_SLOT:
        DrawSlotMenu();
        break;
        case MEMORY_WATCH_ADDRESS:
        DrawAddressEditor();
        break;
        case MEMORY_WATCH_LABEL:
        DrawLabelEditor();
        break;
        default:
        break;
    }
}
