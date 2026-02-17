#include <common.h>
#include <spyro.h>
#include <prims.h>
#include <custom_menu.h>
#include <deckard.h>

// The data from spyro to store every frame in a buffer
typedef struct ReplayData
{
    Vec3 position;
    //Angle angle;
    int relativeSparxAngle;

    //todo had to make ints because of deckard being BE? lmao figure this out lil vro
    char currentAnim;
    char nextAnim;
    char currentKeyframe;
    char nextKeyframe;
    char animSpeed;
    char animSpeedRelated;
    char currentHeadAnim;
    char nextHeadAnim;
    char currentHeadKeyframe;
    char nextHeadKeyframe;
    char headAnimSpeed;
    char maybe_HeadAnimSpeedRelated;
}ReplayData;

typedef struct GhostMetaData
{
    int ghost_current_level;
    int amount_of_frames;
    int final_time_in_frames;
}GhostMetaData;


#if BUILD == 0 || BUILD == 5
#define TEMP_SPYRO_REGION 0x80500000
#define GHOST_DATA_REGION 0x80600000
#elif BUILD == 2
#define TEMP_SPYRO_REGION 0x80A28000
#define GHOST_DATA_REGION 0x80A29000 //TODO change to minimum distance from size of spyro struct
#endif

bool should_save_positions = false;
bool should_load_positions = false;
int current_frame = 0;
int amount_of_frames_saved = 0;

extern GhostMenu ghost_menu;

extern bool has_started_insta_fly_in;
extern bool has_started_reg_fly_in;

void GhostButtonCheck()
{
    if (ghost_menu.ghosts_enabled)
    {
        // Make a copy of spyro every frame to the Deckard region
        memcpy(TEMP_SPYRO_REGION, &_spyro, sizeof(_spyro));
        Spyro* temp_spyro = TEMP_SPYRO_REGION;

        // Enable Recording Mode
        if (_currentButtonOneFrame == L3_BUTTON || has_started_insta_fly_in == true)
        {
            current_frame = 0;
            should_save_positions = true;
            should_load_positions = false;
            amount_of_frames_saved = 0;
        }
        // Enable Playback Mode
        else if (_currentButtonOneFrame == R3_BUTTON || has_started_reg_fly_in == true)
        {
            current_frame = 0;
            should_save_positions = false;
            should_load_positions = true;
        }
        if (_currentButtonOneFrame == R1_BUTTON)
        {
            current_frame = 0;
            should_load_positions = false;
            should_save_positions = false;
            amount_of_frames_saved = 0;
        }
    }
}

