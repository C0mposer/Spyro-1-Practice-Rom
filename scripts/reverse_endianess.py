import numpy

numpy.memmap("build/spyro1_PracticeCodes/RAT_CODE.BIN", dtype=numpy.int32).byteswap().tofile("build/spyro1_PracticeCodes/RAT_CODE_BE.BIN")