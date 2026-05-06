#ifndef BALLOON_DATA_H
#define BALLOON_DATA_H

#include "vector.h"

Vec3 _balloon_position;
int _balloon_rotation;
int _balloon_timer;
int _balloon_drop_off_location;
int* _balloon_ptr;

int _balloon_unk1;

int _balloon_unk2;
int _balloon_unk3;
int _balloon_unk4;
int _balloon_unk5;
int _balloon_unk6;


struct BalloonLevelSelectData
{
    int balloonRotation;
    Vec3 balloonPos;
    Vec3 cameraPos;
    Vec3 spyroPos;
    int balloonTimer;
    int dropOffLocation;

    int unk;

    int unk2;
    int unk3;
    int unk4;
    int unk5;
    int unk6;
};
typedef struct BalloonLevelSelectData BalloonLevelSelectData;

#endif /* BALLOON_DATA_H */
