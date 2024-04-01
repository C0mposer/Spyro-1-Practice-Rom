#include <common.h>


bool is_holding_rstick = false;
int GetHorizontalRightStickDirection(void)
{
    if (!is_holding_rstick)
    {
        if (_rightAnalogStick.x_axis <= 0x25)
        {
            is_holding_rstick = true;
            return LEFT;
        }
        else if (_rightAnalogStick.x_axis >= 0xDA)
        {
            is_holding_rstick = true;
            return RIGHT;
        }
        else
        {
            return -1;
        }
    }
    else if (is_holding_rstick)
    {
        // Reset
        if (_rightAnalogStick.x_axis > 0x25 && _rightAnalogStick.x_axis < 0xDA)
        {
            is_holding_rstick = false;
        }
        return -1;
    }
}