#include <common.h>
#include <moby.h>

#define FRAME_THRESHHOLD 10

// Made the timers and current amount of presses arrays, to keep track of all the different buttons individualls.
int timer[16] = {0};
int current_amount_of_presses[16] = {0};

bool is_holding = false;

int IsPowerOfTwo(unsigned n) 
{ 
    return n && (!(n & (n - 1))); 
} 
  
// Returns position of the only set bit in 'n' 
int GetBitNumber(unsigned n) 
{ 
    if (!IsPowerOfTwo(n)) 
        return -1; 
  
    unsigned i = 1, pos = 1; 
  
    // Iterate through bits of n till we find a set bit 
    // i&n will be non-zero only when 'i' and 'n' have a set bit 
    // at same position 
    while (!(i & n)) { 
        // Unset current bit and set the next bit in 'i' 
        i = i << 1; 
  
        // increment position 
        ++pos; 
    } 
  
    return pos - 1; 
} 

// Check if a button press has happened multiple times in succession
bool CheckButtonMultiTap(unsigned short button, int times_to_press)
{
    
    if ((_currentButtonOneFrame & button))
    {
        int button_index = GetBitNumber(button);
        timer[button_index] = 1;  // 1 is On, and it will start counting from here. Avoids having a seperate bool

        current_amount_of_presses[button_index]++;
        
        if (current_amount_of_presses[button_index] >= times_to_press)
        {
            return true;
        }

        else if (current_amount_of_presses[button_index] < times_to_press)
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

void MultiTapUpdate(void)
{
    // Loop through all the timers
    for (int i = 0; i < 16; i++)
    {
        if (timer[i] > 0)
        {
            timer[i]++;
            if (timer[i] > FRAME_THRESHHOLD)
            {
                timer[i] = 0;
                current_amount_of_presses[i] = 0;
            }
        }
    }
    
}