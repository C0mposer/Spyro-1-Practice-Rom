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

if __name__ == "__main__":
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