#ifndef DECKARD_H
#define DECKARD_H
#include <stddef.h>

// Assuming your address space is 32-bit pointers.
// You might need to adjust these types depending on your PS2 dev environment.
u16 deckard_read_u16(u32* address);

u8 deckard_read_u8(u8* address);

void deckard_write_u8(u8* address, u8 value);

void deckard_write_u16(u16* address, u16 value);

void* deckard_memcpy(void* dest, const void* src, size_t n);

#endif /* DECKARD_H */
