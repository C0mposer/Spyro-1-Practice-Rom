#ifndef MOVING_PLAT_H
#define MOVING_PLAT_H

enum GeoMovingState
{
    GEO_MOVING,
    GEO_DEFAULT,
    GEO_STOPPED
};

struct MovingGeo
{
    char linkedMobyIndex;
    char movingState;
    short timer;
};
typedef struct MovingGeo MovingGeo;

#endif /* MOVING_PLAT_H */
