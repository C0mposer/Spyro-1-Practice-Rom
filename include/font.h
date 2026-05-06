#ifndef FONT_H
#define FONT_H
#include <common.h>

typedef enum FontType
{
    FONT_STROKE,
    FONT_SERIF
} FontType;

void DrawTextSimple(const char*, int x, int y);
void DrawText(const char*, int x, int y, FontType font, RGB text_color, RGB stroke_color);
void DrawTextSelected(const char* str, int x, int y, bool is_selected);

#endif /* FONT_H */
