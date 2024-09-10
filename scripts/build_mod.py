import os
import sys
from reverse_file_endianess import *
from patch_wad import *

version_to_build = sys.argv[1].upper()

os.chdir("../")

os.chdir("mods/PracticeCodes")

if version_to_build == "PS1":
    print("Building...")
    os.system("py ../../../../tools/mod-builder/main.py 2 1 2 4 2")
    
    os.chdir("../")
    os.chdir("../")
    os.chdir("scripts")
    
    PatchWadHeadControl()                           #Patches head_control.c into wad.wad

    os.chdir("../")
    os.chdir("build")
    
    os.system("mkpsxiso spyro1_PracticeCodes_Manual.xml")
    
    if os.path.isfile("PS1_Practice_Codes.bin"):
        os.remove("PS1_Practice_Codes.bin")
    os.rename("mkpsxiso.bin", "PS1_Practice_Codes.bin")
    with open("PS1_Practice_Codes.cue", "w+") as file:
        cue_string = """FILE "PS1_Practice_Codes.bin" BINARY
 TRACK 01 MODE2/2352
  INDEX 01 00:00:00
                 """
        file.write(cue_string)
    os.remove("mkpsxiso.cue")

elif version_to_build == "PS2_DECKARD":
    print("Building...")
    os.system("py ../../../../tools/mod-builder/main.py 2 1 3 4 3")
    
    os.chdir("../")
    os.chdir("../")
    os.chdir("scripts")
    
    PatchWadHeadControl()                           #Patches head_control.c into wad.wad

    os.chdir("../")

    ReverseFileEndianness("build/spyro1_PracticeCodes/PS2M.BIN") # Reverse PS2M for extra ps2 ram
    
    os.chdir("build")
    
    os.system("mkpsxiso spyro1_PracticeCodes_Manual_DECKARD.xml")
    
    if os.path.isfile("PS2_DECKARD_Practice_Codes.bin"):
        os.remove("PS2_DECKARD_Practice_Codes.bin")
    os.rename("mkpsxiso.bin", "PS2_DECKARD_Practice_Codes.bin")
    with open("PS2_DEKCARD_Practice_Codes.cue", "w+") as file:
        cue_string = """FILE "PS2_DECKARD_Practice_Codes.bin" BINARY
 TRACK 01 MODE2/2352
  INDEX 01 00:00:00
                    """
        file.write(cue_string)
    os.remove("mkpsxiso.cue")
    
elif version_to_build == "PS2_IOP":
    print("Building...")
    os.system("py ../../../../tools/mod-builder/main.py 2 1 4 4 4")
    
    os.chdir("../")
    os.chdir("../")
    os.chdir("scripts")
    
    PatchWadHeadControl()                           #Patches head_control.c into wad.wad

    os.chdir("../")
    os.chdir("build")
    
    os.system("mkpsxiso spyro1_PracticeCodes_Manual.xml")
    
    if os.path.isfile("PS2_IOP_Practice_Codes.bin"):
        os.remove("PS2_IOP_Practice_Codes.bin")
    os.rename("mkpsxiso.bin", "PS2_IOP_Practice_Codes.bin")
    with open("PS2_IOP_Practice_Codes.cue", "w+") as file:
        cue_string = """FILE "PS2_IOP_Practice_Codes.bin" BINARY
 TRACK 01 MODE2/2352
  INDEX 01 00:00:00
                    """
        file.write(cue_string)
    os.remove("mkpsxiso.cue")
    
else:
    print("Unknown Build Version!")
    exit()



print("Done!")
exit()