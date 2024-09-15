import sys
from bmp_scripts.texture_bmp_flip_4bit import *
from bmp_scripts.bmp_extract_pallette_4bit import *
from bmp_scripts.texture_bmp_flip_8bit import *
from bmp_scripts.bmp_extract_pallette_8bit import *

vram_offset_in_wad = {
    "artisans" : 0x801000,
    "stone_hill" : 0xB94000,
    "dark_hollow" : 0xF01800,
    "town_square" : 0x119E800,
    "toasty" : 0x148A800,
    "sunny_flight" : 0x16B3000,

    "peace_keepers": 0x1891000,
    "dry_canyon" : 0x1b42800,
    "cliff_town" : 0x1e14000,
    "ice_cavern" : 0x20a6800,
    "doctor_shemp" : 0x238a000,
    "night_flight" : 0x2599800,

    "magic_crafter" : 0x273b800,
    "alpine_ridge" : 0x2a0d800,
    "high_caves" : 0x2cbf000,
    "wizard_peak" : 0x2f6b800,
    "blowhard" : 0x3202800,
    "crystal_flight" : 0x33ea000,

    "beast_makers" : 0x359d000,
    "terrace_village" : 0x3800000,
    "misty_bog" : 0x3a7b000,
    "tree_tops" : 0x3d88800,
    "metal_head" : 0x4035000,
    "wild_flight" : 0x4261000,

    "dream_weavers" : 0x43ed000,
    "dark_passage" : 0x4688000,
    "lofty_castle" : 0x4949800,
    "haunted_towers" : 0x4b70000,
    "jacques" : 0x4dfc800,
    "icy_flight" : 0x503a000,

    "gnorc_gnexus" : 0x51ef000,
    "gnorc_cove" : 0x5458800,
    "twilight_harbor" : 0x56d5800,
    "gnasty_gnorc" : 0x591d000,
    "gnasty's loot" : 0x5af7000
}

def patch_vram_in_wad(x_vram, y_vram, width, height, level, patch_data):
    y_vram -= 1

    wad_offset = vram_offset_in_wad[level]

    if(bytes(patch_data[0:8]) == bytes(b'\x102Tv\x98\xba\xdc\xfe')):            #for bmp's with a clut on the top, it skips the first row
        start_row = 1
    else:
        start_row = 0

    with open("..\\build\\spyro1_PracticeCodes\\WAD.WAD", "rb+") as file:
        for y in range(height):

            offset = (y + y_vram) * 0x400 + (2 * x_vram) + wad_offset
            file.seek(offset)

            patch_start = 2 * ((y + start_row) * width)
            patch_end = 2 * ((y + start_row + 1) * width)
            file.write(patch_data[patch_start:patch_end])

def convert_4bit_texture(texture_bmp):
    try:
        pixel_data, width, height, row_size = read_bmp_4bit(texture_bmp)
        print(f"Pixel data read successfully from {texture_bmp}.")
        print(f"Width: {width} pixels, Height: {height} pixels, Row size: {row_size} bytes.")
        print()
        print("Raw 4-bit pixel data (top to bottom) and bit-endian flipped:")

        # Flip the endianess of the 4-bit data
        flipped_pixel_data = flip_4bit_endianness(pixel_data)
        return flipped_pixel_data
    except Exception as e:
        print(f"An error occurred: {e}")

def convert_8bit_texture(texture_bmp):
    try:
        pixel_data, width, height, row_size = read_bmp_8bit(texture_bmp)
        print(f"Pixel data read successfully from {texture_bmp}.")
        print(f"Width: {width} pixels, Height: {height} pixels, Row size: {row_size} bytes.")
        print()
        print("Raw 8-bit pixel data (top to bottom) and bit-endian flipped:")

        # Flip the endianess of the 4-bit data
        # flipped_pixel_data = flip_4bit_endianness(pixel_data)
        #print_pixel_data_by_row(pixel_data, width, height, row_size)
        return pixel_data
    except Exception as e:
        print(f"An error occurred: {e}")

def extract_and_convert_clut(clut_bmp, trans_flag):
    input_file_name = GetFileNameFromPathNoExt(clut_bmp)
    output_file = "bmps/" + input_file_name + "_palette.bmp"  # Output path for the palette BMP file

    try:
        # Extract the palette from the BMP
        palette = extract_palette_from_bmp_4bit(clut_bmp)
        print(f"Extracted {len(palette)} colors from the palette.")

        # Create and save the palette BMP
        create_palette_bmp(palette, output_file)
        print(f"Palette exported as a BMP to {output_file}.")
    except Exception as e:
        print(f"An error occurred: {e}")


    img = Image.open(output_file)
    
    width, height = img.size
    print('width :', width)
    print('height:', height)

    rgb_img = img.convert('RGB')

    sixteen_bit_bmp_data = [int]

    for y in range(height):
        for x in range(width):
            pixel = rgb_img.getpixel((x, y))
            #print(pixel)
            r, g, b = pixel
            #print(f'| R: {hex(r)} | G: {hex(g)} | B: {hex(b)} |')
            
            # bmp_data.append(r)
            # bmp_data.append(g)
            # bmp_data.append(b)
            
            sixteen_bit_bmp_data.append(RGBToVramBGR(pixel, trans_flag))
            
    with open("temp/" + output_file.split("\\")[-1].split("/")[-1] + "_temp", "wb+") as file:
            for i, data in enumerate(sixteen_bit_bmp_data):
                
                # Needs to be alligned to an int, so place padding after the first section
                if i == 46:
                    file.write(int(0).to_bytes(2, signed=False, byteorder="little"))
                    
                file.write(data.to_bytes(2, signed=False, byteorder='little'))

    with open("temp/" + output_file.split("\\")[-1].split("/")[-1] + "_temp", 'rb') as in_file:
        with open("bmps/" + output_file.split("\\")[-1].split("/")[-1].split(".")[0] + ".bin", 'wb') as out_file:
            out_file.write(in_file.read()[1:])

    with open("bmps/" + output_file.split("\\")[-1].split("/")[-1].split(".")[0] + ".bin", 'rb') as file:
        return file.read()

