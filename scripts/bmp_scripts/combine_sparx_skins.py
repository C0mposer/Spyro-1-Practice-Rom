import os

# Combines all converted sparx bmps to 1 combined sparx.bin file
def CombineSparxSkins():
    directory = "bmps/"
    file_list = ["og_sparx.bin", "red_sparx.bin", "icy_sparx.bin", "jade_sparx.bin", "darkblue_sparx.bin", "darkpurple_sparx.bin", "orange_sparx.bin", "pink_sparx.bin", "turquoise_sparx.bin", "silver_sparx.bin", "black_sparx.bin", "custom_sparx.bin"]  # Add all your files here
    output_file = "final_output/sparx.bin"

    # Define constants
    file_size = 0x100  # Length of each input file
    offset_increment = 0x100  # Offset between each file's start in the output file. Changed to 0x100 from 0xD8 to make moving to the next sector more convientent in cosmetic.c

    # Open the output file in binary write mode
    with open(output_file, "wb") as out_f:
        for index, file_name in enumerate(file_list):
            # Calculate the current offset where the file data should start
            current_offset = index * offset_increment

            # Write zero-padding up to the current offset
            out_f.write(b'\x00' * (current_offset - out_f.tell()))

            # Read the content of the current file
            with open(directory + file_name, "rb") as in_f:
                file_content = in_f.read(file_size)
                out_f.write(file_content)

    print(f"All sparx skin files have been combined into {output_file}.")

if __name__ == "__main__":
    CombineSparxSkins()