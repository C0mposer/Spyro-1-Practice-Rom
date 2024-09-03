from PIL import Image
import sys

def RGBToVramBGR(colour):
    # Extract the RGB components
    r = colour[0] >> 3  # Bottom 3 bits are lost
    g = colour[1] >> 3  # Bottom 3 bits are lost
    b = colour[2] >> 3  # Bottom 3 bits are lost
    
    # Combine into a 15-bit value
    converted = (b << 10) | (g << 5) | r
    

    converted |= 0x8000  # Set the most significant bit for transparency

    return converted

    
filename = sys.argv[1]
img = Image.open(filename)
    
width, height = img.size
print('width :', width)
print('height:', height)

rgb_img = img.convert('RGB')

bmp_data = []
sixteen_bit_bmp_data = [int]

for y in range(height):
    for x in range(width):
        pixel = rgb_img.getpixel((x, y))
        #print(pixel)
        r, g, b = pixel
        print(f'| R: {hex(r)} | G: {hex(g)} | B: {hex(b)} |')
        
        # bmp_data.append(r)
        # bmp_data.append(g)
        # bmp_data.append(b)
        
        sixteen_bit_bmp_data.append(RGBToVramBGR(pixel))
        
with open("temp/" + filename.split("\\")[-1].split("/")[-1] + "_temp", "wb+") as file:
        for i, data in enumerate(sixteen_bit_bmp_data):
            
            # Needs to be alligned to an int, so place padding after the first section
            if i == 46:
                file.write(int(0).to_bytes(2, signed=False, byteorder="little"))
                
            file.write(data.to_bytes(2, signed=False, byteorder='little'))

with open("temp/" + filename.split("\\")[-1].split("/")[-1] + "_temp", 'rb') as in_file:
    with open("bmps/" + filename.split("\\")[-1].split("/")[-1].split(".")[0] + ".bin", 'wb') as out_file:
        out_file.write(in_file.read()[1:])