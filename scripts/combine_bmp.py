import os

# Replace with the list of your file names or paths
file_list = ["og_s1.bin", "dred.bin", "icy.bin", "rblue.bin", "emerald.bin", "gold.bin", "ember.bin", "zera.bin", "berry.bin", "pixie.bin", "jayo.bin", "woke.bin", "ditto.bin", "goth.bin", "cyn_cre.bin"]  # Add all your files here
output_file = "skins.bin"

# Define constants
file_size = 0x200  # Length of each input file
offset_increment = 0x1000  # Offset between each file's start in the output file

# Open the output file in binary write mode
with open(output_file, "wb") as out_f:
    for index, file_name in enumerate(file_list):
        # Calculate the current offset where the file data should start
        current_offset = index * offset_increment

        # Write zero-padding up to the current offset
        out_f.write(b'\x00' * (current_offset - out_f.tell()))

        # Read the content of the current file
        with open(file_name, "rb") as in_f:
            file_content = in_f.read(file_size)
            out_f.write(file_content)

print(f"All files have been combined into {output_file}.")