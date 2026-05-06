#ifndef SYSCALLS_H
#define SYSCALLS_H

// PS1 BIOS Syscall - printf
// Uses syscall 0x3F (std_out_printf) via 0xA0 vector
int printf_syscall(const char* format, ...);

#endif // SYSCALLS_H
