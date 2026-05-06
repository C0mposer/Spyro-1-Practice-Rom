#ifndef SPIN_WHEEL_H
#define SPIN_WHEEL_H

#include <common.h>

typedef void (*SpinWheelCallback)(void);

bool SpinWheelAddOption(const char* label, SpinWheelCallback callback);
void SpinWheelClearOptions(void);
void SpinWheelRegisterTestOptions(void);
void SpinWheelUpdate(void);

#endif /* SPIN_WHEEL_H */
