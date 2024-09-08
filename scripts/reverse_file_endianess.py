import numpy
import os
from termcolor import colored

def ReverseFileEndianness(path):
    numpy.memmap(path, dtype=numpy.int32).byteswap().tofile("build/spyro1_PracticeCodes/BE.bin")
    print(colored("Reversed Endianess of PS2M.bin for code above 0x80A10000", "green"))