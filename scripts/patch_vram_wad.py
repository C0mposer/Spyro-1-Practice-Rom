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
    "metalhead" : 0x4035000,
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
    "gnastys_loot" : 0x5af7000
}

def patch_vram_in_wad(x_vram, y_vram, width, height, level, patch_data):
    y_vram -= 1

    wad_offset = vram_offset_in_wad[level]


    if(bytes(patch_data[0:256]) == bytes(b'\x00\x01\x02\x03\x04\x05\x06\x07\x08\t\n\x0b\x0c\r\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaa\xab\xac\xad\xae\xaf\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xba\xbb\xbc\xbd\xbe\xbf\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xca\xcb\xcc\xcd\xce\xcf\xd0\xd1\xd2\xd3\xd4\xd5\xd6\xd7\xd8\xd9\xda\xdb\xdc\xdd\xde\xdf\xe0\xe1\xe2\xe3\xe4\xe5\xe6\xe7\xe8\xe9\xea\xeb\xec\xed\xee\xef\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff')):
        start_row = 8
    elif(bytes(patch_data[0:8]) == bytes(b'\x102Tv\x98\xba\xdc\xfe')):            #for bmp's with a clut on the top, it skips the first row
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
        # print(f"Pixel data read successfully from {texture_bmp}.")
        # print(f"Width: {width} pixels, Height: {height} pixels, Row size: {row_size} bytes.")
        # print()
        # print("Raw 4-bit pixel data (top to bottom) and bit-endian flipped:")

        # Flip the endianess of the 4-bit data
        flipped_pixel_data = flip_4bit_endianness(pixel_data)
        return flipped_pixel_data
    except Exception as e:
        print(f"An error occurred: {e}")

def convert_8bit_texture(texture_bmp):
    try:
        pixel_data, width, height, row_size = read_bmp_8bit(texture_bmp)
        # print(f"Pixel data read successfully from {texture_bmp}.")
        # print(f"Width: {width} pixels, Height: {height} pixels, Row size: {row_size} bytes.")
        # print()
        # print("Raw 8-bit pixel data (top to bottom):")
        # print_pixel_data_by_row(pixel_data, width, height, row_size)
        return pixel_data
    except Exception as e:
        print(f"An error occurred: {e}")

def extract_and_convert_4bit_clut(clut_bmp, trans_flag):
    input_file_name = GetFileNameFromPathNoExt(clut_bmp)
    output_file = "bmps/" + input_file_name + "_palette.bmp"  # Output path for the palette BMP file

    try:
        # Extract the palette from the BMP
        palette = extract_palette_from_bmp_4bit(clut_bmp)
        # print(f"Extracted {len(palette)} colors from the palette.")

        # Create and save the palette BMP
        create_palette_bmp(palette, output_file)
        # print(f"Palette exported as a BMP to {output_file}.")
    except Exception as e:
        print(f"An error occurred: {e}")


    img = Image.open(output_file)
    
    width, height = img.size
    # print('width :', width)
    # print('height:', height)

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
                file.write(data.to_bytes(2, signed=False, byteorder='little'))

    with open("temp/" + output_file.split("\\")[-1].split("/")[-1] + "_temp", 'rb') as in_file:
        with open("bmps/" + output_file.split("\\")[-1].split("/")[-1].split(".")[0] + ".bin", 'wb') as out_file:
            out_file.write(in_file.read()[1:])

    with open("bmps/" + output_file.split("\\")[-1].split("/")[-1].split(".")[0] + ".bin", 'rb') as file:
        return file.read()

def extract_and_convert_8bit_clut(clut_bmp, trans_flag):
    input_file_name = GetFileNameFromPathNoExt(clut_bmp)
    output_file = "bmp_scripts/bmps/" + input_file_name + "_palette.bmp"  # Output path for the palette BMP file
    
    try:
        palette = extract_palette_from_bmp_8bit(clut_bmp)
        create_8bit_palette_bmp(palette, output_file)
    except Exception as e:
        print(f"An error occurred: {e}")

    img = Image.open(output_file)

    width, height = img.size
    # print('width :', width)
    # print('height:', height)

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

def patch_4bit_texture(x_vram, y_vram, level, texture_bmp, width = 8, height = 32):
    
    texture_data = b""
    texture_data = convert_4bit_texture(texture_bmp)
    
    patch_vram_in_wad(x_vram, y_vram, width, height, level, texture_data)

def patch_4bit_clut(x_vram, y_vram, level, clut_bmp, num_fading_levels = 1, trans_flag = False, width = 16, height = 1):
    
    clut_data = b""
    clut_data = extract_and_convert_4bit_clut(clut_bmp, trans_flag)
    #print(clut_data)
    
    for i in range(num_fading_levels):
        patch_vram_in_wad(x_vram, y_vram + i, width, height, level, clut_data)

