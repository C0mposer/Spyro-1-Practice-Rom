#ifndef HSL_H
#define HSL_H
#include <stdint.h>


typedef struct {
    int h; // Hue: 0-360
    int s; // Saturation: 0-255
    int l; // Lightness: 0-255
} HSL;

typedef struct {
    uint16_t b : 5;
    uint16_t g : 5;
    uint16_t r : 5;
    uint16_t x : 1;
} PS1Color;

#endif /* HSL_H */
