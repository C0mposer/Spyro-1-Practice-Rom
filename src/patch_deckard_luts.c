#include <stdint.h>

void load_lut_handler(uint32_t addr, uint32_t size)
{
    void (*load_func)(uint32_t addr, uint32_t size) = 0x0;

    if (addr < 0x1f808600) {
        uint32_t offset = ((addr - 0x1F801000) >> 2) & 0x3FFFFFFC;
        load_func = (void*)(*(uint32_t*)(0xA10000 + offset));
        load_func(addr, size);

    }
    else if (addr >= 0x1f808600 && addr < 0x1f900000) {
        load_func = (void*)(0xa06748);
        load_func(addr, size);

    }
    else if (addr >= 0x1f900000) {
        load_func = (void*)(0xa0a03c);
        load_func(addr, size);
    }

    return;
}

void store_lut_handler(uint32_t addr, uint32_t value, uint32_t size)
{
    void (*store_func)(uint32_t addr, uint32_t value, uint32_t size) = 0x0;

    if (addr < 0x1f808600) {
        uint32_t offset = ((addr - 0x1F801000) >> 2) & 0x3FFFFFFC;
        store_func = (void*)(*(uint32_t*)(0xA12000 + offset));
        store_func(addr, value, size);

    }
    else if (addr >= 0x1f808600 && addr < 0x1f900000) {
        store_func = (void*)(0xa067d0);
        store_func(addr, value, size);

    }
    else if (addr >= 0x1f900000) {
        store_func = (void*)(0xa0a05c);
        store_func(addr, value, size);
    }

    return;
}

void patch_lut()
{
    // relocate and truncate load LUT
    uint32_t src = 0xA80000;
    uint32_t dst = 0xA10000;

    for (int i = 0; i < 0x2000; i += 4) {
        *(uint32_t*)(dst + i) = *(uint32_t*)(src + i);
    }

    // relocate and truncate store LUT
    src = 0xAC0000;
    dst = 0xA12000;

    for (int i = 0; i < 0x2000; i += 4) {
        *(uint32_t*)(dst + i) = *(uint32_t*)(src + i);
    }

    // replace load LUT fetch instructions
    uint32_t bla = 0x48000003 | (uint32_t)&load_lut_handler;

    // lb
    *(uint32_t*)(0xa05fb0) = 0x4081008c;  // ble 0x8C
    *(uint32_t*)(0xa0603c) = 0x38800001;  // li 4, 0x1
    *(uint32_t*)(0xa06040) = bla;

    // lh
    *(uint32_t*)(0xa06084) = 0x40810068;  // ble 0x68
    *(uint32_t*)(0xa060ec) = 0x38800002;  // li 4, 0x2
    *(uint32_t*)(0xa060f0) = bla;

    // lw
    *(uint32_t*)(0xa06210) = 0x40810090;  // ble 0x90
    *(uint32_t*)(0xa062a0) = 0x38800004;  // li 4, 0x4
    *(uint32_t*)(0xa062a4) = bla;

    // replace store LUT fetch instructions
    bla = 0x48000003 | (uint32_t)&store_lut_handler;

    // sb
    *(uint32_t*)(0xa062d4) = 0x408100a4;  // ble 0xA4
    *(uint32_t*)(0xa06378) = 0x38a00001;  // li 5, 0x1
    *(uint32_t*)(0xa0637c) = bla;

    // sh
    *(uint32_t*)(0xa063c4) = 0x40810068;  // ble 0x68
    *(uint32_t*)(0xa0642C) = 0x38a00002;  // li 5, 0x2
    *(uint32_t*)(0xa06430) = bla;

    // sw
    *(uint32_t*)(0xa066cc) = 0x40810070;  // ble 0x70
    *(uint32_t*)(0xa0673c) = 0x38a00004;  // li 5, 0x4
    *(uint32_t*)(0xa06740) = bla;

    // replace common LUT init instr
    *(uint32_t*)(0xa06a68) = 0x38000200;  // li 0, 0x200
    *(uint32_t*)(0xa06a70) = 0x3d2000a1;  // lis 9, 0xA1
    *(uint32_t*)(0xa06a80) = 0x3d2000a1;  // lis 9, 0xA1
    *(uint32_t*)(0xa06a88) = 0x61292000;  // ori 9, 9, 0x2000

    // replace add entry func instrs
    *(uint32_t*)(0xa069e0) = 0x3d2000a1;  // lis 9, 0xA1
    *(uint32_t*)(0xa069e4) = 0x3d6000a1;  // lis 11, 0xA1
    *(uint32_t*)(0xa069ec) = 0x616b2000;  // ori 11, 11, 0x2000

    printf("Patch: Load and Store LUTs resized and relocated\n");
}