# This code replaces the dragon count for each level in the inventory menu, with the amount of fly-ins.

# Since this is replacing logic in the middle of a function, there are some registers being used here that
# already have been loaded

la   $a3, fly_in_resets  # Load the base address of the fly_in_resets array into $a3
sll  $t0, $v1, 1         # v1 already has the level index in the inventory menu, so I am multiplying it by 2 to index into the short array
add  $t1, $a3, $t0       # Add the base address and the byte offset to get the effective address of &fly_in_resets[t0]
lh   $a2, 0($t1)         # Get the value of fly_in_resets[t0]
la $a1, 0x800755bc       # Load ptr to %d
move $a0, $s6            # Load the text buffer from s6, into a0 for sprintf
jal sprintf

# Normal logic, just copied to be shifted down in the function.
move $a1, $s4 
li $a2, 0x12
li $a3, 0x2
jal DrawTextCapitals