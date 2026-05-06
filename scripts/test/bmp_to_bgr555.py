# Takes a 24 bit bmp file, and converts it to the raw PSX bgr555 format (for 16bit textures, and CLUTs)

from PIL import Image
import sys

# Convert the RGB data to bgr555
def rgb_to_bgr555(colour):
    # Extract the RGB components
    r = colour[0] >> 3  # Bottom 3 bits are lost
    g = colour[1] >> 3  # Bottom 3 bits are lost
    b = colour[2] >> 3  # Bottom 3 bits are lost
    
    # Combine into a 15-bit value
    converted = (b << 10) | (g << 5) | r

    return converted


# CLI
if __name__ == "__main__":
    # Usage
    if len(sys.argv) < 2 or sys.argv[1] == "-h" or sys.argv[1] == "-help" :
        print("Convert an image file to the bgr555 format used on PS1 for 16 bit textures, and CLUTs\n")
        print("Usage:")
        print("py image_to_bgr555.py <input_file>")
        print("py image_to_bgr555.py <input_file> -o <output>")
        print("")
        print("Accepted input formats: BMP | PNG | JPG")
    else:
        input_filename = sys.argv[1]
        
        # Determine output filename
        output_file_name = ""
        
        if len(sys.argv) == 2:
            input_file_paths_removed = input_filename.split("\\")[-1].split("/")[-1]
            input_file_ext_removed = input_file_paths_removed.split(".")[0]
            output_file_name = input_file_ext_removed + ".bgr555"
        elif len(sys.argv) == 3 and sys.argv[2] == "-o":
            print("No output filename given, using default")
            input_file_paths_removed = input_filename.split("\\")[-1].split("/")[-1]
            input_file_ext_removed = input_file_paths_removed.split(".")[0]
            output_file_name = input_file_ext_removed + ".bgr555"
        elif len(sys.argv) == 4 and sys.argv[2] == "-o":
            output_file_name = sys.argv[3]
            
        # Process inputted BMP
        img = Image.open(input_filename)
            
        width, height = img.size
        # print('width :', width)
        # print('height:', height)

        rgb_img = img.convert('RGB')

        bmp_data = []
        sixteen_bit_bmp_data = []

        for y in range(height):
            for x in range(width):
                pixel = rgb_img.getpixel((x, y))
                r, g, b = pixel
                #print(f'| R: {hex(r)} | G: {hex(g)} | B: {hex(b)} |')
                
                sixteen_bit_bmp_data.append(rgb_to_bgr555(pixel))
                
        with open(output_file_name, "wb+") as file:
                file_data = b""
                for i, data in enumerate(sixteen_bit_bmp_data):
                    file_data += data.to_bytes(2, signed=False, byteorder='little')
                    
                file.write(file_data)
        
        print("Created:", output_file_name)