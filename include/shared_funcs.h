#ifndef SHARED_FUNCS_H
#define SHARED_FUNCS_H

//Used to avoid "implicit declaration of function" warning
void InGameTimerUpdate();
void ResetLevelCollectables();
void SaveSpyroAndCamera(bool flyInFlag);
void ReloadSpyroAndCamera();
void HexEdit(void);

const char* ToggleMenuBool(bool* value, bool is_selected, const char* on_text, const char* off_text);

void PlayMenuSound(void);
void StopCameraFollowSpyro(void);
void RestartCameraFollowSpyro(void);
void StopUpdateSpyro(void);
void RestartUpdateSpyro(void);

/* Patch first two instructions of a MIPS function to jr $ra; nop, or restore from originalOpcodePair[0..1]. */
void FunctionCallToggleApply(void* functionAddress, const int originalOpcodePair[2]);
void FunctionCallToggleRestore(void* functionAddress, const int originalOpcodePair[2]);

#endif /* SHARED_FUNCS_H */
