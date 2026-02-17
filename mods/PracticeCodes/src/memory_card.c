// memory_card.c
#include <common.h>

// =========================
// Config
// =========================

// Must be <= 8 chars, uppercase, no extension
#define SAVE_NAME "bu00:SPYROPRC"

#define O_RDONLY  0x0001
#define O_WRONLY  0x0002
#define O_CREAT   0x0200

// For memcards: blocks go in bits 16..31
#define MEMCARD_BLOCKS(n) ((n) << 16)

#define SAVE_BLOCKS 1
#define SAVE_IO_SIZE 0x80


// =========================
// BIOS syscall declarations
// =========================

extern int bios_open(const char* filename, int accessmode);
extern int bios_read(int fd, void* dst, int len);
extern int bios_write(int fd, const void* src, int len);
extern int bios_close(int fd);
extern int bios_get_errno(void);
extern int _card_info(int port);
extern int _card_load(int port);


// =========================
// Memcard init
// =========================

void PrepareMemcard(void)
{
    _card_stop();
    InitCARD2(1);
    StartCARD2();
    bu_init();
    _card_load(0);
}


// =========================
// Save / Load
// =========================

int SavePracticeSettings(void)
{

    // Re-verify device is accessible
    int test_fd = bios_open("bu00:", O_RDONLY);
    printf("test open: fd=%d errno=%d\n", test_fd, bios_get_errno());
    if (test_fd >= 0) {
        bios_close(test_fd);
        printf("Device accessible\n");
    }
    else {
        printf("Device NOT accessible\n");
        PrepareMemcard();
    }

    unsigned char buf[SAVE_IO_SIZE];
    for (int i = 0; i < SAVE_IO_SIZE; i++)
        buf[i] = 0;

    ((int*)buf)[0] = _globalGems;
    printf("Saving gems = %d\n", _globalGems);

    // Try O_CREAT alone first
    printf("Attempting create...\n");
    int mode_create = O_CREAT | MEMCARD_BLOCKS(SAVE_BLOCKS);
    int fd_create = bios_open(SAVE_NAME, mode_create);
    printf("create: fd=%d errno=%d\n", fd_create, bios_get_errno());

    if (fd_create >= 0) {
        bios_close(fd_create);
        printf("File created\n");
    }

    // Try opening for write
    printf("Attempting write open...\n");
    int fd = bios_open(SAVE_NAME, O_WRONLY);
    printf("open for write: fd=%d errno=%d\n", fd, bios_get_errno());

    if (fd < 0) {
        printf("Failed to open for writing\n");
        return 0;
    }

    int w = bios_write(fd, buf, SAVE_IO_SIZE);
    printf("write ret=%d errno=%d\n", w, bios_get_errno());

    int c = bios_close(fd);
    printf("close ret=%d errno=%d\n", c, bios_get_errno());

    return (w == SAVE_IO_SIZE);
}


int LoadPracticeSettings(void)
{
    int fd = bios_open(SAVE_NAME, O_RDONLY);
    printf("load open: fd=%d\n", fd);

    if (fd < 0)
        return 0;

    unsigned char buf[SAVE_IO_SIZE];

    int r = bios_read(fd, buf, SAVE_IO_SIZE);
    printf("read ret=%d\n", r);

    bios_close(fd);

    if (r != SAVE_IO_SIZE)
        return 0;

    _globalGems = ((int*)buf)[0];
    printf("Loaded gems = %d\n", _globalGems);

    return 1;
}