// Hooked into RenderGameObjects()
void GhostUpdate(void)
{
    if (ghost_menu.ghosts_enabled)
    {
        Spyro* temp_spyro = TEMP_SPYRO_REGION;

        // If on Redux or Duckstation, Extra memory should exist in the 0x80500000 range
        #if BUILD == 0 || BUILD == 5
        //Edit Draw Spyro to read our temp spyro
        * (int*)0x80023afc = 0x3C1F8050; // la $ra, 0x80500000
        *(int*)0x80023b00 = 0x27FF0000; // la $ra, 0x80500000

        // If on Deckard PS2, Extra memory should exist in the 0x80A29000 range
        #elif BUILD == 2
        * (int*)0x80023afc = 0x3C1F80A3; // la $ra, 0x80A29000
        *(int*)0x80023b00 = 0x27FF8000; // la $ra, 0x80A29000
        #endif

       // Disable Updating Flame Rotation Matrix
        * (int*)0x80024110 = NOP;
        *(int*)0x80024114 = NOP;
        *(int*)0x80024118 = NOP;
        *(int*)0x8002411C = NOP;
        *(int*)0x80024120 = NOP;

        // Save ReplayData every frame
        if (should_save_positions == true)
        {
            if (_gameState != GAMESTATE_DRAGON_STATE)
            {
                ReplayData* replay_data = GHOST_DATA_REGION;

                (replay_data[current_frame].position.x) = _spyro.position.x;
                (replay_data[current_frame].position.y) = _spyro.position.y;
                (replay_data[current_frame].position.z) = _spyro.position.z;
                //(replay_data[current_frame].angle.yaw) = _spyro.angle.yaw;
                (replay_data[current_frame].relativeSparxAngle) = _spyro.relativeSparxAngle;
                deckard_write_u8(&(replay_data[current_frame].currentAnim), _spyro.currentAnim);
                deckard_write_u8(&(replay_data[current_frame].nextAnim), _spyro.nextAnim);
                deckard_write_u8(&(replay_data[current_frame].currentKeyframe), _spyro.currentKeyfame);
                deckard_write_u8(&(replay_data[current_frame].nextKeyframe), _spyro.nextKeyframe);
                deckard_write_u8(&(replay_data[current_frame].animSpeed), _spyro.animSpeed);
                deckard_write_u8(&(replay_data[current_frame].animSpeedRelated), _spyro.maybe_AnimSpeedRelated);

                deckard_write_u8(&(replay_data[current_frame].currentHeadAnim), _spyro.currentHeadAnim);
                deckard_write_u8(&(replay_data[current_frame].nextHeadAnim), _spyro.nextHeadAnim);
                deckard_write_u8(&(replay_data[current_frame].currentHeadKeyframe), _spyro.currentHeadKeyframe);
                deckard_write_u8(&(replay_data[current_frame].nextHeadKeyframe), _spyro.nextHeadKeyframe);
                deckard_write_u8(&(replay_data[current_frame].headAnimSpeed), _spyro.headAnimSpeed);
                deckard_write_u8(&(replay_data[current_frame].maybe_HeadAnimSpeedRelated), _spyro.maybe_HeadAnimSpeedRelated);
                                //(replay_data + 10)[current_frame] = _spyro.currentHeadAnim;
                                //(replay_data + 11)[current_frame] = _spyro.nextHeadAnim;
                                //(replay_data + 12)[current_frame] = _spyro.currentTailAnim;
                                //(replay_data + 13)[current_frame] = _spyro.nextTailAnim;
                               // (replay_data + 14)[current_frame] = _spyro.currentHeadKeyframe;
                               // (replay_data + 15)[current_frame] = _spyro.nextHeadKeyframe;
                                //(replay_data + 16)[current_frame] = _spyro.currentTailKeyframe;
                                //(replay_data + 17)[current_frame] = _spyro.nextTailKeyframe;
                                //(replay_data + 18)[current_frame] = _spyro.maybe_HeadAnimSpeedRelated;
                                //(replay_data + 19)[current_frame] = _spyro.maybe_TailAnimSpeedRelated;
                                //(replay_data + 21)[current_frame] = _spyro.unkown1;
                                //(replay_data + 22)[current_frame] = _spyro.unkown2;
                                //(replay_data + 23)[current_frame] = _spyro.headAnimSpeed;

                current_frame += 1;
                amount_of_frames_saved = current_frame;
            }
        }

        // Load ReplayData every frame
        if (should_load_positions == true && should_save_positions == false)
        {
            if (_gameState != GAMESTATE_DRAGON_STATE)
                if (current_frame < amount_of_frames_saved)
                {
                    {
                        ReplayData* replay_data = GHOST_DATA_REGION;

                        temp_spyro->position.x = (replay_data[current_frame].position.x);
                        temp_spyro->position.y = (replay_data[current_frame].position.y);
                        temp_spyro->position.z = (replay_data[current_frame].position.z);
                        //temp_spyro->angle.yaw = (replay_data[current_frame].angle.yaw);
                        temp_spyro->relativeSparxAngle = (replay_data[current_frame].relativeSparxAngle);
                        temp_spyro->currentAnim = deckard_read_u8(&(replay_data[current_frame].currentAnim));
                        temp_spyro->nextAnim = deckard_read_u8((&replay_data[current_frame].nextAnim));
                        temp_spyro->currentKeyfame = deckard_read_u8((&replay_data[current_frame].currentKeyframe));
                        temp_spyro->nextKeyframe = deckard_read_u8((&replay_data[current_frame].nextKeyframe));
                        temp_spyro->animSpeed = deckard_read_u8((&replay_data[current_frame].animSpeed));
                        temp_spyro->maybe_AnimSpeedRelated = deckard_read_u8((&replay_data[current_frame].animSpeedRelated));

                        temp_spyro->currentHeadAnim = deckard_read_u8((&replay_data[current_frame].currentHeadAnim));
                        temp_spyro->nextHeadAnim = deckard_read_u8((&replay_data[current_frame].nextHeadAnim));
                        temp_spyro->currentHeadKeyframe = deckard_read_u8((&replay_data[current_frame].currentHeadKeyframe));
                        temp_spyro->nextHeadKeyframe = deckard_read_u8((&replay_data[current_frame].nextHeadKeyframe));
                        temp_spyro->headAnimSpeed = deckard_read_u8((&replay_data[current_frame].headAnimSpeed));
                        temp_spyro->maybe_HeadAnimSpeedRelated = deckard_read_u8((&replay_data[current_frame].maybe_HeadAnimSpeedRelated));
                        //temp_spyro->currentHeadAnim = (replay_data + 10)[current_frame];
                        //temp_spyro->nextHeadAnim = (replay_data + 11)[current_frame];
                        //temp_spyro->currentTailAnim = (replay_data + 12)[current_frame];
                        //temp_spyro->nextTailAnim = (replay_data + 13)[current_frame];
                       // temp_spyro->currentHeadKeyframe = (replay_data + 14)[current_frame];
                       // temp_spyro->currentHeadKeyframe = (replay_data + 15)[current_frame];
                        //temp_spyro->currentTailKeyframe = (replay_data + 16)[current_frame];
                        //temp_spyro->nextTailKeyframe = (replay_data + 17)[current_frame];
                        //temp_spyro->maybe_HeadAnimSpeedRelated = (replay_data + 18)[current_frame];
                        //temp_spyro->maybe_TailAnimSpeedRelated = (replay_data + 19)[current_frame];
                        //temp_spyro->unkown1 = (replay_data + 21)[current_frame];
                        //temp_spyro->unkown2 = (replay_data + 22)[current_frame];
                        //temp_spyro->headAnimSpeed = (replay_data + 23)[current_frame];

                        current_frame += 1;

                        // Hack to change the ghost spyro's color filter. 
                        // DrawSpyro seemingly doesn't use the RA register with the spyro pointer I've changed to determine 
                        // his color, so I am setting the regular spyro's color filter before the call, 
                        // and then resetting after the call
                        _spyro.colorFilter.B = 0xFF;
                        _spyro.colorFilter.opacity = 0xCC;

                        if (_gameState == GAMESTATE_GAMEPLAY)
                        {
                            if (ghost_menu.ghosts_visual == VISUAL_SPYRO)
                                DrawSpyro();
                            else // else if VISUAL_POLYGON
                                DrawSquareOverTempSpyro(temp_spyro);
                        }
                        _spyro.colorFilter.B = 0x0;
                        _spyro.colorFilter.opacity = 0x0;
                    }
                }
        }

        // Restore Draw Spyro to point to the original Spyro
        *(int*)0x80023afc = 0x3C1F8008; // la $ra, &_spyro
        *(int*)0x80023b00 = 0x27FF8A58;

        // Enable Updating Flame Rotation Matrix
        *(int*)0x80024110 = 0xAC2700B8;
        *(int*)0x80024114 = 0xAC2800BC;
        *(int*)0x80024118 = 0xAC2900C0;
        *(int*)0x8002411C = 0xAC2A00C4;
        *(int*)0x80024120 = 0xAC2B00C8;

    }
}

