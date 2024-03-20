#ifndef SPYRO_H
#define SPYRO_H

#include "common.h"
#include "vector.h"

enum SpyroStates
{
    IDLE_STANDING = 0x0,
    BEGIN_WALK = 0x1,
    WALKING = 0x2,
    END_WALK = 0x3,
    PIVOT = 0x4,
    JUMP = 0x5,
    FLOP = 0x6,
    HURT_ZAPPED = 0x7,
    ROLL_LEFT = 0x9,
    ROLL_RIGHT = 0xA,
    CHARGE = 0xB,
    BONK = 0xC,
    LEDGE_PEER = 0xD,
    HURT_PITCH_HIGH = 0xE,
    GLIDE = 0xF,
    TRIANGLE_DROP = 0x10,
    WHIRLWIND = 0x11,
    IDLE_LOOK_AROUND = 0x12,
    ROLL_DOWN_SLOPE = 0x13,
    CHARGING_AIR = 0x14,
    UPHILL_WALK = 0x15,
    HURT_ROLL = 0x16,
    PULL_BACK_GLIDE = 0x17,
    CHARGING_AIR_SC_JUMP = 0x18,
    HURT_SQUISHED = 0x19,
    CANNON_ROTATE = 0x1A,
    HURT_YAW = 0x1B,
    HURT_PITCH_LOW = 0x1C,
    DROWNING = 0x1D,
    DEATH_SPIN = 0x1E,
    DEATH_SQUISHED = 0x1F,
    SUPERFLY = 0x20,
    SUPERFLY_ASCENDING = 0x21,
    SUPERFLY_DESCENDING = 0x22,
    IDLE_SIT = 0x27,
    SUPERCHARGE = 0x2C
};

