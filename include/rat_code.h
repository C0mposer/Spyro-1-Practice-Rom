#ifndef RAT_CODE_H
#define RAT_CODE_H
#include <moby.h>

// Start of dorito rat moby struct
extern Moby dorito;
extern char dorito_run_time_left;

typedef enum RatState
{
    RAT_RUNNING,
    RAT_DEATH,
    RAT_IDLE
}RatState;

void RatCodeUpdate(void);

#endif /* RAT_CODE_H */
