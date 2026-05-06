#include <common.h>
#include <sound.h>

void NumberToAscii(int number, char* result)
{
    // Ensure the input number is within the 0-999 range
    if (number < 0 || number > 999)
    {
        result[0] = '?'; // Use '?' to indicate an out-of-range error
        result[1] = '\0';
        return;
    }

    // Handle numbers based on their range
    if (number < 10)
    {
        // Single-digit number
        result[0] = '0' + number;  // Convert the single digit to ASCII
        result[1] = '\0';          // Null-terminate the string
    }
    else if (number < 100)
    {
        // Two-digit number
        result[0] = '0' + (number / 10); // Tens place
        result[1] = '0' + (number % 10); // Ones place
        result[2] = '\0';                // Null-terminate the string
    }
    else
    {
        // Three-digit number
        result[0] = '0' + (number / 100);        // Hundreds place
        result[1] = '0' + ((number / 10) % 10);  // Tens place
        result[2] = '0' + (number % 10);         // Ones place
        result[3] = '\0';                        // Null-terminate the string
    }
}

const char* ToggleMenuBool(bool* value, bool is_selected, const char* on_text, const char* off_text)
{
    if (is_selected && (_currentButtonOneFrame == RIGHT_BUTTON || _currentButtonOneFrame == LEFT_BUTTON))
    {
        *value = !*value;
    }
    return *value ? on_text : off_text;
}

void PlayMenuSound(void)
{
    PlaySoundEffect(SOUND_EFFECT_SPARX_GRAB_GEM, 0, SOUND_PLAYBACK_MODE_NORMAL, 0);
}

void FunctionCallToggleApply(void* functionAddress, const int originalOpcodePair[2])
{
    int* p = (int*)functionAddress;
    (void)originalOpcodePair;
    p[0] = JR_RA;
    p[1] = NOP;
}

void FunctionCallToggleRestore(void* functionAddress, const int originalOpcodePair[2])
{
    int* p = (int*)functionAddress;
    p[0] = originalOpcodePair[0];
    p[1] = originalOpcodePair[1];
}