#include <common.h>
#include <levelselect.h>
#include <shared_funcs.h>
#include <multitap.h>
#include <moving_geo.h>

#define STARTING_MEM 0x80A10000

byte* mem_region = STARTING_MEM;
byte* mem_region2 = STARTING_MEM + 0x10000;

int geo_load_timer = 0;

// from IGT.c
extern int savestate_selection;
// from main_updates.c
extern bool hasSavedSpyro;



void SaveGeoData(void)
{
    int _amount_of_moving_textures = _ptr_moving_texture_data[-1];   
    int _amount_of_moving_collision = _ptr_moving_collision_data[-1];

    for (int i = 0; i < _amount_of_moving_textures; i++)
    {
        memcpy(mem_region2 + 0x720 + (i*4), _ptr_moving_texture_data[i], sizeof(int));
    }
    
    for (int i = 0; i < _amount_of_moving_collision; i++)
    {
        memcpy(mem_region2 + 0x900 + (i*4), _ptr_moving_collision_data[i], sizeof(int));
    }
}

void LoadGeoData(void)
{
    int _amount_of_moving_textures = _ptr_moving_texture_data[-1];   
    int _amount_of_moving_collision = _ptr_moving_collision_data[-1];
    
    for (int i = 0; i < _amount_of_moving_textures; i++)
    {
        memcpy(_ptr_moving_texture_data[i], mem_region2 + 0x720 + (i*4), sizeof(int));
    }

    for (int i = 0; i < _amount_of_moving_collision; i++)
    {
        memcpy(_ptr_moving_collision_data[i], mem_region2 + 0x900 + (i*4), sizeof(int));
    }
    
    geo_load_timer = 1;
}

void GeoDataUpdate(void)
{

    
    if (geo_load_timer == 1)
    {
        int _amount_of_moving_textures = _ptr_moving_texture_data[-1];   
        int _amount_of_moving_collision = _ptr_moving_collision_data[-1];
        
        for (int i = 0; i < _amount_of_moving_textures; i++)
        {
            MovingGeo* geo_data = _ptr_moving_texture_data[i];

            geo_data->movingState = GEO_MOVING;
        }

        for (int i = 0; i < _amount_of_moving_collision; i++)
        {
            MovingGeo* geo_data = _ptr_moving_collision_data[i];
            
            geo_data->movingState = GEO_MOVING;

        }

        geo_load_timer = 2;
    }

    else if (geo_load_timer == 2)
    {
        int _amount_of_moving_textures = _ptr_moving_texture_data[-1];   
        int _amount_of_moving_collision = _ptr_moving_collision_data[-1];
        
        for (int i = 0; i < _amount_of_moving_textures; i++)
        {
            memcpy(_ptr_moving_texture_data[i], mem_region2 + 0x720 + (i*4), sizeof(int));
        }

        for (int i = 0; i < _amount_of_moving_collision; i++)
        {
            memcpy(_ptr_moving_collision_data[i], mem_region2 + 0x900 + (i*4), sizeof(int));
        }

        geo_load_timer = 0;
    }

}

void SetMemoryRegion(void)
{
    switch (savestate_selection)
    {
      case 0:
        mem_region = STARTING_MEM;
        mem_region2 = STARTING_MEM + 0x10000;
        break;
      case 1:
        mem_region = STARTING_MEM + 0x10800;
        mem_region2 = STARTING_MEM + 0x20800;
        break;
      case 2:
        mem_region = STARTING_MEM + 0x21000;
        mem_region2 = STARTING_MEM + 0x31000;
        break;
    
      default:
        mem_region = STARTING_MEM;
        mem_region2 = STARTING_MEM + 0x10000;
    }
}

