import os

import numpy
from termcolor import colored


def ReverseFileEndianness(path: str) -> None:
    """In-place 32-bit big-endian swap (Deckard PS2M payload)."""
    path = os.path.abspath(path)
    data = numpy.fromfile(path, dtype=numpy.uint32)
    data.byteswap(inplace=True)
    data.tofile(path)
    print(colored(f"Reversed endianness in-place: {path}", "green"))
