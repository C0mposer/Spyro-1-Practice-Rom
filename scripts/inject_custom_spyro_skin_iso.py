import sys

# Find the offset in a file, where byte sequence exists
def FindBytesInFile(file_path, byte_sequence):
    with open(file_path, "rb") as file:
        
        content = file.read()
        
    offset = content.find(byte_sequence)
    
    if offset != -1:
        print(f"Byte sequence found at offset: {hex(offset)}")
    else:
        print("Byte sequence not found in the file.")
        
    return offset

def InjectCustomSkin(iso_file_path, skin_file_path, offset):
    
    #Get custom skin binary data
    skin_data = b""
    with open(skin_file_path, "rb") as skin_file:
        skin_data = skin_file.read()
        
    #Inject custom skin data into ISO
    with open(iso_file_path, "rb+") as iso_file_path:
        offset_to_beginning_of_custom_spyro = offset + 4
        
        iso_file_path.seek(offset_to_beginning_of_custom_spyro) # +4 because found last 4 bytes of spyro skin
        
        iso_file_path.write(skin_data) # Write the custom skin data

        print(f"Injected custom skin at {hex(offset_to_beginning_of_custom_spyro)}")

if __name__ == "__main__":
    iso_file = sys.argv[1]
    skin_file = sys.argv[2]
    
    file_path = iso_file 
    # The byte sequence to search for
    byte_sequence = bytes.fromhex('B944B23C')

    offset_to_end_of_last_spyro_skin = FindBytesInFile(file_path, byte_sequence)
    
    InjectCustomSkin(iso_file, skin_file, offset_to_end_of_last_spyro_skin)
    