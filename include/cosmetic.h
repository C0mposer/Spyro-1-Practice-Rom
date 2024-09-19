#ifndef COSMETIC_H
#define COSMETIC_H

typedef struct RedGreen
{
    byte r;
    byte g;
}RedGreen;

struct FlameTriangleSingleColor
{
    byte r;
    byte g;
    byte b;
    byte shape_code;
};
typedef struct FlameTriangleSingleColor FlameTriangleSingleColor;

struct FlameTriangleColorGradient
{
    FlameTriangleSingleColor tip;
    FlameTriangleSingleColor center;
    FlameTriangleSingleColor right;
    FlameTriangleSingleColor left;
};
typedef struct FlameTriangleColorGradient FlameTriangleColorGradient;
extern FlameTriangleColorGradient _flame_triangle_color_gradient;



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
    SKIN_EMERALD,
    SKIN_CORAL_BLUE,
    SKIN_GOLD,
    SKIN_BERRY,
    SKIN_GREY,
    SKIN_ZERA,
    SKIN_PIXIE,
    SKIN_CANDY,
    SKIN_PERIDOT,
    SKIN_TRANS,
    SKIN_GOTH,
    SKIN_DITTO,
    SKIN_EMBER,
    SKIN_CYNDER,
    SKIN_CUSTOM
}SpyroColor;
typedef enum FlameColor
{
    FLAME_SKIN_DEFAULT,
    FLAME_SKIN_DARK_RED,
    FLAME_SKIN_ICY,
    FLAME_SKIN_JADE,
    FLAME_SKIN_BLUE,
    FLAME_SKIN_GOLD,
    FLAME_SKIN_GREEN,
    FLAME_SKIN_PINK,
    FLAME_SKIN_ACID,
    FLAME_SKIN_PURPLE,
    FLAME_SKIN_LIGHT_PINK,
    FLAME_SKIN_PASTEL,
    FLAME_SKIN_RAINBOW,
    FLAME_SKIN_CYNDER,
    FLAME_SKIN_GHOST
}FlameColor;

typedef enum SparxColor
{
    SPARX_SKIN_DEFAULT,
    SPARX_SKIN_RED,
    SPARX_SKIN_ICY,
    SPARX_SKIN_JADE,
    SPARX_SKIN_DARK_BLUE,
    SPARX_SKIN_PURPLE,
    SPARX_SKIN_ORANGE,
    SPARX_SKIN_PINK,
    SPARX_SKIN_TURQUOISE,
    SPARX_SKIN_SILVER,
    SPARX_SKIN_BLACK,
}SparxColor;




#endif /* COSMETIC_H */
