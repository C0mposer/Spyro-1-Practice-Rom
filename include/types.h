// types.h - Auto-generated

#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
// Only include bool macro if below C23, where its a keyword
#if __STDC_VERSION__ < 202311L
#include <stdbool.h>
#endif

// ==================== Type Definitions ====================

typedef unsigned char           u8, uint8_t, byte, uchar, undefined1;
typedef signed char             s8, int8_t;
typedef unsigned short int      u16, uint16_t, ushort, undefined2;
typedef signed short int        s16, int16_t;
typedef unsigned int            u32, uint32_t, uint, undefined4;
typedef signed int              s32, int32_t;


// ==================== Helper Macros ====================

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define ALIGNED(x) __attribute__((aligned(x)))
#define PACKED __attribute__((packed))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define BIT(n) (1U << (n))

// ==================== Modding Macros ====================

// Run code once per function call
#define ONCE     static bool _has_run = false;     if (!_has_run && (_has_run = true))

// Run code every X frames (function calls)
#define COOLDOWN(x)     static int _cooldown = 0;     if (_cooldown++ % x == 0)

// Easier pointer arithmetic
#define OFFSET_VAL(type, ptr, offset) (*(type*)((u8*)(ptr) + (offset))) // Get a value from a pointer at an offset
#define OFFSET_PTR(type, ptr, offset) ((type*)((u8*)(ptr) + (offset)))  // Get an address of a pointer from an offset

// Call a function without making a symbol. Good for quick tests
#define CALL_FUNC(ret, addr, ...) ((ret(*)())addr)(__VA_ARGS__)


/*
 * MOD_EXPORT exposes a global or static variable to Tools -> Exports after a build.
 * The macro compiles to nothing; the modding tool scans it, resolves the next
 * variable's symbol address from the linker map, and lets you live-edit it in
 * emulator RAM.
 *
 * Usage:
 *   MOD_EXPORT("HUD/X")
 *   int hud_x = 24;
 *
 * Optional mode/options:
 *   MOD_EXPORT("HUD/X", "slider", "min=0,max=320,step=1")
 *   int hud_x = 24;
 *
 * If only a label is provided, Tools -> Exports defaults to a slider. You can
 * change the mode, type, min/max range, color format, and enum values there
 * without touching the source again.
 *
 * Function-local static variables are supported when the linker map contains
 * a unique generated symbol for them.
 *
 * Supported modes:
 *   number, slider, checkbox, enum, color_rgb, vec2, hex
 *
 * Common options:
 *   type=u8/s8/u16/s16/u32/s32/float
 *   min=0,max=100,step=1
 *   format=rgb or format=bgr for color_rgb
 *   Normal=0,Practice=1 for enum values
 */
#define MOD_EXPORT(label, ...)

// ==================== Hook Macros ====================

#define J_HOOK(address, ...)

#define JAL_HOOK(address, ...)

#define B_HOOK(address, ...)

#define BL_HOOK(address, ...)

#endif /* TYPES_H */

