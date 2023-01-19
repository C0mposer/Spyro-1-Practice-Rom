#ifndef VECTOR_H
#define VECTOR_H


//~~~~~~~
//Structs
//~~~~~~~
typedef struct Vec3
{
    int x;
    int y;
    int z;
}Vec3;

typedef struct Vec3Padding
{
    int x;
    int y;
    int z;
    int padding;
}Vec3Padding;

typedef struct u16Vec3
{
    unsigned short x;
    unsigned short y;
    unsigned short z;
}u16Vec3;

typedef struct u8Vec3
{
    unsigned char x;
    unsigned char y;
    unsigned char z;
}u8Vec3;

typedef struct Vec2
{
    int x;
    int y;

}Vec2;

typedef struct u16Vec2
{
    unsigned short x;
    unsigned short y;
}u16Vec2;

typedef struct u8Vec2
{
    unsigned char x;
    unsigned char y;
}u8Vec2;

typedef struct Angle
{
    int yaw;
    int pitch;
    int roll;
}Angle;

typedef struct u16Angle
{
    short yaw;
    short pitch;
    short roll;
}u16Angle;

typedef struct SpyroHeadAngle
{
    short roll;
    short pad1;

    short pitch;
    short pad2;

    short yaw;
    short pad3;
}SpyroHeadAngle;;

typedef struct CameraAngle
{
    short roll;
    short pitch;
    short yaw;
}CameraAngle;

typedef struct u8Angle
{
    char yaw;
    char pitch;
    char roll;
}u8Angle;

#endif /* VECTOR_H */
