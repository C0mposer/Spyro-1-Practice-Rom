#include <common.h>
#include <deckard_strings.h>

// put in deckard_strings.c for ps2 compatability
extern const char special_keys[21];
extern const char special_vals[20][2];

enum FontType
{
    FONT_STROKE,
    FONT_SERIF
};
typedef enum FontType FontType;

enum CaseType
{
    UPPERCASE,
    LOWERCASE
};

int my_strlen(const char* str) {
    int length = 0; // Use size_t as it is the standard return type for strlen
    while (str[length] != '\0') { // Loop until the null character is found
        length++; // Increment the counter for each character
    }
    return length; // Return the final count
}


void DrawTexture(RECT* dest, Vec2* clut, RECT* texture, int* color) {

    int uv[4];

    int clutPrim = (clut->y << 6) | (clut->x / 16);
    int tpagePrim = ((texture->y / 256) << 4) | (texture->x / 256);
    int textureX1 = (texture->x % (64 * 4));
    int textureX2 = textureX1 + texture->w;
    int textureY1 = texture->y % 256;
    int textureY2 = textureY1 + texture->h;

    if (textureX2 >= 256) {
        textureX2 = 255;
    }

    if (textureY2 >= 256) {
        textureY2 = 255;
    }


    uv[0] = (clutPrim << 16) | (textureY1 << 8) | textureX1;
    uv[1] = (tpagePrim << 16) | (textureY1 << 8) | textureX2;
    uv[2] = (textureY2 << 8) | textureX1;
    uv[3] = (textureY2 << 8) | textureX2;

    int* puVar1;
    char uVar2;

    puVar1 = _ptr_graphicsRelated;
    *_ptr_graphicsRelated = 0x9000000;
    *(char*)((int)puVar1 + 7) = 0x2c;
    if (color == (int*)0x0) {
        uVar2 = 0x80;
        *(char*)(puVar1 + 1) = 0x80;
        *(char*)((int)puVar1 + 5) = 0x80;
    }
    else {
        *(char*)(puVar1 + 1) = (char)*color;
        *(char*)((int)puVar1 + 5) = (char)color[1];
        uVar2 = (char)color[2];
    }
    *(char*)((int)puVar1 + 6) = uVar2;
    *(short*)(puVar1 + 2) = dest->x;
    *(short*)((int)puVar1 + 10) = dest->y;
    *(short*)(puVar1 + 4) = dest->x + dest->w;
    *(short*)((int)puVar1 + 0x12) = dest->y;
    *(short*)(puVar1 + 6) = dest->x;
    *(short*)((int)puVar1 + 0x1a) = dest->y + dest->h;
    *(short*)(puVar1 + 8) = dest->x + dest->w;
    *(short*)((int)puVar1 + 0x22) = dest->y + dest->h;
    puVar1[3] = *uv;
    puVar1[5] = uv[1];
    puVar1[7] = uv[2];
    puVar1[9] = uv[3];

    DrawPrimitive(puVar1);
    _ptr_graphicsRelated = puVar1 + 10;
}


static int dest_h = 7;
static int dest_w = 7;
static int texture_h = 7;
static int texture_w = 7;

int testfont = 0;

void FontTesting(void)
{
    if (_gameState == GAMESTATE_GAMEPLAY)
    {
        testfont += 1;
        if (testfont == 100) {
            testfont = 0;
        }
        DrawTextSimple("Number 15: "
            "Burger King foot lettuce.\n"
            "The last thing you want on your burger king burger\n"
            "is somebodys foot fungus.\n"
            "But as it turns out that might be what you get.\n", 6, 50);

        DrawText("#This is a sentence that I typed, \"but it's got\"\nsome special stuff!\n) hehe )c <= I hope that worked [OX].", testfont, 90, 0, (RGB) { 0x80, 0x0, 80 }, (RGB) { 0, 0, 0 });
        DrawText("#This is a sentence that I typed, \"but it's got\"\nsome special stuff!\n) hehe )c <= I hope that worked [OX].", testfont, 120, 1, (RGB) { 0x80, 0x0, 80 }, (RGB) { 0, 0, 0 });
        DrawText("[", 5, 150, 0, (RGB) { 62, 227, 161 }, (RGB) { 0, 0, 0 });
        DrawText("O", 12, 150, 0, (RGB) { 255, 60, 60 }, (RGB) { 0, 0, 0 });
        DrawText("X", 19, 150, 0, (RGB) { 70, 70, 225 }, (RGB) { 0, 0, 0 });
        DrawText("]", 26, 150, 0, (RGB) { 246, 70, 200 }, (RGB) { 0, 0, 0 });

        DrawText("[OX]", 4, 170, 1, (RGB) { 0, 0, 0 }, (RGB) { 0, 0, 0 });
        DrawText("[OX]", 6, 170, 1, (RGB) { 0, 0, 0 }, (RGB) { 0, 0, 0 });
        DrawText("[OX]", 5, 171, 1, (RGB) { 0, 0, 0 }, (RGB) { 0, 0, 0 });
        DrawText("[OX]", 5, 169, 1, (RGB) { 0, 0, 0 }, (RGB) { 0, 0, 0 });

        DrawText("[", 5, 170, 1, (RGB) { 62, 227, 161 }, (RGB) { 0, 0, 0 });
        DrawText("O", 13, 170, 1, (RGB) { 255, 60, 60 }, (RGB) { 0, 0, 0 });
        DrawText("X", 21, 170, 1, (RGB) { 50, 70, 225 }, (RGB) { 0, 0, 0 });
        DrawText("]", 29, 170, 1, (RGB) { 246, 70, 200 }, (RGB) { 0, 0, 0 });
    }
}


