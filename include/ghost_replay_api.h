#ifndef GHOST_REPLAY_API_H
#define GHOST_REPLAY_API_H

#include "vector.h"

typedef struct GhostBufferState
{
    bool isActive;
    bool isRecording;
    int currentFrame;
    Vec3 currentPosition;
    Vec3 previousPosition;
} GhostBufferState;

extern GhostBufferState g_ghostA;
extern GhostBufferState g_ghostB;
extern bool g_ghost_new_best;
extern int g_dbg_ghost_update_calls;
extern int g_dbg_ghost_update_dragon_skips;
extern int g_dbg_ghost_record_calls;
extern int g_dbg_ghost_start_calls;
extern int g_dbg_ghost_start_insta_calls;
extern int g_dbg_ghost_start_reg_calls;

// Called once at startup: if Ghost A already has a completed run (loaded
// externally via ghost-loader), auto-enable ghosts and IL mode so playback
// starts on the first fly-in without user having to navigate menus.
void GhostAutoEnableIfLoaded(void);
void GhostButtonCheck(void);
void GhostFrameTick(void);

#endif
