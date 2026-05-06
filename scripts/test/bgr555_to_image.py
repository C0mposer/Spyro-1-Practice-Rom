# Takes a raw bgr555 file (no header), and converts it to an image format

from PIL import Image
import sys


def bytearray_to_2byte(bytes):
    two_byte_integers = []
    for i in range(0, len(bytes), 2):
        chunk = bytes[i:i+2]
        if len(chunk) == 2:
            value = int.from_bytes(chunk, byteorder='little')
            two_byte_integers.append(value)
    return two_byte_integers

# Convert the bgr555 data to RGB
def bgr555_to_rgb(input: bytes):
    converted_pixels = bytes()
    for chunk in input:      
        b = (chunk & 0b0111110000000000) >> 10
        g = (chunk & 0b0000001111100000) >> 5
        r = (chunk & 0b0000000000011111)
        
        r = int(r * 255 / 31) # 31 is 255 in 5bit
        g = int(g * 255 / 31) # 31 is 255 in 5bit
        b = int(b * 255 / 31) # 31 is 255 in 5bit
        
        converted_pixels += r.to_bytes()
        converted_pixels += g.to_bytes()
        converted_pixels += b.to_bytes()
    return converted_pixels


# CLI
if __name__ == "__main__":
    # Usage
    if len(sys.argv) < 4 or sys.argv[1] == "-h" or sys.argv[1] == "-help" :
        print("Convert a bgr555 format file to an image file\n")
        print("Usage:")
        print("py bgr555_to_image.py <input_file> <width> <height>")
        print("py bgr555_to_image.py <input_file> <width> <height> -o <output>")
        print("")
        print("Accepted output formats: BMP | PNG | JPG")
    else:
        input_filename = sys.argv[1]
        format = ".bmp"
        width_input = 0
        height_input = 0
        
        # Determine output filename
        output_file_name = ""
        
        if len(sys.argv) == 4:
            input_file_paths_removed = input_filename.split("\\")[-1].split("/")[-1]
            input_file_ext_removed = input_file_paths_removed.split(".")[0]
            output_file_name = input_file_ext_removed
            
            width_input = int(sys.argv[2])
            height_input = int(sys.argv[3])
            
        elif len(sys.argv) == 5 and sys.argv[4] == "-o":
            print("No output filename given, using BMP as default")
            input_file_paths_removed = input_filename.split("\\")[-1].split("/")[-1]
            input_file_ext_removed = input_file_paths_removed.split(".")[0]
            output_file_name = input_file_ext_removed
            
            width_input = int(sys.argv[2])
            height_input = int(sys.argv[3])
            
        elif len(sys.argv) == 6 and sys.argv[4] == "-o":
            output_file_name = sys.argv[5]
            output_file_paths_removed = output_file_name.split("\\")[-1].split("/")[-1]
            output_file_ext_removed = output_file_paths_removed.split(".")[0]
            output_file_name = output_file_ext_removed
            
            width_input = int(sys.argv[2])
            height_input = int(sys.argv[3])
            
            format = "." + sys.argv[5].split(".")[1] # Get extention
            
            if format.lower() not in [".png", ".jpg", ".bmp"]:
                print("Unrecognized output file format, defaulting to bmp")
                format = ".bmp"
            
        # Process inputted bgr555 file
        input_filename = sys.argv[1]
        file_data = b""
        
        with open(input_filename, "rb+") as file:
            file_data = file.read()
            
        byte_chunks = bytearray_to_2byte(file_data)
        
        converted_data = bgr555_to_rgb(byte_chunks)
        
        print((width_input,height_input))
        img = Image.frombytes(mode="RGB", size=(width_input,height_input), data=converted_data)
        
            

        img.save(output_file_name + format)
        
        print("Created:", output_file_name + format)