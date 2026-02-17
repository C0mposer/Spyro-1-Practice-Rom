.set noreorder

.global printf_sys
.type printf_sys, @function
printf_sys:
    li    $t2, 0xA0
    jr    $t2
    li    $t1, 0x3F

.global LoadExec
.type LoadExec, @function
LoadExec:
    li    $t2, 0xA0
    jr    $t2
    li    $t1, 0x51
    nop

.global InitCARD2
.type InitCARD2, @function
InitCARD2:
    li    $t2, 0xB0
    jr    $t2
    li    $t1, 0x4A
    nop

.global StartCARD2
.type StartCARD2, @function
StartCARD2:
    li    $t2, 0xB0
    jr    $t2
    li    $t1, 0x4B
    nop

.global bu_init
.type bu_init, @function
bu_init:
    li    $t2, 0xA0
    jr    $t2
    li    $t1, 0x70
    nop

.global _96_remove
.type _96_remove, @function
_96_remove:
    li    $t2, 0xA0
    jr    $t2
    li    $t1, 0x72
    nop

# A(00h) open(filename, accessmode)
.global bios_open
.type bios_open, @function
bios_open:
    li    $t2, 0xA0
    jr    $t2
    li    $t1, 0x00
    nop

# A(02h) read(fd, dst, len)
.global bios_read
.type bios_read, @function
bios_read:
    li    $t2, 0xA0
    jr    $t2
    li    $t1, 0x02
    nop

# A(03h) write(fd, src, len)
.global bios_write
.type bios_write, @function
bios_write:
    li    $t2, 0xA0
    jr    $t2
    li    $t1, 0x03
    nop

# A(04h) close(fd)
.global bios_close
.type bios_close, @function
bios_close:
    li    $t2, 0xA0
    jr    $t2
    li    $t1, 0x04
    nop

# B(54h) _get_errno()
.global bios_get_errno
.type bios_get_errno, @function
bios_get_errno:
    li    $t2, 0xB0
    jr    $t2
    li    $t1, 0x54
    nop

# B(4Ch) _card_info(port)
.global _card_info
.type _card_info, @function
_card_info:
    li    $t2, 0xB0
    jr    $t2
    li    $t1, 0x4C
    nop

# B(4Dh) _card_load(port)  
.global _card_load
.type _card_load, @function
_card_load:
    li    $t2, 0xB0
    jr    $t2
    li    $t1, 0x4D
    nop

# B(4Eh) _card_clear(port)
.global _card_clear
.type _card_clear, @function
_card_clear:
    li    $t2, 0xB0
    jr    $t2
    li    $t1, 0x4E
    nop

# B(56h) _card_stop  
.global _card_stop
.type _card_stop, @function
_card_stop:
    li    $t2, 0xB0
    jr    $t2
    li    $t1, 0x56
    nop