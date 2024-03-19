#ifndef MOBY_H
#define MOBY_H

typedef struct Vec3_u8
{
    unsigned char x;
    unsigned char y;
    unsigned char z;
}Vec3_u8;

typedef struct BGR_u8
{
    unsigned char B;
    unsigned char G;
    unsigned char R;
}BGR_u8;

typedef struct Moby
{
    void* ptr_mobyData;

    int empty1;
    int* ptr_colision;

    struct Vec3 position;

    short UNK21;                              
    
    short interactState;                         //0 for alive, 1 for flamed, 2 for charged. 

    int UNK2;
    int UNK3;
    int UNK4;
    int UNK5;
    int UNK6;
    int UNK7;

    short subType;                                  // Research this more
    short type;                                     // Determines what type of moby it is

    short UNK10;
    char canDropObject;                             //7D and lower yes, 80 and higher no
    char UNK221;


    char currentAnimation;                          //Should use next animation for setting animations.
    char nextAnimation;                             
    char currentKeyFrame;
    char nextKeyFrame;

    char speedRelated;
    char animationSpeed;


    char UNK13;
    char active;                                    //FF for yes, 0 for no

    struct Vec3_u8 rotation;

    char requiredHUD1;

    unsigned char state;                            // 00 still active, FD picked up. Had to make unsinged because, apparently its negative, so I have to force it to be positive.

    union
    {
        char howPickedUp;                           // 01 picked up by spyro, 03 picked up by sparx
        char subState;                             
    };

    char visable;                                   // FF visable, any other value not

    char UNK_shadowRelated;                         // 1-10 only shadow appears?

    struct BGR_u8 RGB;

    char color;                                      // Determines the color of the moby. (01 red, 02 green, etc.) This can be poked outside of its normal range to get some awesome colors.

    short requiredHUD2;

    char keyPickedUp; //0xFF for picked up

    char heldValue;

    short UNK20;

    short scale;                                    // Determines the scale of the gem. Range from 0x0000 - 0xFFFF. (0 normal sized, 0xFFFF smallest size)

}Moby;

#endif /* MOBY_H */
