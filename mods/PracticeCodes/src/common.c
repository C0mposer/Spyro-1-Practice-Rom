#include <common.h>

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