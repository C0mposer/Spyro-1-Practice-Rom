import sys

def read_binary_file(file_path):
    """
    Reads the content of a binary file.

    :param file_path: Path to the binary file
    :return: Content of the file as bytes
    """
    try:
        with open(file_path, 'rb') as file:
            return file.read()
    except FileNotFoundError:
        print(f"Error: The file '{file_path}' was not found.")
        sys.exit(1)
    except IOError as e:
        print(f"IOError: {e}")
        sys.exit(1)

def patch_binary_file(target_file_path, offset, replace_bytes):
    """
    Patches a binary file by replacing bytes at a specific offset with replace_bytes.

    :param target_file_path: Path to the binary file to be patched
    :param offset: Byte offset where the replacement starts
    :param replace_bytes: Bytes sequence to replace with (as a bytes object)
    """
    try:
        with open(target_file_path, 'r+b') as file:
            file.seek(offset)
            file.write(replace_bytes)

        print(f"File '{target_file_path}' has been patched successfully at offset {offset}.")

    except IOError as e:
        print(f"IOError: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

def PatchWadHeadControl():
    target_file = '../build/spyro1_PracticeCodes/wad.wad'
    offset = int(0x51E9C98)
    replace_file_path = '../mods/PracticeCodes/output/head.bin'

    replace_bytes = read_binary_file(replace_file_path)

    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)

    offset = int(0x51E9C88)
    replace_file_path = '../mods/PracticeCodes/output/asmheads.bin'

    replace_bytes = read_binary_file(replace_file_path)

    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)

    offset = int(0x988F68)
    replace_file_path = 'bmps/FlagLODVertexColors.bin'

    replace_bytes = read_binary_file(replace_file_path)

    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)

    #ARTISANS BALLOONIST PATCH FOR ANY%
    offset = int(0x7fa6E8)
    replace_bytes = b'\0' * 0xD4
    
    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)

    bytesHex = 0x03000a240780013cf4772aac
    replace_bytes = bytesHex.to_bytes(12, 'big')
    
    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)

    #PEACE KEEPERS BALLOONIST PATCH FOR ANY%
    offset = int(0x1887b84)
    replace_bytes = b'\0' * 0xD4
    
    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)

    bytesHex = 0x090002240780013cf47722ac
    replace_bytes = bytesHex.to_bytes(12, 'big')
    
    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)

    #MAGIC CRAFTERS BALLOONIST PATCH FOR ANY%
    offset = int(0x2730ee8)
    replace_bytes = b'\0' * 0xEC
    
    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)

    bytesHex = 0x0f0002240780013cf47722ac
    replace_bytes = bytesHex.to_bytes(12, 'big')
    
    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)

    #BEAST MAKERS BALLOONIST PATCH FOR ANY%
    offset = int(0x35948a0)
    replace_bytes = b'\0' * 0xE4
    
    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)

    bytesHex = 0x150002240780013cf47722ac
    replace_bytes = bytesHex.to_bytes(12, 'big')
    
    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)

    #DREAM WEAVERS BALLOONIST PATCH FOR ANY%
    offset = int(0x43e5a74)
    replace_bytes = b'\0' * 0xEC
    
    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)

    bytesHex = 0x1b0002240780013cf47722ac
    replace_bytes = bytesHex.to_bytes(12, 'big')
    
    try:
        patch_binary_file(target_file, offset, replace_bytes)
    except ValueError as ve:
        print(f"ValueError: {ve}")
        sys.exit(1)