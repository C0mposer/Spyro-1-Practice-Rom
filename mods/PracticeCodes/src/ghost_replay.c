#include <common.h>
#include <spyro.h>

// The data from spyro to store every frame in a buffer
typedef struct ReplayData
{
    Vec3 position;
    Angle angle;
    int relativeSparxAngle;
    int currentAnim;
    int nextAnim;   //todo had to make ints because of deckard being BE? lmao figure this out lil vro
    int currentKeyframe;
    int nextKeyframe;
    int animSpeed;
    int animSpeedRelated;
}ReplayData;


#if BUILD == 0
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

void GhostButtonCheck()
{
    memcpy(TEMP_SPYRO_REGION, &_spyro, sizeof(_spyro));
    Spyro* temp_spyro = TEMP_SPYRO_REGION;

    if (_currentButtonOneFrame == L3_BUTTON)
    {
        current_frame = 0;
        should_save_positions = true;
        should_load_positions = false;
        amount_of_frames_saved = 0;
    }
    else if (_currentButtonOneFrame == R3_BUTTON)
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
void GhostTest(void)
{
    Spyro* temp_spyro = TEMP_SPYRO_REGION;

    #if BUILD == 0
    //Edit Draw Spyro to read our temp spyro
    * (int*)0x80023afc = 0x3C1F8050; // la $ra, 0x80500000
    *(int*)0x80023b00 = 0x27FF0000;
    #elif BUILD == 2
    * (int*)0x80023afc = 0x3C1F80A3; // la $ra, 0x80A23000
    *(int*)0x80023b00 = 0x27FF8000;
    #endif

    //Edit Draw Spyro Shadow to read our temp spyro
    //*(int*)0x80059A48 = 0x3C180050;
    //*(int*)0x80059A4C = 0x27180000;

    if (should_save_positions == true)
    {
        if (_gameState != GAMESTATE_DRAGON_STATE)
        {
            ReplayData* replay_data = GHOST_DATA_REGION;

            (replay_data[current_frame].position.x) = _spyro.position.x;
            (replay_data[current_frame].position.y) = _spyro.position.y;
            (replay_data[current_frame].position.z) = _spyro.position.z;
            (replay_data[current_frame].angle.yaw) = _spyro.angle.yaw;
            (replay_data[current_frame].relativeSparxAngle) = _spyro.relativeSparxAngle;
            (replay_data[current_frame].currentAnim) = _spyro.currentAnim;
            (replay_data[current_frame].nextAnim) = _spyro.nextAnim;
            (replay_data[current_frame].currentKeyframe) = _spyro.currentKeyfame;
            (replay_data[current_frame].nextKeyframe) = _spyro.nextKeyframe;
            (replay_data[current_frame].animSpeed) = _spyro.animSpeed;
            (replay_data[current_frame].animSpeedRelated) = _spyro.maybe_AnimSpeedRelated;
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
    if (should_load_positions == true && should_save_positions == false)
    {
        if (_gameState != GAMESTATE_DRAGON_STATE)
            if (current_frame < amount_of_frames_saved)
            {
                {
                    printf("Bruh\n");
                    ReplayData* replay_data = GHOST_DATA_REGION;

                    temp_spyro->position.x = (replay_data[current_frame].position.x);
                    temp_spyro->position.y = (replay_data[current_frame].position.y);
                    temp_spyro->position.z = (replay_data[current_frame].position.z);
                    temp_spyro->angle.yaw = (replay_data[current_frame].angle.yaw);
                    temp_spyro->relativeSparxAngle = (replay_data[current_frame].relativeSparxAngle);
                    temp_spyro->currentAnim = (replay_data[current_frame].currentAnim);
                    temp_spyro->nextAnim = (replay_data[current_frame].nextAnim);
                    temp_spyro->currentKeyfame = (replay_data[current_frame].currentKeyframe);
                    temp_spyro->nextKeyframe = (replay_data[current_frame].nextKeyframe);
                    temp_spyro->animSpeed = (replay_data[current_frame].animSpeed);
                    temp_spyro->maybe_AnimSpeedRelated = (replay_data[current_frame].animSpeedRelated);
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

                    // Hack to change the temp spyro's color filter. DrawSpyro seemingly doesn't use the RA register with the spyro pointer I change to determine his color, so I am setting the regular spyro's color filter before the call, and then resetting after the call
                    _spyro.colorFilter.B = 0xFF;
                    _spyro.colorFilter.opacity = 0xCC;

                    if (_gameState == GAMESTATE_GAMEPLAY)
                        DrawSpyro();
                    //DrawSpyroShadow();
                    _spyro.colorFilter.B = 0x0;
                    _spyro.colorFilter.opacity = 0x0;
                }
            }
    }

        // Restore Draw Spyro
    *(int*)0x80023afc = 0x3C1F8008;
    *(int*)0x80023b00 = 0x27FF8A58;
    // // Restore Draw Spyro Shadow
    // *(int*)0x80059A48 = 0x3C188008;
    // *(int*)0x80059A4C = 0x27188A58;
}