#ifndef COSMETIC_H
#define COSMETIC_H

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
    SKIN_BERRY,
    SKIN_ZERA,
    SKIN_PIXIE,
    SKIN_JAYO,
    SKIN_PERIDOT,
    SKIN_TRANS,
    SKIN_GOTH,
    SKIN_DITTO,
    SKIN_EMBER, 
    SKIN_CYNDER,
    SKIN_CYNDER_CREST
}SpyroColor;
typedef enum FlameColor
{
    FLAME_SKIN_DEFAULT,
    FLAME_SKIN_ICE,
    FLAME_SKIN_BLOOD,
    FLAME_SKIN_GHOST
}FlameColor;




#endif /* COSMETIC_H */
