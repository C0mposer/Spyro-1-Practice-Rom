#include <common.h>


bool is_holding_rstick_horizontal = false;
bool is_holding_rstick_vertical = false;

int GetHorizontalRightStickDirection(void)
{
    if (!is_holding_rstick_horizontal)
    {
        if (_rightAnalogStick.x_axis <= 0x25)
        {
            is_holding_rstick_horizontal = true;
            return LEFT;
        }
        else if (_rightAnalogStick.x_axis >= 0xDA)
        {
            is_holding_rstick_horizontal = true;
            return RIGHT;
        }
        else
        {
            return -1;
        }
    }
    else if (is_holding_rstick_horizontal)
    {
        // Reset
        if (_rightAnalogStick.x_axis > 0x25 && _rightAnalogStick.x_axis < 0xDA)
        {
            is_holding_rstick_horizontal = false;
        }
        return -1;
    }
}
int GetVerticalRightStickDirection(void)
{
    if (!is_holding_rstick_vertical)
    {
        if (_rightAnalogStick.y_axis <= 0x25)
        {
            is_holding_rstick_vertical = true;
            return UP;
        }
        else if (_rightAnalogStick.y_axis >= 0xDA)
        {
            is_holding_rstick_vertical = true;
            return DOWN;
        }
        else
        {
            return -1;
        }
    }
    else if (is_holding_rstick_vertical)
    {
        // Reset
        if (_rightAnalogStick.y_axis > 0x25 && _rightAnalogStick.y_axis < 0xDA)
        {
            is_holding_rstick_vertical = false;
        }
        return -1;
    }
}