void SaveStateTest(void)
{

    SetMemoryRegion();
    
    // Copy all level mobys, dynamic mobys, and moby data to the free DECKARD IOP area
    memcpy((int*)mem_region, (byte*)_ptr_levelMobys, 0x10000);

    // Copy other misc data to the DECKARD IOP area
    memcpy((int*)mem_region2,       &_ptr_dynamicLevelMobys,     sizeof(int*));
    memcpy((int*)mem_region2 + 1,   &_ptr_levelMobyData,         sizeof(int*));
    memcpy((int*)mem_region2 + 2,   &_ptr_sparx,                 sizeof(int*));
    memcpy((int*)mem_region2 + 3,   &_ptr_sparx2,                sizeof(int*));
    memcpy((int*)mem_region2 + 4,   &_doesHaveSuperflame,        sizeof(int));
    memcpy((int*)mem_region2 + 5,   &_superflameTimer,           sizeof(int));
    memcpy((int*)mem_region2 + 6,   &_levelGemsCollectedArray,   sizeof(_levelGemsCollectedArray));
    memcpy((int*)mem_region2 + 45,   &_globalEggs,            sizeof(int));
    memcpy((int*)mem_region2 + 46,   &_globalGems,                sizeof(int));
    memcpy((int*)mem_region2 + 47,   &_globalDragons,             sizeof(int));
    memcpy((int*)mem_region2 + 48, &_globalFlightLevelTimer,    sizeof(int));
    memcpy((int*)mem_region2 + 49, &_globalFlightLevelCollectedItemsArray, sizeof(_globalFlightLevelCollectedItemsArray));


        // Copy general hud timer area
    memcpy((int*)mem_region2 + 55,      (void*)0x80077F20, 0x260);

    memcpy((byte*)mem_region2 + 0x350, &_spyro, sizeof(_spyro));
    memcpy((byte*)mem_region2 + 0x600, &_cameraStart, 0xFF);
    memcpy((byte*)mem_region2 + 0x710, &_keyState, 0x4);
    SaveGeoData();

    hasSavedSpyro = true;

}

void LoadStateTest(void)
{
    SetMemoryRegion();

    if (*mem_region2 != NULL)
    {
        // Reload all level mobys, dynamic mobys, and moby data from the free DECKARD IOP area
        memcpy( (byte*)_ptr_levelMobys, mem_region, 0x10000);

        // Reload other misc data from the DECKARD IOP area
        memcpy(&_ptr_dynamicLevelMobys,                 (int*)mem_region2,     sizeof(int*));
        memcpy(&_ptr_levelMobyData,                     (int*)mem_region2 + 1, sizeof(int*));
        memcpy(&_ptr_sparx,                             (int*)mem_region2 + 2, sizeof(int*));
        memcpy(&_ptr_sparx2,                            (int*)mem_region2 + 3, sizeof(int*));
        memcpy(&_doesHaveSuperflame,                    (int*)mem_region2 + 4, sizeof(int*));
        memcpy(&_superflameTimer,                       (int*)mem_region2 + 5, sizeof(int*));
        memcpy(&_levelGemsCollectedArray,               (int*)mem_region2 + 6, sizeof(_levelGemsCollectedArray));
        memcpy(&_globalEggs,                            (int*)mem_region2 + 45, sizeof(int));
        memcpy(&_globalGems,                            (int*)mem_region2 + 46, sizeof(int));
        memcpy(&_globalDragons,                         (int*)mem_region2 + 47, sizeof(int));
        memcpy(&_globalFlightLevelTimer,                (int*)mem_region2 + 48, sizeof(int));
        memcpy(&_globalFlightLevelCollectedItemsArray,  (int*)mem_region2 + 49, sizeof(_globalFlightLevelCollectedItemsArray));


        // Copy general hud timer area
        memcpy((void*)0x80077F20,                       (int*)mem_region2 + 55, 0x260);

        memcpy(&_spyro, (byte*)mem_region2 + 0x350, sizeof(_spyro));
        memcpy(&_cameraStart, (byte*)mem_region2 + 0x600, 0xFF);
        memcpy(&_keyState, (byte *)mem_region2 + 0x710, 0x4);
        LoadGeoData();
    }
}