void DrawText(const char* str, int x, int y, FontType font, RGB text_color, RGB stroke_color)
{
    #define CHAR_PER_ROW 13
    #define CHAR_PIXEL_SIZE 7

    int length = my_strlen(str);

    // Newline specific data
    int newline_offset = 0;
    int i_offset_with_newline = 0;
    int current_row = 0;
    int case_type;

    const int SPECIAL_COUNT = sizeof(special_keys) - 1;

    for (int i = 0; i < length; i++)
    {

        char tex_x_position_offset;
        char tex_y_position_offset;

        int lowercase_offset = 0;

        // Handle spaces
        if (str[i] == ' ')
        {
            continue;
        }

        // Handle newlines
        if (str[i] == '\n')
        {
            current_row += 1;
            newline_offset = i + 1;
            continue;
        }
        i_offset_with_newline = i - newline_offset;

        case_type = UPPERCASE;

        // Handle lowercase letters
        if (str[i] >= 'a' && str[i] <= 'z')
        {
            if (font == FONT_STROKE) {
                if (str[i] == 'g' || str[i] == 'j' || str[i] == 'p' || str[i] == 'q' || str[i] == 'y') {
                    lowercase_offset = 1;
                }
                case_type = LOWERCASE;
            }

            tex_x_position_offset = ((str[i] - 'a') % CHAR_PER_ROW) * CHAR_PIXEL_SIZE;
            tex_y_position_offset = CHAR_PIXEL_SIZE * ((str[i] - 'a') / CHAR_PER_ROW);
        }

        // Handle base letters
        if (str[i] >= 'A' && str[i] <= 'Z')
        {
            tex_x_position_offset = ((str[i] - 'A') % CHAR_PER_ROW) * CHAR_PIXEL_SIZE;
            tex_y_position_offset = CHAR_PIXEL_SIZE * ((str[i] - 'A') / CHAR_PER_ROW);
        }

        // Handle base numbers
        else if (str[i] >= '0' && str[i] <= '9')
        {
            tex_x_position_offset = ((str[i] - '0') % CHAR_PER_ROW) * CHAR_PIXEL_SIZE;
            tex_y_position_offset = CHAR_PIXEL_SIZE * 2; // Numbers on third row of font table
        }

        // Handle special characters
        else {
            int j;
            for (j = 0; j < SPECIAL_COUNT; j++) {
                if (str[i] == special_keys[j]) {
                    tex_x_position_offset = special_vals[j][0] * CHAR_PIXEL_SIZE;
                    tex_y_position_offset = special_vals[j][1] * CHAR_PIXEL_SIZE;
                    if (str[i] == ',') lowercase_offset = 1;
                    break;
                }
            }
        }


        RECT dest;
        dest.h = CHAR_PIXEL_SIZE;
        dest.w = (tex_x_position_offset == 91 ? 4 : CHAR_PIXEL_SIZE);
        dest.x = x + (i_offset_with_newline * (font == FONT_SERIF ? 8 : 7));
        dest.y = y + current_row * (font == FONT_SERIF ? 8 : 7) + lowercase_offset;

        RECT texture;
        texture.x = 4000 + tex_x_position_offset;
        texture.y = 364 + tex_y_position_offset;
        texture.w = CHAR_PIXEL_SIZE;
        texture.h = CHAR_PIXEL_SIZE;

        Vec2 clut;
        clut.x = 1008;


        if (font == FONT_STROKE) {
            clut.y = (case_type == UPPERCASE) ? 399 : 397;      //first draws the stroke and chooses which based on upper or lower case
            DrawTexture(&dest, &clut, &texture, &stroke_color);
            clut.y--;                                           //then decrements the clut value to get to draw the actual characters
        }
        else {
            clut.y = 395;                                       //alternate font
        }

        DrawTexture(&dest, &clut, &texture, &text_color);
    }
}

void DrawTextSimple(const char* str, int x, int y)
{
    RGB text_color = { 0x80,0x80,0x80 };
    RGB background_color = { 0x1,0x1,0x1 };

    DrawText(str, x, y, FONT_STROKE, text_color, background_color);
}

void DrawTextSelected(const char* str, int x, int y, bool is_selected)
{
    RGB text_color;
    RGB background_color;
    if (is_selected)
    {
        text_color = (RGB){ 0x80,0x80,0x0 };
        background_color = (RGB){ 0x1,0x1,0x1 };
    }
    else
    {
        text_color = (RGB){ 0x80,0x80,0x80 };
        background_color = (RGB){ 0x1,0x1,0x1 };
    }
    DrawText(str, x, y, FONT_STROKE, text_color, background_color);
}


enum { LOCKED, UNSELECTED, SELECTED };
void DrawTextPossiblyLocked(const char* str, int x, int y, int color)
{
    RGB text_color;
    RGB background_color;
    if (color == LOCKED)
    {
        text_color = (RGB){ 0x20,0x20,0x20 };
        background_color = (RGB){ 0x1,0x1,0x1 };
    }
    else if (color == UNSELECTED)
    {
        text_color = (RGB){ 0x80,0x80,0x80 };
        background_color = (RGB){ 0x1,0x1,0x1 };
    }
    else
    {
        text_color = (RGB){ 0x80,0x80,0x00 };
        background_color = (RGB){ 0x1,0x1,0x1 };
    }
    DrawText(str, x, y, FONT_STROKE, text_color, background_color);
}