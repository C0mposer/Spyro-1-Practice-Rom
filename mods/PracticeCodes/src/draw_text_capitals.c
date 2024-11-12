#include <common.h>
#include <moby.h>
#include <vector.h>

#define MOBY_SIZE 0x58

extern char* _ptr_hudMobys;

/** @ingroup reveresed_functions
 *  @{
 */

/**
 * @brief Draws a text string to the screen (Only capital characters allowed)
 * @details Fills out a moby struct in the hud mobys array, using arguments for Capital ASCII text

 * @param char* text - text to be drawn.
 * @param int* textInfo - pointer to the desired location and size of the text.
 * @param int spacing - how much space between characters.
 * @param char color - color.
 * @note Function: DrawTextCapitals \n
   Original Address: 0x80017fe4 \n
   Hook File: draw_text_capitals_hook.s \n
   Prototype: n/a \n
 * @see DrawTextAll()
*/
char* MyDrawTextCapitals(char* text, int* textInfo, int spacing, char color)
{
    unsigned int currentCharacter;                                                  //? Needs to be unsigned lmao?

    bool isColon = false;

    currentCharacter = *text;                                                       // Puts first character of string in currentcCharacter

    while (currentCharacter != 0) {                                                 // Not a NULL terminator
        if (currentCharacter != 0x20) {                                             // Not a space character
            _ptr_hudMobys -= MOBY_SIZE;                                             // Shifts the moby pointer to a new empty slot
            memset(_ptr_hudMobys, 0, MOBY_SIZE);                                    // Clears the new slot

            if (!isColon)
            {
                Vec3Copy((int*)(_ptr_hudMobys + 0xc), textInfo);                    // Copy text x pos, y pos, and size(z pos) to the new moby
            }

            else
            {
                int textInfoColonDot[3] = { 0 };
                Vec3Copy(textInfoColonDot, textInfo);                               // Copy Text Info
                textInfoColonDot[1] -= 0x9;                                         // Raise 2nd Dot for colon
                Vec3Copy((int*)(_ptr_hudMobys + 0xc), textInfoColonDot);            // Copy text x pos, y pos, and size(z pos) to the new moby
            }

            currentCharacter = *text;                                               // Puts each character of the string in currentCharacter each iteration of the loop

            if (currentCharacter - '0' < 10)
            {                                         // If currentCharacter 0-9
                *(unsigned short*)(_ptr_hudMobys + 0x36) = currentCharacter + 0xd4;
            }
            else if (currentCharacter - 'A' < 26)
            {                                    // If currentCharacter is A-Z
                *(unsigned short*)(_ptr_hudMobys + 0x36) = currentCharacter + 0x169;
            }

            else
            {
                short mobyType;                                                     // Special Characters
                if (currentCharacter == '/') {
                    mobyType = 0x115;
                }
                else if (currentCharacter == '?') {
                    mobyType = 0x116;
                }
                else if (currentCharacter == '%') {
                    mobyType = 0x110;
                }
                else if (currentCharacter == '^') {
                    mobyType = 0x141;
                }
                else if (currentCharacter == '+') {
                    mobyType = 0x13d;
                }
                else if (currentCharacter == ':') {
                    mobyType = 0x147;

                    // Set isColon to true, then false the 2nd time
                    isColon++;
                    isColon %= 2;
                }
                else {                                                              // Default Character (.)
                    mobyType = 0x147;
                }

                *(short*)(_ptr_hudMobys + 0x36) = mobyType;

            }

            _ptr_hudMobys[0x47] = 0x7f;
            _ptr_hudMobys[0x4f] = color;
            _ptr_hudMobys[0x50] = 0xff;

        }
        if (!isColon)                                                               // Only update the text and spacing on the 2nd dot
        {
            text++;                                                                 // Move to next char in String
            textInfo[0] += spacing;                                                 // The x position in the textInfo struct is increased by the spacing amount
            currentCharacter = *text;                                               // currentCharacter is updated to the next character
        }
    }
    //printf("my print: %X\n", _ptr_hudMobys);
    return _ptr_hudMobys;
}