typedef struct Spyro
{

struct Vec3 position;                               //? Spyro's position vector.

struct u8Angle RelativeSparxAngle;                  //? Something to do with sparx's rotation based on spyros angle I think.

char pad1;
int unkown1;
int unkown2;

char currentAnim;                                   //? Spyros current animation.
char nextAnim;                                      //? Spyros next animation.

char maybe_currentAnimDupe;
char maybe_nextAnimDupe;
char maybe_currentAnimDupe2;
char maybe_nextAnimDupe2;

char currentKeyfame;                                //? The current animation keyframe.
char nextKeyframe;                                  //? The next animation keyframe.

char maybe_keyframeDupe1;
char maybe_keyframeDupe2;
char maybe_keyframeDupe3;
char maybe_keyframeDupe4;
char maybe_AnimSpeedRelated;
char maybe_animSpeedRelatedDupe1;
char maybe_animSpeedRelatedDupe2;
char unkown3;


u8RGBA colorFilter;                       //? The color filter to apply to spyro. Used during fairy kiss, electric shock, etc.

short maybe_weirdDamageAnimRelated;
short unkown4;
int pad2;
int unkown5;
int unkown6;
int unkown7;
int unkown8;
int unkown9;
int pad3;
int pad4;
int pad5;
int maybe_animUpdateRelated;

char animSpeed;                                     //? The speed at which to play the animations.

char unkown10;
char unkown11;
char unkown12;
int maybe_animStateRelated;
int maybe_headAnimRelated;
int unkown_anim1;
int maybe_headAnimRelated2;
int maybe_tailAnimRelated;
int unkown13;
int maybe_tailAnimRelated2;

int state;                                          //? Spyro's current state.

int subState;                                       //? Spyro's current substate.

int framesInCurrentState;	                        //? The amount of frames spent in the current state. Used for calculations for jumping, boking, damage, time sensitive states, etc

int unkown14;
int unkown15;

struct Vec3 maybe_positionRelative;

int timer_framesIdle;                               //? The amount of frames you have spent idleing.

int timer_framesInAir;                              //? The amount of frames spyro has spent in the air.


int unkown_heightRelated;
int unkown_angleRelated;
int unkown_angleRelated2;
int unkown_angleRelated3;
int unkown_angleRelated4;
int unkown_angleRelated5;
int unkown_portalRelated;
int unkown_portalRelated2;
int unkown_portalRelated3;
int unkown_landingMomentumRelated;
int angle_superChargeHorMomentum;
int angle_superChargeVerMomentum;



struct Vec3 groundedChargeMomentum;                 //? Spyro's grounded charge momentum vector.

struct Vec3 walkingMomentum;                        //? Spyro's walking momentum vector.

struct Vec3 groundedChargeMomentum2;                //? Spyro's groundedChargeMomentum vector again? Unsure.

struct Vec3 airMomentum;                            //? Spyro's air momentum vector.

int unkown_angleRelated8;
int unkown16;
int unkown17;
int unkown18;
int unkown19;
int maybe_glideAndWalkOffLedgeZMomentum;
int maybe_momentumRelated;

struct Angle angle;                                 //? Spyro's angle vector.

int maybe_speedRelated;
int unkown21;                               
int unkown22;
int unkown23;
int unkown24;
int unkown25;
int unkown26;

int walkingAcceleration;                            //? Spyro's walking acceleration.

int maybe_walkingTurnSpeed;
int maybe_gravity;

int glideUsed;                                      //? intean for if Spyro has used glide or not yet, since can only use once in this game.     

int isGliding;                                      //? intean for if Spyro is currently gliding.

int neutralJumpZPos;

int isGrounded;                                     //? intean for if Spyro is currently grounded.

int iFramesTimer;                                   //? Timer for the amount of invincibility frames Spyro should have. Counts down to 0 every frame unless set every frame to a value.

int health;                                         //? Spyro's health value. Sparx's color get's set based on this value.

int drownTimer;                                     //? Timer for the amount of time left to drown I think, I didn't double check lol.

int maybe_superchargeRelated;

int isOverGoop;                                     //? intean for if Spyro is currently over goop.

int maybe_drowningRelated;

int unkown32;

struct Vec3 lastGroundedPosition;                   //? Spyro's last grounded position vector.

int unkown33;
int unkown34;
int unkown35;

int autoFlameRelated;

int maybe_isFlaming;
int unkown36;
int maybe_flameChargeTimerRelated;

SpyroHeadAngle TargetHeadAngle;                     //? Spyro's target head angle vector.

SpyroHeadAngle headAngle;                           //? Spyro's head angle vector.

int maybe_headRollTransitionSpeed;
int maybe_headPitchTransitionSpeed;
int maybe_headYawTransitionSpeed;

int maybe_smokeFlameStuff1;
int maybe_smokeFlameStuff2;
int maybe_smokeFlameStuff3;
int maybe_smokeFlameStuff4;
int maybe_smokeFlameStuff5;

int unkown37;
int unkown38;
int unkown39;

int maybe_tailRelated;
int maybe_chargeTimerRelated;

int isMovementLocked;                               //? Value for if spyro can input or not. Counts down from any value to 0 every frame if not set every frame to a value manually.

int maybe_weirdResearchThis;

int unkownOops;
int unkown40;
int unkown41;
int unkown42;
int unkown43;
int unkown44;
int unkown45;
int unkown46;
int unkown47;
int unkown48;
int unkown49;
int unkown50;
int unkown51;
int unkown52;
int unkown53;
int unkown54;
int unkown55;
int unkown56;
int unkown57;
int unkown58;
int unkown59;

int canSuperfly;                                           //? intean for if Spyro can superfly or not.
int isOnSuperchargeGround;                                 //? intean for if Spyro is on supercharge ground or not.

int maybe_pitchRelatedFlag;

int rollMomentum;                                           //? Spyro's roll momentum.
int pitchMomentum;                                          //? Spyro's pitch momentum.


}Spyro;


#endif //SPYRO_H
