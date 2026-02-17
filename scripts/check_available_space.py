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

BIN_FILES = ["MainHeaderArea.bin", "StartFairyArea.bin", "FairyArea.bin", "MemoryCardArea.bin", "PlayIntroCutscenes.bin", "TitleCameraRelated.bin", "DebugLevelSelect.bin", "ControlDemoModeSpyro.bin", "PrepareFairyGamestate.bin", "kern.bin", "kern2.bin"]
DECKARD_BIN_FILES = ["MainHeaderArea.bin", "StartFairyArea.bin", "FairyArea.bin", "MemoryCardArea.bin", "PlayIntroCutscenes.bin", "TitleCameraRelated.bin", "DebugLevelSelect.bin", "ControlDemoModeSpyro.bin", "PrepareFairyGamestate.bin", "kern.bin", "kern2.bin", "PS2M.bin"]

#PS1_AREAS = [0x8000A91C, 0x800314b4, 0x8001D718, 0x80066840, 0x8000C000, 0x8000EA00]


IOP_AREAS = [0x8000B0B8, 0x800314b4, 0x8001D718, 0x80066840, 0x8002D580, 0x80008EB0, 0x80007540, 0x800539fc]
IOP_AREA_SIZES = [0x7B4, 0x101C, 0xB30, 0x21CC, 0x6A8, 0x43C, 0x28C, 0x268, 0x360, 0x1800, 0xF2F] #Technically we have 0x1A00 available in kern1, but we can only load 0x800 bytes (1 sector) at a time.

DECKARD_AREAS = [0x8000B0B8, 0x800314b4, 0x8001D718, 0x80066840, 0x8002D580, 0x80008EB0, 0x80007540, 0x800539fc, 0x80A60000]
DECKARD_AREA_SIZES = [0x7B4, 0x101C, 0xB30, 0x21CC, 0x6A8, 0x43C, 0x28C, 0x268, 0x360, 0x1800, 0xF2F, 0x2FFFF] #Technically we have 0x1A00 available in kern1, but we can only load 0x800 bytes (1 sector) at a time.

os.chdir("../mods/PracticeCodes/output/")

console = sys.argv[1].lower()

if console == "ps2_deckard":
    for index, current_bin_file in enumerate(DECKARD_BIN_FILES):
        size_of_file = os.path.getsize(current_bin_file)
        bytes_left_in_area = DECKARD_AREA_SIZES[index] - size_of_file
        amount_of_space_used = GetPercentage(size_of_file, DECKARD_AREA_SIZES[index])
        full_space = 100
        print(f"We are using {hex(size_of_file)} of {hex(DECKARD_AREA_SIZES[index])} space in area {current_bin_file}. " + colored(f"We have {hex(bytes_left_in_area)} bytes left. ", "blue") + colored(f"We are using {int(full_space - amount_of_space_used)}% of the space.", "yellow"))

elif console == "ps2_iop":
    for index, current_bin_file in enumerate(BIN_FILES):
        size_of_file = os.path.getsize(current_bin_file)
        bytes_left_in_area = IOP_AREA_SIZES[index] - size_of_file
        amount_of_space_used = GetPercentage(size_of_file, IOP_AREA_SIZES[index])
        full_space = 100
        print(f"We are using {hex(size_of_file)} of {hex(IOP_AREA_SIZES[index])} space in area {current_bin_file}. " + colored(f"We have {hex(bytes_left_in_area)} bytes left. ", "blue") + colored(f"We are using {int(full_space - amount_of_space_used)}% of the space.", "yellow"))    
else:
    print("Unknown Console! Try \"PS2_DECKARD\" or \"PS2_IOP\"")
