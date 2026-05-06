"""
utils.py
General-purpose utilities used across build scripts.
"""

from pathlib import Path
import numpy
from termcolor import colored


def filename_no_ext(path: str) -> str:
    """Return the filename from a path, without its extension."""
    return Path(path).stem


def filename_with_ext(path: str) -> str:
    """Return the filename from a path, including its extension."""
    return Path(path).name


def reverse_file_endianness(path: str) -> None:
    """In-place 32-bit big-endian byte swap. Used to convert the Deckard PS2M.BIN payload."""
    resolved = Path(path).resolve()
    data = numpy.fromfile(str(resolved), dtype=numpy.uint32)
    data.byteswap(inplace=True)
    data.tofile(str(resolved))
    print(colored(f"Reversed endianness: {resolved}", "green"))
