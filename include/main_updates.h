#ifndef MAIN_UPDATES_H
#define MAIN_UPDATES_H

void MainUpdate(void);

void MoonjumpChecks();

void SaveStateUpdate();

void ChangeInventoryMenu(SwitchButton state);

int AttackFrogThreeHitGateHook(void* frog_ptr);

#endif /* MAIN_UPDATES_H */
