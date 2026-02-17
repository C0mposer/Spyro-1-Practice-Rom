#include <common.h>
#include <stdint.h>
#include <custom_menu.h>
#include <hsl.h>
#include <cosmetic.h>
#include <deckard.h>

extern SkinEditorMenu skin_editor_menu;
extern char* original_spyro_clut;
extern int current_menu;


extern int index_array_sizes[7];

extern int* array_of_section_pixels[7];

extern int spyro_color_index;

PS1Color original_ps1_clut[512] = { 0 };
PS1Color spyro_clut_buffer[512] = { 0 };

// Moved to color_edit_globals.c because the be/le nonsense with deckard
extern HSL hslBodyPartData[7];


// Helper functions (same as before)
int integer_abs(int n) {
    return (n < 0) ? -n : n;
}
int max3(int a, int b, int c) {
    int m = a;
    if (b > m) m = b;
    if (c > m) m = c;
    return m;
}
int min3(int a, int b, int c) {
    int m = a;
    if (b < m) m = b;
    if (c < m) m = c;
    return m;
}

// HSL-specific helper function to get color component from HSL
int HSLToRGB_Channel(int p, int q, int t) {
    if (t < 0) t += 360;
    if (t > 360) t -= 360;
    if (t < 60) return p + (q - p) * t / 60;
    if (t < 180) return q;
    if (t < 240) return p + (q - p) * (240 - t) / 60;
    return p;
}

// Converts a 15-bit PS1 BGR color to an integer-based HSL structure.
// Saturation and Lightness are on a 0-255 scale.
HSL PS1ColorToHSL(PS1Color color) {
    int r = color.r * 8;
    int g = color.g * 8;
    int b = color.b * 8;

    int c_max = max3(r, g, b);
    int c_min = min3(r, g, b);
    int delta = c_max - c_min;

    HSL hsl;
    hsl.l = (c_max + c_min) / 2;

    if (delta == 0) {
        hsl.s = 0;
    }
    else {
        int lightness_double = (c_max + c_min);
        int divisor = (lightness_double <= 255) ? lightness_double : 510 - lightness_double;
        if (divisor == 0) hsl.s = 0;
        else hsl.s = (delta * 255) / divisor;
    }

    if (delta == 0) {
        hsl.h = 0;
    }
    else if (c_max == r) {
        hsl.h = 60 * (g - b) / delta;
    }
    else if (c_max == g) {
        hsl.h = 60 * (b - r) / delta + 120;
    }
    else { // c_max == b
        hsl.h = 60 * (r - g) / delta + 240;
    }
    if (hsl.h < 0) {
        hsl.h += 360;
    }

    return hsl;
}

// Converts an integer-based HSL structure back to a 15-bit PS1 BGR color.
// Saturation and Lightness are on a 0-255 scale.
PS1Color HSLToPS1Color(HSL hsl) {
    PS1Color new_color;

    // Pure gray, no saturation
    if (hsl.s == 0) {
        new_color.r = (hsl.l + 4) / 8;
        new_color.g = (hsl.l + 4) / 8;
        new_color.b = (hsl.l + 4) / 8;
        new_color.x = 0;
        return new_color;
    }

    int h_360 = hsl.h;
    int s_255 = hsl.s;
    int l_255 = hsl.l;

    int q = (l_255 < 128) ? (l_255 * (256 + s_255)) / 256 : (l_255 + s_255 - (l_255 * s_255) / 256);
    int p = 2 * l_255 - q;

    int r = HSLToRGB_Channel(p, q, h_360 + 120);
    int g = HSLToRGB_Channel(p, q, h_360);
    int b = HSLToRGB_Channel(p, q, h_360 - 120);

    r = (r < 0) ? 0 : (r > 255) ? 255 : r;
    g = (g < 0) ? 0 : (g > 255) ? 255 : g;
    b = (b < 0) ? 0 : (b > 255) ? 255 : b;

    new_color.r = (r + 4) / 8;
    new_color.g = (g + 4) / 8;
    new_color.b = (b + 4) / 8;
    new_color.x = 0;

    return new_color;
}

