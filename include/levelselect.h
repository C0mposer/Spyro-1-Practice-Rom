#ifndef LEVELSELECT_H
#define LEVELSELECT_H

void ActivateLevelSelect();

typedef struct
{
    char levelID;
    signed short anim;
} levelSelectData;

extern char levelSelectState;
extern char leavingLevel;

#endif /* LEVELSELECT_H */
