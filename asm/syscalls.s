.set noreorder
.align 2
.global printf_syscall
.type printf_syscall, @function
printf_syscall:
    li    $t2, 0xA0
    jr    $t2
    li    $t1, 0x3F