def patch_8bit_texture(x_vram, y_vram, level, texture_bmp, width = 16, height = 32):
    texture_data = b""
    texture_data = convert_8bit_texture(texture_bmp)
    
    patch_vram_in_wad(x_vram, y_vram, width, height, level, texture_data)

def patch_8bit_clut(x_vram, y_vram, level, clut_bmp, num_fading_levels = 1, trans_flag = False, width = 256, height = 1):
    clut_data = b""
    clut_data = extract_and_convert_8bit_clut(clut_bmp, trans_flag)
    #print(clut_data)
    
    for i in range(num_fading_levels):
        patch_vram_in_wad(x_vram, y_vram + i, width, height, level, clut_data)

def patch_flame_texture(x_vram, y_vram, level, texture_bmp):
    
    texture_data = b""
    texture_data = convert_4bit_texture(texture_bmp)
    
    patch_vram_in_wad(x_vram, y_vram, 8, 128, level, texture_data)

def PatchArtisansFlag():
    try:
        # Main Flag Texture
        patch_4bit_texture(912, 256, "artisans", "bmp_scripts\\bmps\\Comp_Kara_Logo.bmp")
        patch_4bit_clut(816, 480, "artisans", "bmp_scripts\\bmps\\Comp_Kara_Logo.bmp", 8)

        # Bottom spiral and background
        patch_4bit_texture(904, 320, "artisans", "bmp_scripts\\bmps\\CustomFlag.bmp")
        patch_4bit_texture(904, 352, "artisans", "bmp_scripts\\bmps\\CustomFlag.bmp")
        patch_4bit_clut(784, 480, "artisans", "bmp_scripts\\bmps\\CustomFlag.bmp", 8)
        patch_4bit_clut(800, 480, "artisans", "bmp_scripts\\bmps\\CustomFlag.bmp", 8)

        
        # for levels in vram_offset_in_wad:
        #     patch_flame_texture(960, 384, levels, "bmp_scripts\\bmps\\flame_texture_custom.bmp")

        # 
        for levels in vram_offset_in_wad: 
            
            if levels in ["artisans", "stone_hill", "town_square", "toasty", "beast_makers", "twilight_harbor"]:    #also cliff town, MC, alpine, wizard, Terrace, metalhead, DW, passage, lofty, jacques
                y_clut = 112
            elif levels in ["blowhard", "tree_tops"]:   #also doctor shemp, blowhard, tree tops, haunted, 
                y_clut = 16
            elif levels in ["peace_keepers", "gnorc_gnexus", "gnasty_gnorc"]:     #also dry canyon, ice cavern, high caves, misty bog cove
                y_clut = 0
            else:
                continue

            patch_8bit_texture(768, 0, levels, "bmp_scripts\\bmps\\teehee.bmp", 96, 128)
            patch_8bit_clut(512, y_clut, levels, "bmp_scripts\\bmps\\teehee.bmp", 16)



        patch_4bit_texture(808, 448, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\out\\out\\gnastylow1.bmp")
        patch_4bit_texture(808, 480, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\out\\out\\gnastylow2.bmp")
        patch_4bit_texture(816, 448, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\out\\out\\gnastylow3.bmp")
        patch_4bit_texture(816, 480, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\out\\out\\gnastylow4.bmp")
        patch_4bit_clut(928, 128, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\out\\out\\gnastylow1.bmp", 16)
        patch_4bit_clut(928, 144, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\out\\out\\gnastylow2.bmp", 16)
        patch_4bit_clut(944, 112, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\out\\out\\gnastylow3.bmp", 16)
        patch_4bit_clut(944, 128, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\out\\out\\gnastylow4.bmp", 16)


        # patch_8bit_texture(640, 448, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\gnastybmps-viaphotoshop\\gg1.bmp")
        # patch_8bit_texture(624, 480, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\gnastybmps-viaphotoshop\\gg2.bmp")
        # patch_8bit_texture(624, 448, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\gnastybmps-viaphotoshop\\gg3.bmp")
        # patch_8bit_texture(608, 480, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\gnastybmps-viaphotoshop\\gg4.bmp")
        # patch_8bit_clut(512, 147, "gnastys_loot", "C:\\Users\\Kara\\Downloads\\gnastybmps-viaphotoshop\\gnasty1.bmp", 1)


        print()
        print("WAD VRAM patch successful")
        print()
    
    except Exception as e:
        print(f"An error occurred: {e}")



if __name__ == "__main__":
    
    PatchArtisansFlag()