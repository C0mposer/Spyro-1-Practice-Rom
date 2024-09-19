
#include <common.h>
#include <moving_geo.h>


int geo_reset_state = OFF;

// From save_state.c
extern byte* mem_region;

void SaveGeoData(byte* current_mem_region)
{
    int* local_mem_region = current_mem_region + 0x10;

    //printf("geo start %X\n", local_mem_region);

    // The amount is 4 bytes before the pointers begin.
    int _amount_of_moving_collision = _ptr_moving_collision_data[-1];
    int _amount_of_moving_textures = _ptr_moving_texture_data[-1];
    int _amount_of_low_lod_textures = _ptr_low_lod_texture_data[-1];
    int _amount_of_myst_textures = _ptr_myst_texture_data[-1];

    // Loop over the amount of moving textures
    for (int i = 0; i < _amount_of_moving_textures; i++)
    {
        // Copy the specific moving texture data
        memcpy(local_mem_region, _ptr_moving_texture_data[i], sizeof(int));
        local_mem_region += 1;

        short texturesArrayOffset = *(short *)((int *)_ptr_moving_texture_data[i] + 1);
        int *ptr_textureBlock = (int *)_ptr_textures_array[texturesArrayOffset];
        int * ptr_movingTextureUpdateRelated = (ptr_textureBlock + 6);
        memcpy(local_mem_region, ptr_movingTextureUpdateRelated, sizeof(int));
        local_mem_region += 1;

        unsigned char movingTextureOffset = *((char *)ptr_textureBlock + 0x17);
        unsigned char movingTextureSize = *((char *)ptr_textureBlock + 0x14);
        int *ptr_movingTexture = ptr_textureBlock + 0x7 + movingTextureOffset;
        memcpy(local_mem_region, ptr_movingTexture, movingTextureSize * sizeof(int));
        local_mem_region += movingTextureSize;

        //printf("size %d: %X\n", i, movingTextureSize);
    }

    for (int i = 0; i < _amount_of_low_lod_textures; i++)
    {
        // Copy the specific moving texture data
        memcpy(local_mem_region, _ptr_low_lod_texture_data[i], sizeof(int));
        local_mem_region += 1;

        short texturesArrayOffset = *(short *)((int *)_ptr_low_lod_texture_data[i] + 1);
        int *ptr_textureBlock = (int *)_ptr_textures_array[texturesArrayOffset];
        int * ptr_movingTextureUpdateRelated = (ptr_textureBlock + 6);
        memcpy(local_mem_region, ptr_movingTextureUpdateRelated, sizeof(int));
        local_mem_region += 1;

        unsigned char movingTextureOffset = *((char *)ptr_textureBlock + 0x17);
        unsigned char movingTextureSize = *((char *)ptr_textureBlock + 0x14);
        int *ptr_movingTexture = ptr_textureBlock + 0x7 + movingTextureOffset;
        memcpy(local_mem_region, ptr_movingTexture, movingTextureSize * sizeof(int));
        local_mem_region += movingTextureSize;

        //printf("lod size %d: %X\n", i, movingTextureSize);
    }

    // Loop over the amount of moving collision 
    for (int i = 0; i < _amount_of_moving_collision; i++)
    {
        memcpy(local_mem_region, _ptr_moving_collision_data[i], sizeof(int));
        local_mem_region += 1;    
    }

    // All moving collision in 1 area, no need to loop
    if(*(&_ptr_moving_collision_data - 1)){ //checks to make sure that there is moving collision to save
        int movingCollisionSize = (*((short *)_ptr_moving_collision_data[0] + 2) + *((short *)_ptr_moving_collision_data[0] + 3)) * 3;
        int* ptr_movingCollision = ((int*)*(_ptr_ptr_moving_collision + 4));

        //printf("%X\n", *(&_ptr_moving_collision_data - 1));
        // Copy the actual collision data
        memcpy(local_mem_region, ptr_movingCollision, movingCollisionSize * sizeof(int));
        local_mem_region += movingCollisionSize;
    }
}

