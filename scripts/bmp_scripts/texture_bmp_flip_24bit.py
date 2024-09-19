import struct
import sys

def read_bmp_24bit(input_path):
    with open(input_path, 'rb') as f:
        # Read BMP header (first 54 bytes)
        header = f.read(54)

        # Extract width, height, and bit depth information from the BMP header
        width = struct.unpack('<I', header[18:22])[0]  # Little-endian unsigned int
        height = struct.unpack('<I', header[22:26])[0]
        bit_depth = struct.unpack('<H', header[28:30])[0]  # Little-endian unsigned short

        # Ensure it's a 24-bit BMP file
        if bit_depth != 24:
            raise ValueError("The input image is not a 24-bit BMP file.")

        # Calculate the row size (padded to the nearest 4-byte boundary)
        row_size = ((width * 3 + 3) & ~3)  # Each pixel is 3 bytes in 24-bit BMP

        # Calculate the actual pixel data size without padding
        pixel_data_row_size = width * 3  # 3 bytes per pixel

        # Seek to the start of pixel data (offset is in bytes 10-14 of header)
        pixel_data_offset = struct.unpack('<I', header[10:14])[0]
        f.seek(pixel_data_offset)

        # Read and reverse the pixel data
        raw_data = f.read(row_size * height)
        reversed_data = []

        # Process from bottom to top and reverse, skipping padding bytes
        for y in range(height - 1, -1, -1):
            row_start = y * row_size
            row_end = row_start + pixel_data_row_size  # Only get the actual pixel data
            row_data = raw_data[row_start:row_end]  # Exclude padding bytes

            # Convert BGR to RGB by reversing the order of bytes in each pixel
            row_rgb = bytearray()
            for i in range(0, len(row_data), 3):
                b, g, r = row_data[i:i+3]  # Read BGR
                row_rgb.extend([r, g, b])  # Swap to RGB
            reversed_data.append(row_rgb)

        # Reversed data from top to bottom
        reversed_data = b''.join(reversed_data)
        return reversed_data, width, height, row_size

def save_pixel_data_to_bin(pixel_data, output_file):
    with open(output_file, 'wb') as bin_file:
        bin_file.write(pixel_data)
    print(f"Pixel data saved to {output_file}")

if __name__ == "__main__":
    texture_bmp = sys.argv[1]  # Path to your 24-bit BMP file
    output_bin = sys.argv[2]   # Path to the output .bin file

    try:
        pixel_data, width, height, row_size = read_bmp_24bit(texture_bmp)
        print(f"Pixel data read successfully from {texture_bmp}.")
        print(f"Width: {width} pixels, Height: {height} pixels, Row size: {row_size} bytes.")

        # Save the reversed RGB pixel data to a binary file
        save_pixel_data_to_bin(pixel_data, output_bin)
    except Exception as e:
        print(f"An error occurred: {e}")