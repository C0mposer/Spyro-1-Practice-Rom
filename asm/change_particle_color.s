#! Custom Particle Color Code

#Set Colors
sb $a0, 0xc($s2) #Red
sb $a1, 0xd($s2) #Green
sb $a2, 0xe($s2) #Blue

lw $v0, 0x8000f738  #Load end_of_rand_switch
jr $v0              #Jump to end_of_rand_switch to resume normal control flow
nop