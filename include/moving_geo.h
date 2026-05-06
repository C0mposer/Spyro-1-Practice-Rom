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


void SaveGeoData();
void LoadGeoData();
void GeoDataUpdate();

#endif /* MOVING_GEO_H */
