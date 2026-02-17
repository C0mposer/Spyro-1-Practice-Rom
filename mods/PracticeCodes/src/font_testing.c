#include <common.h>

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

void FontTesting(void)
{
    if (_gameState == GAMESTATE_GAMEPLAY)
    {
        DrawTextSimple("Number 15:"
            "Burger King foot lettuce.\n"
            "The last thing you want on your burger king burger\n"
            "is somebodys foot fungus.\n"
            "But as it turns out that might be what you get.\n", 5, 50);

            //DrawText("THIS IS A TEST OF\nCOLORED TEXT 567 890", 70, 70, (RGB) { 0x0, 0x0, 80 }, (RGB) { 0x80, 0, 0 });
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
            current_row += 7;
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
        else if (str[i] == '!') // Exclaimation point
        {
            tex_x_position_offset = 13 * CHAR_PIXEL_SIZE;
            tex_y_position_offset = 0; // Exclaimation point on 1st row
        }
        else if (str[i] == ':') // :
        {
            tex_x_position_offset = 13 * CHAR_PIXEL_SIZE;
            tex_y_position_offset = CHAR_PIXEL_SIZE * 1; // Colon on 2nd row
        }
        else if (str[i] == '?') // Question Mark
        {
            tex_x_position_offset = 10 * CHAR_PIXEL_SIZE;
            tex_y_position_offset = CHAR_PIXEL_SIZE * 2; // Question Mark on 2nd row
        }
        else if (str[i] == '-') // -
        {
            tex_x_position_offset = 11 * CHAR_PIXEL_SIZE;
            tex_y_position_offset = CHAR_PIXEL_SIZE * 2; // - on 2nd row
        }
        else if (str[i] == '%') // %
        {
            tex_x_position_offset = 12 * CHAR_PIXEL_SIZE;
            tex_y_position_offset = CHAR_PIXEL_SIZE * 2; // % on 2nd row
        }
        else if (str[i] == '.') // .
        {
            tex_x_position_offset = 13 * CHAR_PIXEL_SIZE;
            tex_y_position_offset = CHAR_PIXEL_SIZE * 2; // . on 2nd row
        }

        RECT dest;
        dest.h = CHAR_PIXEL_SIZE;
        dest.w = CHAR_PIXEL_SIZE;
        dest.x = x + (i_offset_with_newline * (font == FONT_SERIF ? 8 : 7));
        dest.y = y + current_row + lowercase_offset;

        RECT texture;
        texture.x = 4000 + tex_x_position_offset;
        texture.y = 364 + tex_y_position_offset;
        texture.w = CHAR_PIXEL_SIZE;
        texture.h = CHAR_PIXEL_SIZE;

        Vec2 clut;
        clut.x = 1008;


        if (font == FONT_STROKE) {
            if (case_type == UPPERCASE) {
                clut.y = 399;
            }
            else {
                clut.y = 397;
            }

            DrawTexture(&dest, &clut, &texture, &stroke_color);

            if (clut.y == 399) {              //if default stroke
                clut.y = 398;
            }               //default letters/symbols
            else {
                clut.y = 396;               //lowercase letters
            }
        }
        else {
            clut.y = 395;
        }

        DrawTexture(&dest, &clut, &texture, &text_color);
    }
}

DrawTextSimple(const char* str, int x, int y)
{
    RGB text_color = { 0x80,0x80,0x80 };
    RGB background_color = { 0x1,0x1,0x1 };

    DrawText(str, x, y, FONT_STROKE, text_color, background_color);
}