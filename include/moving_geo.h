#ifndef MOVING_GEO_H
#define MOVING_GEO_H

// In game enum reversing
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


// Custom enum for 1 frame reset
enum GeoResetState
{
    OFF,
    SHOULD_MOVE,
    SHOULD_LOAD
};

void SaveGeoData();
void LoadGeoData();
void GeoDataUpdate();

#endif /* MOVING_GEO_H */