void LoadGeoData(byte* current_mem_region)
{
    int* local_mem_region = current_mem_region + 0x10;

    // The amount is 4 bytes before the pointers begin.
    int _amount_of_moving_textures = _ptr_moving_texture_data[-1];   
    int _amount_of_moving_collision = _ptr_moving_collision_data[-1];
    int _amount_of_low_lod_textures = _ptr_low_lod_texture_data[-1];
    int _amount_of_myst_textures = _ptr_myst_texture_data[-1];

        // Loop over the amount of moving textures
    for (int i = 0; i < _amount_of_moving_textures; i++)
    {
        memcpy(_ptr_moving_texture_data[i], local_mem_region, sizeof(int)); // Hard coding 0x10700 incase I add stuff tosave_state.c
        local_mem_region += 1;

        short texturesArrayOffset = *(short *)((int *)_ptr_moving_texture_data[i] + 1);
        int *ptr_textureBlock = (int *)_ptr_textures_array[texturesArrayOffset];
        int * ptr_movingTextureUpdateRelated = (ptr_textureBlock + 6);
        memcpy(ptr_movingTextureUpdateRelated, local_mem_region, sizeof(int));
        local_mem_region += 1;

        unsigned char movingTextureOffset = *((char *)ptr_textureBlock + 0x17);
        unsigned char movingTextureSize = *((char *)ptr_textureBlock + 0x14);
        int *ptr_movingTexture = ptr_textureBlock + 0x7 + movingTextureOffset;
        memcpy(ptr_movingTexture, local_mem_region, movingTextureSize * sizeof(int));
        local_mem_region += movingTextureSize;
    }

    for (int i = 0; i < _amount_of_low_lod_textures; i++)
    {
        // Copy the specific moving texture data
        memcpy(_ptr_low_lod_texture_data[i], local_mem_region, sizeof(int));
        local_mem_region += 1;

        short texturesArrayOffset = *(short *)((int *)_ptr_low_lod_texture_data[i] + 1);
        int *ptr_textureBlock = (int *)_ptr_textures_array[texturesArrayOffset];
        int * ptr_movingTextureUpdateRelated = (ptr_textureBlock + 6);
        memcpy(ptr_movingTextureUpdateRelated, local_mem_region, sizeof(int));
        local_mem_region += 1;

        unsigned char movingTextureOffset = *((char *)ptr_textureBlock + 0x17);
        unsigned char movingTextureSize = *((char *)ptr_textureBlock + 0x14);
        int *ptr_movingTexture = ptr_textureBlock + 0x7 + movingTextureOffset;
        memcpy(ptr_movingTexture, local_mem_region, movingTextureSize * sizeof(int));
        local_mem_region += movingTextureSize;
    }

        // Loop over the amount of moving collision 
    for (int i = 0; i < _amount_of_moving_collision; i++)
    {
        memcpy(_ptr_moving_collision_data[i], local_mem_region, sizeof(int));
        local_mem_region += 1;

        // Force textures that weren't moving in either locked state, to update
    }

    //All moving collision in 1 area, no need to loop
    if(*(&_ptr_moving_collision_data - 1)) //checks to make sure that there is moving collision to save
    {
    int movingCollisionSize = (*((short *)_ptr_moving_collision_data[0] + 2) + *((short *)_ptr_moving_collision_data[0] + 3)) * 3;
    int* ptr_movingCollision = ((int*)*(_ptr_ptr_moving_collision + 4));
    // Copy the actual collision data

    //printf("T%X\n", local_mem_region);
        memcpy(ptr_movingCollision, local_mem_region, movingCollisionSize * sizeof(int));
        local_mem_region += movingCollisionSize;
    }
        printf("End of local savestate region: %X\n", local_mem_region);

    // Set our geo state to SHOULD_MOVE, so that GeoDataUpdate() can begin
    //geo_reset_state = SHOULD_MOVE;
}

//HACK
// // Runs every frame
// void GeoDataUpdate(void)
// {

//     // Set the geo to move for 1 frame, since it will not update if it is already stopped
//     if (geo_reset_state == SHOULD_MOVE)
//     {
//         int _amount_of_moving_textures = _ptr_moving_texture_data[-1];   
//         int _amount_of_moving_collision = _ptr_moving_collision_data[-1];
        
//         // Loop over the moving texture pointers, and set its movement state to moving
//         for (int i = 0; i < _amount_of_moving_textures; i++)
//         {
//             MovingGeo* geo_data = _ptr_moving_texture_data[i];

//             geo_data->movingState = GEO_MOVING;
//         }

//         // Loop over the moving collision pointers, and set its movement state to moving
//         for (int i = 0; i < _amount_of_moving_collision; i++)
//         {
//             MovingGeo* geo_data = _ptr_moving_collision_data[i];
            
//             geo_data->movingState = GEO_MOVING;

//         }

//         geo_reset_state = SHOULD_LOAD;
//     }

//     // Reload the moving geo once more after it has updated for 1 frame
//     else if (geo_reset_state == SHOULD_LOAD)
//     {
//         // The amount is 4 bytes before the pointers begin.
//         int _amount_of_moving_textures = _ptr_moving_texture_data[-1];   
//         int _amount_of_moving_collision = _ptr_moving_collision_data[-1];
        
//         // Loop over the moving texture pointers and load the MovingGeo data
//         for (int i = 0; i < _amount_of_moving_textures; i++)
//         {
//             memcpy(_ptr_moving_texture_data[i], mem_region + 0x10700 + (i*4), sizeof(int));
//         }

//         // Loop over the moving collision pointers and load the MovingGeo data
//         for (int i = 0; i < _amount_of_moving_collision; i++)
//         {
//             memcpy(_ptr_moving_collision_data[i], mem_region + 0x10A00 + (i*4), sizeof(int));
//         }

//         geo_reset_state = OFF;
//     }

// }