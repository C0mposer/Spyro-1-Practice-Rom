#ifndef VRAM_H
#define VRAM_H

#include <common.h>

void LoadSpyroBMPToMainRam(int sector);
void WriteSpyroBMPToVram();
void LoadFlameBMPToMainRam(int sector);
void WriteFlameBMPToVram();


#endif /* VRAM_H */
