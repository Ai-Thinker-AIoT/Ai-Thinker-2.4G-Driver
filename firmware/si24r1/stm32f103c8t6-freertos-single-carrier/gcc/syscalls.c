/*
 * Minimal newlib system-call stubs for a bare-metal Cortex-M3 target.
 *
 * Replaces -lnosys so that:
 *   1. printf() is wired to the real UART via _write() in BSP/src/bsp_uart.c
 *      (libnosys.a also defines _write as a STRONG symbol, which would clash),
 *   2. the six "xxx is not implemented and will always fail" linker warnings
 *      disappear instead of being noise in every build log.
 *
 * None of these calls are used by this firmware: there is no filesystem, no
 * process model and no dynamic memory allocation. They exist only to satisfy
 * the symbols that newlib-nano's printf/exit paths reference.
 */
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdint.h>

/* USART1 (PA9) low-level byte out, same UART as main.c's USART1_Init(). */
extern void USART1_Putc(uint8_t ch);

/* newlib printf -> _write -> USART1. */
int _write(int fd, const void *buf, size_t len)
{
    const uint8_t *p = (const uint8_t *) buf;
    size_t i;
    (void) fd;
    for (i = 0; i < len; i++) {
        USART1_Putc(p[i]);
    }
    return (int) len;
}

int _close(int fd)
{
    (void) fd;
    return -1;
}

int _fstat(int fd, struct stat *st)
{
    (void) fd;
    if (st != 0) {
        st->st_mode = S_IFCHR;
    }
    return 0;
}

int _isatty(int fd)
{
    (void) fd;
    return 1;
}

int _lseek(int fd, off_t offset, int whence)
{
    (void) fd;
    (void) offset;
    (void) whence;
    return 0;
}

int _read(int fd, void *buf, size_t len)
{
    (void) fd;
    (void) buf;
    (void) len;
    return 0;
}

int _getpid(void)
{
    return 1;
}

int _kill(int pid, int sig)
{
    (void) pid;
    (void) sig;
    errno = EINVAL;
    return -1;
}

void _exit(int status)
{
    (void) status;
    while (1) {
        __asm volatile ("wfi");
    }
}

/* crti/crtn equivalents.
 *
 * The ST GCC startup file calls __libc_init_array(), which in turn calls
 * _init()/_fini() and walks .init_array/.fini_array. A bare-metal build has no
 * crti.o/crtn.o, so provide real (Thumb) no-op functions here. Defining them
 * as linker-script address constants instead would leave the symbols untyped
 * and produce "Unknown destination type (ARM/Thumb)" / "dangerous relocation"
 * from ld. */
void _init(void)
{
}

void _fini(void)
{
}

/*
 * _sbrk is required as soon as anything in newlib touches malloc/free.
 * Keep the classic "heap grows up from end, do not pass the stack" guard.
 */
extern char end;            /* provided by the linker script */
static char *heap_end = 0;

void *_sbrk(ptrdiff_t incr)
{
    extern char _estack;    /* top of RAM, from the linker script */
    char *prev;

    if (heap_end == 0) {
        heap_end = &end;
    }
    prev = heap_end;

    if (heap_end + incr > &_estack) {
        errno = ENOMEM;
        return (void *) -1;
    }
    heap_end += incr;
    return (void *) prev;
}