def RGBToVramBGR(colour, trans_flag):
    # Extract the RGB components
    r = colour[0] >> 3  # Bottom 3 bits are lost
    g = colour[1] >> 3  # Bottom 3 bits are lost
    b = colour[2] >> 3  # Bottom 3 bits are lost
    
    # Combine into a 15-bit value
    converted = (b << 10) | (g << 5) | r
    
    if(trans_flag):
        converted |= 0x8000  # Set the most significant bit for transparency

    return converted

def patch_4bit_texture(x_vram, y_vram, level, texture_bmp):
    
    texture_data = b""
    texture_data = convert_4bit_texture(texture_bmp)
    
    patch_vram_in_wad(x_vram, y_vram, 8, 32, level, texture_data)

def patch_4bit_clut(x_vram, y_vram, fading_flag, trans_flag, level, clut_bmp):
    
    clut_data = b""
    clut_data = extract_and_convert_clut(clut_bmp, trans_flag)
    #print(clut_data)
    
    if(fading_flag):
        height = 8
    else:
        height = 1
    for i in range(8):
        patch_vram_in_wad(x_vram, y_vram + i, 16, 1, level, clut_data)

def patch_flame_texture(x_vram, y_vram, level, texture_bmp):
    
    texture_data = b""
    texture_data = convert_4bit_texture(texture_bmp)
    
    patch_vram_in_wad(x_vram, y_vram, 8, 128, level, texture_data)

def PatchArtisansFlag():
    try:
        #Main Flag Texture
        patch_4bit_texture(912, 256, "artisans", "bmp_scripts\\bmps\\Comp_Kara_Logo.bmp")
        patch_4bit_clut(816, 480, True, False, "artisans", "bmp_scripts\\bmps\\Comp_Kara_Logo.bmp")

        #Bottom spiral and background
        patch_4bit_texture(904, 320, "artisans", "bmp_scripts\\bmps\\CustomFlag.bmp")
        patch_4bit_texture(904, 352, "artisans", "bmp_scripts\\bmps\\CustomFlag.bmp")
        patch_4bit_clut(784, 480, True, False, "artisans", "bmp_scripts\\bmps\\CustomFlag.bmp")
        patch_4bit_clut(800, 480, True, False, "artisans", "bmp_scripts\\bmps\\CustomFlag.bmp")

        
        # for levels in vram_offset_in_wad:
        #     patch_flame_texture(960, 384, levels, "bmp_scripts\\bmps\\flame_texture_custom.bmp")

        texture_8bit_data = convert_8bit_texture("bmp_scripts\\bmps\\teehee.bmp")
        patch_vram_in_wad(768, 0, 96, 128, "artisans", texture_8bit_data)

        palette = extract_palette_from_bmp_8bit("bmp_scripts\\bmps\\teehee.bmp")
        create_8bit_palette_bmp(palette, "pls_palette.bmp")

        output_file = "pls_palette.bmp"
        img = Image.open(output_file)
    
        width, height = img.size
        print('width :', width)
        print('height:', height)

        rgb_img = img.convert('RGB')

        sixteen_bit_bmp_data = [int]

        for y in range(height):
            for x in range(width):
                pixel = rgb_img.getpixel((x, y))
                #print(pixel)
                r, g, b = pixel
                #print(f'| R: {hex(r)} | G: {hex(g)} | B: {hex(b)} |')
                
                # bmp_data.append(r)
                # bmp_data.append(g)
                # bmp_data.append(b)
                
                sixteen_bit_bmp_data.append(RGBToVramBGR(pixel, False))
                
        with open("temp/" + output_file.split("\\")[-1].split("/")[-1] + "_temp", "wb+") as file:
                for i, data in enumerate(sixteen_bit_bmp_data):
                    
                    # Needs to be alligned to an int, so place padding after the first section
                    if i == 46:
                        file.write(int(0).to_bytes(2, signed=False, byteorder="little"))
                        
                    file.write(data.to_bytes(2, signed=False, byteorder='little'))

        with open("temp/" + output_file.split("\\")[-1].split("/")[-1] + "_temp", 'rb') as in_file:
            with open("bmps/" + output_file.split("\\")[-1].split("/")[-1].split(".")[0] + ".bin", 'wb') as out_file:
                out_file.write(in_file.read()[1:])

        with open("bmps/" + output_file.split("\\")[-1].split("/")[-1].split(".")[0] + ".bin", 'rb') as file:
            clut_8bit_data = file.read()

        for i in range(8):
            patch_vram_in_wad(512, 112 + i, 256, 1, "artisans", clut_8bit_data)

        print()
        print("WAD VRAM patch successful")
        print()
    
    except Exception as e:
        print(f"An error occurred: {e}")



if __name__ == "__main__":
    
    PatchArtisansFlag()