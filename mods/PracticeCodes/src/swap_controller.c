#include <common.h>

enum
{
    CONTROLLER_ONE,
    CONTROLLER_TWO
};

extern short* _ptr_controller_one;
extern short* _ptr_controller_two;


int current_controller = CONTROLLER_ONE;
short* current_inactive_controller_ptr = NULL;

int amt_button_repeats = 0;
bool is_unpressed = TRUE;
int switch_controller_timer = 0;


void SwapControllerUpdate(void)
{
    UpdateSwitchControllerTimer();

    if (current_controller == CONTROLLER_ONE)
    {
        current_inactive_controller_ptr = _ptr_controller_two;
    }
    else if (current_controller == CONTROLLER_TWO)
    {
        current_inactive_controller_ptr = _ptr_controller_one;
    }

    if (*(current_inactive_controller_ptr + 1) != 0xFFFFFFFF && is_unpressed)
    {
        //printf("Pressed %d times\n", amt_button_repeats);
        amt_button_repeats++;
        is_unpressed = false;
        switch_controller_timer = 1;
    }
    else if (*(current_inactive_controller_ptr + 1) == 0xFFFFFFFF && !is_unpressed)
    {
        //printf("Unpressed\n");
        is_unpressed = true;
    }

    if (amt_button_repeats == 3)
    {
        //printf("Swapping\n");
        SwapActiveController();

        if (current_controller == CONTROLLER_ONE)
        {
            current_controller = CONTROLLER_TWO;
        }
        else if (current_controller == CONTROLLER_TWO)
        {
            current_controller = CONTROLLER_ONE;
        }
    }
}

void SwapActiveController(void)
{
    int* temp = _ptr_controller_one;
    _ptr_controller_one = _ptr_controller_two;
    _ptr_controller_two = temp;
    amt_button_repeats = 0;
    switch_controller_timer = 0;
}

void UpdateSwitchControllerTimer()
{
    if (switch_controller_timer > 0)
    {
        //printf("Counting Timer\n");
        switch_controller_timer++;
    }
    if (switch_controller_timer > 15)
    {
        //printf("Timer expired\n");
        switch_controller_timer = 0;
        amt_button_repeats = 0;
    }
}