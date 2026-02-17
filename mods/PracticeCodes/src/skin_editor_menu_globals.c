#include <common.h>
#include <custom_menu.h>

SkinEditorMenu skin_editor_menu = { 0 };

char hue_text_buf[20] = { 0 };
char saturation_text_buf[20] = { 0 };
char value_text_buf[20] = { 0 };

// Have to copy the strings here, because when I do something like char* thing = "thing"; this will put it in .rodata in the deckard BE region. 
const char* hue_text_str = "HUE %d";
const char* sat_text_str = "SAT %d";
const char* light_text_str = "LIGHT %d";
const char* section_strings[7] = { "SECTION BODY", "SECTION BELLY", "SECTION WING", "SECTION WINGBONE", "SECTION HORNS", "SECTION CREST", "SECTION EYES" };
const char* save_str = "SAVE";

int right_press_timer = 0;
int left_press_timer = 0;