import os
import sys
from termcolor import colored

def GetPercentage(current, previous):
    if current == previous:
        return 100.0
    try:
        return (previous - current) / previous * 100.0
    except ZeroDivisionError:
        return 0

BIN_FILES = ["MainHeaderArea.bin", "StartFairyArea.bin", "FairyArea.bin", "MemoryCardArea.bin", "OtherHeader1.bin", "OtherHeader2.bin"]
PS2_AREAS = [0x8000B0B8, 0x800314b4, 0x8001D718, 0x80066840, 0x80008EB0, 0x80007540]
PS2_AREA_SIZES = [0x800, 0x101C, 0xB30, 0x21CC, 0x1A00, 0xF2F]
#PS1_AREAS = [0x8000A91C, 0x800314b4, 0x8001D718, 0x80066840, 0x8000C000, 0x8000EA00]

os.chdir("../mods/PracticeCodes/output/")

for index, current_bin_file in enumerate(BIN_FILES):
    size_of_file = os.path.getsize(current_bin_file)
    bytes_left_in_area = PS2_AREA_SIZES[index] - size_of_file
    amount_of_space_used = GetPercentage(size_of_file, PS2_AREA_SIZES[index])
    full_space = 100
    print(f"We are using {hex(size_of_file)} of {hex(PS2_AREA_SIZES[index])} space in area {current_bin_file}. " + colored(f"We have {hex(bytes_left_in_area)} bytes left. ", "blue") + colored(f"We are using {int(full_space - amount_of_space_used)}% of the space.", "yellow"))
    

