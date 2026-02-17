#include <custom_types.h>
#include <stdint.h>
#include <stddef.h> // For size_t

// Assuming your address space is 32-bit pointers.
// You might need to adjust these types depending on your PS2 dev environment.


u16 deckard_read_u16(u32* address) {
    // Determine the offset within the 32-bit word
    // This is based on the assumption that a read from 0x...00 gives you the value
    // that was stored at 0x...02 after the endian swap.
    u32 offset = ((int)address) & 0x2;

    // Read the full 32-bit word containing the target address.
    // The pointer is cast to u32* to ensure a 32-bit read.
    volatile u32* word_ptr = (volatile u32*)((int)address & ~0x3);
    u32 temp_word = *word_ptr;

    // Based on the observed behavior, a 16-bit access from the emulated MIPS
    // environment reads the "wrong" half-word due to the 32-bit endian swap.
    // We can correct this by shifting the bytes to get the expected value.
    if (offset == 0) {
        // A read from 0x...00 gets the value from the next short over,
        // which means the high and low half-words are swapped.
        // We need to take the second half-word of the original word.
        return (u16)(temp_word & 0xFFFF);
    }
    else {
     // A read from 0x...02 gets the value from the first short,
     // so we need to take the first half-word.
        return (u16)((temp_word >> 16) & 0xFFFF);
    }
}

u8 deckard_read_u8(u8* address) {
    // Determine the offset within the 32-bit word.
    int offset = ((int)address) & 0x3;

    // Read the full 32-bit word containing the target address.
    volatile u32* word_ptr = (volatile u32*)((int)address & ~0x3);
    u32 temp_word = *word_ptr;

    // The logic below extracts the correct byte based on the half-word swap.
    // Let the original 32-bit value be represented by bytes B3, B2, B1, B0.
    // The half-word swap results in the new order: B1, B0, B3, B2.
    // We need to access the correct byte from the newly ordered word.

    switch (offset) {
        case 0: // Original Byte 0
        return (u8)(temp_word & 0xFF);
        case 1: // Original Byte 1
        return (u8)((temp_word >> 8) & 0xFF);
        case 2: // Original Byte 2
        return (u8)((temp_word >> 16) & 0xFF);
        case 3: // Original Byte 3
        return (u8)((temp_word >> 24) & 0xFF);
        default:
        return 0; // Should not be reached
    }
}

/**
 * @brief Writes an 8-bit value to the Deckard memory region, correcting for the hardware quirk.
 *
 * This function performs a read-modify-write operation at the 32-bit word level
 * and places the byte in the correct position based on the little-endian
 * byte ordering quirk.
 *
 * @param address The memory address.
 * @param value The 8-bit value to write.
 */
void deckard_write_u8(u8* address, u8 value) {
    int offset = ((int)address) & 0x3;
    volatile u32* word_ptr = (volatile u32*)((int)address & ~0x3);

    // Step 1: Read the current 32-bit word from memory.
    u32 temp_word = *word_ptr;

    // Step 2: Clear the old byte and set the new byte in the correct position.
    // The bit-shifting logic is based on the little-endian order confirmed by your read function.
    switch (offset) {
        case 0: // Original Byte 0
        temp_word = (temp_word & 0xFFFFFF00) | ((u32)value);
        break;
        case 1: // Original Byte 1
        temp_word = (temp_word & 0xFFFF00FF) | ((u32)value << 8);
        break;
        case 2: // Original Byte 2
        temp_word = (temp_word & 0xFF00FFFF) | ((u32)value << 16);
        break;
        case 3: // Original Byte 3
        temp_word = (temp_word & 0x00FFFFFF) | ((u32)value << 24);
        break;
    }

    // Step 3: Write the modified 32-bit word back to memory.
    *word_ptr = temp_word;
}


/**
 * @brief Writes a 16-bit value to the Deckard memory region, correcting for the hardware quirk.
 *
 * This function performs a read-modify-write operation at the 32-bit word level
 * and places the half-word in the correct position.
 *
 * @param address The memory address (must be 16-bit aligned).
 * @param value The 16-bit value to write.
 */
void deckard_write_u16(u16* address, u16 value) {
    int offset = ((int)address) & 0x2;
    volatile u32* word_ptr = (volatile u32*)((int)address & ~0x3);

    // Step 1: Read the current 32-bit word from memory.
    u32 temp_word = *word_ptr;

    // Step 2: Clear the old half-word and set the new half-word.
    // This is also based on the little-endian ordering.
    if (offset == 0) {
        // Original half-word 0
        temp_word = (temp_word & 0xFFFF0000) | (u32)value;
    }
    else {
     // Original half-word 1
        temp_word = (temp_word & 0x0000FFFF) | ((u32)value << 16);
    }

    // Step 3: Write the modified 32-bit word back to memory.
    *word_ptr = temp_word;
}


/**
 * @brief Custom memcpy for the Deckard memory region that accounts for half-word swapping.
 *
 * This function ensures that both aligned and unaligned memory transfers
 * are handled correctly by using 32-bit word copies for the bulk of the data
 * and byte-by-byte copies for unaligned portions.
 *
 * @param dest The destination memory address.
 * @param src The source memory address.
 * @param n The number of bytes to copy.
 * @return The destination memory address.
 */
void* deckard_memcpy(void* dest, const void* src, size_t n) {
    u8* dest_byte = (u8*)dest;
    const u8* src_byte = (const u8*)src;


    while (n > 0 && ((int)dest_byte & 0x3) != 0) {
        deckard_write_u8(dest_byte, deckard_read_u8(src_byte));
        dest_byte++;
        src_byte++;
        n--;
    }

    // Handle the main bulk of aligned 32-bit words
    // We can use a direct word copy here as 32-bit transfers work correctly
    u32* dest_word = (u32*)dest_byte;
    const u32* src_word = (const u32*)src_byte;

    while (n >= 4) {
        *dest_word = *src_word;
        dest_word++;
        src_word++;
        n -= 4;
    }

    // Handle trailing unaligned bytes
    // These must also be copied byte-by-byte
    dest_byte = (u8*)dest_word;
    src_byte = (const u8*)src_word;
    while (n > 0) {
        deckard_write_u8(dest_byte, deckard_read_u8(src_byte));
        dest_byte++;
        src_byte++;
        n--;
    }

    return dest;
}