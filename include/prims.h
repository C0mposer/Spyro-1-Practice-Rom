#ifndef PRIMS_H
#define PRIMS_H

typedef struct Poly4F
{

    int tag;

    u8RGB color;
    char code;

    u16Vec2 point1Pos;
    u16Vec2 point2Pos;
    u16Vec2 point3Pos;
    u16Vec2 point4Pos;

} Poly4F;

typedef struct ShadedLine
{
    int tag;

    u8RGB colorP1;                  //? 0x4 - 0x6
    char code;                      //? 0x7

    u16Vec2 point1Pos;              //? 0x8-0xB

    u8RGB colorP2;                  //? 0xC-0xE
    char PADDING5;                  //? 0xF

    u16Vec2 point2Pos;              //? 0x10-0x13 
} ShadedLine;

typedef enum PrimitiveCode
{
    POLY3F_OPAQUE = 0x20,
    POLY3F_TRANSPARENT = 0x22,
    POLY3F_OPAQUE_SHADED = 0x30,
    POLY3F_TRANSPARENT_SHADED = 0x32,
    POLY3F_OPAQUE_TEXURED_RAW = 0x25,
    POLY3F_TRANSPARENT_TEXTURED_RAW = 0x27,
    POLY3F_OPAQUE_SHADEDANDTEXTURED_RAW = 0x34,
    POLY3F_TRANSPARENT_SHADEDANDTEXTURED_RAW = 0x36,

    POLY4F_OPAQUE = 0x28,
    POLY4F_TRANSPARENT = 0x2A,
    POLY4F_OPAQUE_SHADED = 0x38,
    POLY4F_TRANSPARENT_SHADED = 0x3A,
    POLY4F_OPAQUE_TEXTURED_RAW = 0x2D,
    POLY4F_TRANSPARENT_TEXTURED_RAW = 0x2F,
    POLY4F_OPAQUE_SHADEDANDTEXTURED_RAW = 0x3C,
    POLY4F_TRANSPARENT_SHADEDANDTEXTURED_RAW = 0x3E,


    LINE_OPAQUE = 0x40,
    LINE_TRANSPARENT = 0x42,
    LINE_OPAQUE_SHADED = 0x50,
    LINE_TRANSPARENT_SHADED = 0x52
}PrimitiveCode;

#endif /* PRIMS_H */
