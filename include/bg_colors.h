#ifndef BG_COLORS_H
#define BG_COLORS_H

typedef struct RedGreen
{
    byte r;
    byte g;
}RedGreen;

typedef enum BackgroundColor
{
    BG_BLUE,
    BG_PURPLE,
    BG_TEAL,
    BG_GREY,
    BG_PINK,
    BG_ORANGE,
    BG_YELLOW
}BackgroundColor;

typedef enum SpyroColor
{
    SKIN_DEFAULT,
    SKIN_DARK_RED,
    SKIN_ICY,
    SKIN_CORAL_BLUE,
    SKIN_EMERALD,
    SKIN_GOLD,
    SKIN_EMBER,
    //SKIN_PETE,
    SKIN_ZERA,
    SKIN_BERRY,
    SKIN_PIXIE,
    SKIN_JAYO,
    SKIN_TRANS,
    SKIN_DITTO,
    SKIN_GOTH,
    SKIN_CYNDER,
    SKIN_CYNDER_CREST
}SpyroColor;




#endif /* BG_COLORS_H */
