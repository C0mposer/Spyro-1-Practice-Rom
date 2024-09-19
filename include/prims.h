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

#endif /* PRIMS_H */