// Function to be called ONCE at the start of your game or scene
void InitSpyroClut() {
    deckard_memcpy(original_ps1_clut, &original_spyro_clut, sizeof(original_ps1_clut));
}

const int SATURATION_MULTIPLIER = 200;

void TestUpdateSpyroColor() {
    if (_gameState == GAMESTATE_GAMEPLAY && current_menu == SKIN_EDITOR_MENU)
    {
        InitSpyroClut();
        deckard_memcpy(spyro_clut_buffer, original_ps1_clut, sizeof(original_ps1_clut));

        int hue_offset = skin_editor_menu.hue_level;
        int sat_offset_100 = skin_editor_menu.saturation_level;
        int lightness_offset_100 = skin_editor_menu.lightness_level;

        // save hsv data for specific body part
        hslBodyPartData[skin_editor_menu.skin_section].h = hue_offset;
        hslBodyPartData[skin_editor_menu.skin_section].s = sat_offset_100;
        hslBodyPartData[skin_editor_menu.skin_section].l = lightness_offset_100;

        // Loop over the 7 body parts
        for (int j = 0; j < 7; j++)
        {
            // Loop over the pixels for the body part
            for (int i = 0; i < index_array_sizes[j]; ++i) {
                PS1Color original_color = { 0 };
                deckard_memcpy(&original_color, &(original_ps1_clut[(array_of_section_pixels[j])[i]]), sizeof(PS1Color));
                HSL hsl = PS1ColorToHSL(original_color);
                hsl.h += hslBodyPartData[j].h;
                if (hsl.h >= 360) hsl.h -= 360;
                if (hsl.h < 0) hsl.h += 360;

                PS1Color temp_color = HSLToPS1Color(hsl);
                int r = temp_color.r * 8;
                int g = temp_color.g * 8;
                int b = temp_color.b * 8;

                // Use the new multiplier
                r = r + ((r - (r + g + b) / 3) * hslBodyPartData[j].s * SATURATION_MULTIPLIER) / (100 * 100);
                g = g + ((g - (r + g + b) / 3) * hslBodyPartData[j].s * SATURATION_MULTIPLIER) / (100 * 100);
                b = b + ((b - (r + g + b) / 3) * hslBodyPartData[j].s * SATURATION_MULTIPLIER) / (100 * 100);

                if (hslBodyPartData[j].l > 0) {
                    r = r + ((255 - r) * hslBodyPartData[j].l) / 100;
                    g = g + ((255 - g) * hslBodyPartData[j].l) / 100;
                    b = b + ((255 - b) * hslBodyPartData[j].l) / 100;
                }
                else {
                    r = r + (r * hslBodyPartData[j].l) / 100;
                    g = g + (g * hslBodyPartData[j].l) / 100;
                    b = b + (b * hslBodyPartData[j].l) / 100;
                }

                r = (r < 0) ? 0 : (r > 255) ? 255 : r;
                g = (g < 0) ? 0 : (g > 255) ? 255 : g;
                b = (b < 0) ? 0 : (b > 255) ? 255 : b;

                PS1Color final_color;
                final_color.r = ((r + 4) / 8 > 31) ? 31 : (r + 4) / 8;
                final_color.g = ((g + 4) / 8 > 31) ? 31 : (g + 4) / 8;
                final_color.b = ((b + 4) / 8 > 31) ? 31 : (b + 4) / 8;
                final_color.x = 0;

                deckard_memcpy(&(spyro_clut_buffer[(array_of_section_pixels[j])[i]]), &final_color, sizeof(final_color)); //memcpy because deckard
                //spyro_clut_buffer[(array_of_section_pixels[j])[i]] = final_color; // original code

            }
        }

        deckard_memcpy(0x800740B0, spyro_clut_buffer, sizeof(spyro_clut_buffer));
        spyro_color_index = SKIN_CUSTOM;
        WriteSpyroBMPToVram();
    }
}