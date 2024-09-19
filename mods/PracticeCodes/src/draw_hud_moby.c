#include <moby.h>
#include <common.h>
#include <vector.h>

#define MOBY_SIZE 0x58


void CustomDrawMoby(short type, struct MyHudMoby* info, char color)

{
    _ptr_HudMobyData = _ptr_HudMobyData - MOBY_SIZE / 4;              //set moby ptr to next moby space
    memset(_ptr_HudMobyData, 0, MOBY_SIZE);                         //clear a space of memory for the moby
    struct Moby* ptr_current_moby = (Moby*)_ptr_HudMobyData;


    ptr_current_moby->type = type;                                  //setting the moby type.

    ptr_current_moby->position.x = info->position.x;                //setting the moby x position.
    ptr_current_moby->position.y = info->position.y;                //setting the moby y position.
    ptr_current_moby->position.z = info->position.z;                //setting the moby size 

    ptr_current_moby->color = color;                                //setting the moby's color

    ptr_current_moby->rotation.z = info->angle.yaw;                 //setting the moby's Z Rotation
    ptr_current_moby->rotation.y += info->angle.pitch;              //setting the moby's X Rotation
    ptr_current_moby->rotation.x += info->angle.roll;               //setting the moby's Y Rotation

    ptr_current_moby->requiredHUD1 = 0x7f;                          //UNK, required for moby
    ptr_current_moby->requiredHUD2 = 0xff;                          //UNK, required for moby

}