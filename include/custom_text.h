#ifndef CUSTOM_TEXT_H
#define CUSTOM_TEXT_H

#define DEFAULT_SPACING 0x12
#define DEFAULT_SIZE 0x1100
#define DEFAULT_ROW 0x0

enum MobyColors
{
    MOBY_COLOR_TRANSPARENT = 0x00,
    MOBY_COLOR_WHITE = 0x01,
    MOBY_COLOR_BLACK = 0x06,
    MOBY_COLOR_MATTE_GREY = 0x07,
    MOBY_COLOR_EMERALD_GREEN = 0x09,
    MOBY_COLOR_BLUE = 0x0A,
    MOBY_COLOR_GOLD = 0x0B,
    MOBY_COLOR_GREY = 0x0C,
    MOBY_COLOR_RED = 0x0D,
    MOBY_COLOR_GREEN = 0x0E,
    MOBY_COLOR_PURPLE = 0x17,
    MOBY_COLOR_NEON_YELLOW = 0x18,
    MOBY_COLOR_CHEESE_YELLOW = 0x19,
    MOBY_COLOR_DARK_RED = 0x1F,
    MOBY_COLOR_NEON_RED = 0x20,
    MOBY_COLOR_DARK_GREEN = 0x21,
    MOBY_COLOR_NEON_GREEN = 0x22,
    MOBY_COLOR_NEON_BLUE = 0x24,
    MOBY_COLOR_DARK_GOLD = 0x25,
    MOBY_COLOR_DARK_PURPLE = 0x27,
    MOBY_COLOR_NEON_PURPLE = 0x28,
    MOBY_COLOR_SHINY_RED = 0x29,
    MOBY_COLOR_SHINY_GREEN = 0x2A,
    MOBY_COLOR_SHINY_BLUE = 0x2B,
    MOBY_COLOR_SHINY_GOLD = 0x2C,
    MOBY_COLOR_SHINY_PURPLE = 0x2D,
    MOBY_COLOR_SILVER = 0x2E,
    MOBY_COLOR_MATTE_RED = 0x30,
    MOBY_COLOR_SKY_BLUE = 0x39,
    MOBY_COLOR_CYAN = 0x7E,
    MOBY_COLOR_PINK = 0xA5,
    MOBY_COLOR_INDIGO = 0xA6,
    MOBY_COLOR_MATTE_GREEN = 0xAC,
    MOBY_COLOR_NEON_LIME = 0xAD,
    MOBY_COLOR_BRIGHT_BLUE = 0xB1,
    MOBY_COLOR_VIOLET = 0xB2,
    MOBY_COLOR_BRIGHT_RED = 0xB3

};

//~~~~~~~
//Structs
//~~~~~~~
typedef struct CapitalTextInfo
{
    int x;                                  
    int y;                                  
    int size;                                        //Higher number, smaller size

}CapitalTextInfo;


struct LowercaseTextInfo
{
    int spacing;
    int yOffset;
    int size;                                        //Higher number, smaller size
};

void MobyRender();

#endif /* CUSTOM_TEXT_H */
