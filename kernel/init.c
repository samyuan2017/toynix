#include <string.h>
#include <stdio.h>
#include <x86.h>
#include <kernel/console.h>
#include <kernel/monitor.h>

#define NESTED_TIME_TEST 5

void
init(void)
{
	extern char edata[], end[];

	/*
	* Before doing anything else, complete the ELF loading process.
	* Clear the uninitialized global data (BSS) section of our program.
	* This ensures that all static/global variables start out zero.
	*/
	memset(edata, 0, end - edata);

	/*
	* Initialize the console.
	* Can't call printf until after we do this!
	*/
	cons_init();

	cprintf("Enter toynix...\n");

	// Test the stack backtrace function.
	test_backtrace(NESTED_TIME_TEST);

	// Drop into the kernel monitor.
	while (1)
		monitor(NULL);
}

/*
 * Variable panicstr contains argument to first call to panic; used as flag
 * to indicate that the kernel has already called panic.
 */
const char *panicstr;

/*
 * Panic is called on unresolvable fatal errors.
 * It prints "panic: mesg", and then enters the kernel monitor.
 */
void
_panic(const char *file, int line, const char *fmt,...)
{
	va_list ap;

	if (panicstr)
		goto dead;
	panicstr = fmt;

	// Be extra sure that the machine is in as reasonable state
	asm volatile("cli; cld");

	va_start(ap, fmt);
	cprintf("kernel panic at %s:%d: ", file, line);
	vcprintf(fmt, ap);
	cprintf("\n");
	va_end(ap);

dead:
	/* break into the kernel monitor */
	while (1)
		monitor(NULL);
}

/* like panic, but don't */
void
_warn(const char *file, int line, const char *fmt,...)
{
	va_list ap;

	va_start(ap, fmt);
	cprintf("kernel warning at %s:%d: ", file, line);
	vcprintf(fmt, ap);
	cprintf("\n");
	va_end(ap);
}

