from PIL import Image
import sys
  
filename = sys.argv[1]
img = Image.open(filename)
    
width, height = img.size
print('width :', width)
print('height:', height)

rgb_img = img.convert('RGB')

bmp_data = []
bmp_pixel_data = []

for y in range(height):
    for x in range(width):
        pixel = rgb_img.getpixel((x, y))
        #print(pixel)
        r, g, b = pixel
        print(f'| R: {hex(r)} | G: {hex(g)} | B: {hex(b)} |')
        
        # bmp_data.append(r)
        # bmp_data.append(g)
        # bmp_data.append(b)
        
        bmp_pixel_data.append(pixel)
        
with open("bmps/" + filename.split("\\")[-1].split("/")[-1].split(".")[0] + ".bin", 'wb') as file:
        for i, data in enumerate(bmp_pixel_data):
            file.write(data[0].to_bytes(1, signed=False, byteorder="little")) #R
            file.write(data[1].to_bytes(1, signed=False, byteorder="little")) #G
            file.write(data[2].to_bytes(1, signed=False, byteorder="little")) #B
            file.write(int(0xFF).to_bytes(1, signed=False, byteorder="little"))