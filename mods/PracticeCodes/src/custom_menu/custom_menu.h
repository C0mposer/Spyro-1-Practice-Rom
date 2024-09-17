#pragma once
void CustomMenuUpdate(void);

void CheckReleasedButtons();

void ToggleMainMenu();

void UpdateMainMenu();

void HandleEnteringSubMenu();

void RenderMainMenu();

void CreateMenuTextInfo(CapitalTextInfo  menu_text_info[5]);

void HandleSwitchingMainMenuSelections();

void ExitMainMenu();

void UpdateILMenu();

void UpdateManualTimerMenu();

void HandleManualTimerResetMenuSelection();

void HandleManualTimerStopButtonMenuSelection();

void HandleManualTimerDisplayMenuSelection();

void HandleManualTimerMenuSelection();

void HandleSwitchingManualTimerMenuSelections();

void UpdateSavestateMenu();

void RenderSavestateMenu();

void HandleSwitchSaveSlotButtonMenuSelection();

void HandleLoadButtonMenuSelection();

void HandleSaveButtonMenuSelection();

void HandleSaveSlotMenuSelection();

void HandleSwitchingSavestateMenuSelections();

void ReturnToCustomMainMenu();

void RenderDragonTouchFrames();