// Draw A Poly4F Polygon in world space over Spyro. This is for laggy levels where drawing another spyro adds more lag.
void DrawSquareOverTempSpyro(Spyro* temp_spyro)
{
    #define WORLD_SQUARE_SIZE 150
    Vec3 spyroCenter = temp_spyro->position;
    Vec3 worldPoint1, worldPoint2, worldPoint3, worldPoint4;
    Vec3 screenPoint1, screenPoint2, screenPoint3, screenPoint4;

    // Calculate the corners of the square around Spyro
    worldPoint1.x = spyroCenter.x - WORLD_SQUARE_SIZE;
    worldPoint1.y = spyroCenter.y + WORLD_SQUARE_SIZE;
    worldPoint1.z = spyroCenter.z;

    worldPoint2.x = spyroCenter.x - WORLD_SQUARE_SIZE;
    worldPoint2.y = spyroCenter.y - WORLD_SQUARE_SIZE;
    worldPoint2.z = spyroCenter.z;

    worldPoint3.x = spyroCenter.x + WORLD_SQUARE_SIZE;
    worldPoint3.y = spyroCenter.y + WORLD_SQUARE_SIZE;
    worldPoint3.z = spyroCenter.z;

    worldPoint4.x = spyroCenter.x + WORLD_SQUARE_SIZE;
    worldPoint4.y = spyroCenter.y - WORLD_SQUARE_SIZE;
    worldPoint4.z = spyroCenter.z;


    Poly4F poly = { .code = POLY4F_TRANSPARENT, .color.B = 0xFF };

    WorldToScreen(&screenPoint1, &worldPoint1);
    WorldToScreen(&screenPoint2, &worldPoint2);
    WorldToScreen(&screenPoint3, &worldPoint3);
    WorldToScreen(&screenPoint4, &worldPoint4);

    poly.point1Pos.x = screenPoint1.x;
    poly.point1Pos.y = screenPoint1.y;
    poly.point2Pos.x = screenPoint2.x;
    poly.point2Pos.y = screenPoint2.y;
    poly.point3Pos.x = screenPoint3.x;
    poly.point3Pos.y = screenPoint3.y;
    poly.point4Pos.x = screenPoint4.x;
    poly.point4Pos.y = screenPoint4.y;

    // Draw the polygon
    CustomDrawPolygon(poly, POLY4F_OPAQUE);
}