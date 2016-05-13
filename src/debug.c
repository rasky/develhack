#include "debug.h"

#include <stdarg.h>
#include <stdio.h>

#include <nds.h>

static int DEBUG_OUTPUT_ENABLED = 1;

// Gets if the program runs in an emulator.
__attribute__((noinline)) int is_emulator()
{
// The idea behind the code is to overwrite
// the "mov r0, #0" instruction with "mov r0, r0" (NOP).
// On real hardware, the instruction would have been fetched,
// so the overwrite has no effect for the first time executed.
// In order to be able to call the function more than once,
// the original instruction is being restored.
// see http://forum.gbadev.org/viewtopic.php?t=910
#ifdef __thumb__
    int mov_r0_r0 = 0x1c00; // mov r0, r0
    int mov_r0_0 = 0x2000; // mov r0, #0

    asm volatile(
        "mov  r0, %1     \n\t" // r0 = mov_r0_r0
        "mov  r2, %2     \n\t" // r2 = mov_r0_0
        "mov  r1, pc     \n\t" // r1 = program counter
        "strh r0, [r1]   \n\t" // Overwrites following instruction with mov_r0_r0
        "mov  r0, #0     \n\t" // r0 = 0
        "strh r2, [r1]   \n\t" // Restore previous instruction
        : "=r"(mov_r0_r0) // output registers
        : "r"(mov_r0_r0), "r"(mov_r0_0) // input registers
        : "%r1", "%r2" // clobbered registers
        );

#else
    int mov_r0_r0 = 0xe1a00000; // mov r0, r0
    int mov_r0_0 = 0xe3a00000; // mov r0, #0

    asm volatile(
        "mov  r0, %1     \n\t" // r0 = mov_r0_r0
        "mov  r2, %2     \n\t" // r2 = mov_r0_0
        "mov  r1, pc     \n\t" // r1 = program counter
        "str  r0, [r1]   \n\t" // Overwrites following instruction with mov_r0_r0
        "mov  r0, #0     \n\t" // r0 = 0
        "str  r2, [r1]   \n\t" // Restore previous instruction
        : "=r"(mov_r0_r0) // output registers
        : "r"(mov_r0_r0), "r"(mov_r0_0) // input registers
        : "%r1", "%r2" // clobbered registers
        );
#endif

    // TODO: find a way to detect that we're running on ndsemu as well
    return mov_r0_r0 != 0;
}

// This function must be noinline, because
// iDeaS expects the text to output in register r0.
// If this code is inlined somewhere, it's not guaranteed
// that text is located in r0 anymore, thus will not work.
static __attribute__((noinline)) void output_debug_string_internal(const char* text)
{
#ifdef __thumb__
    asm volatile("swi #0xfc" ::"r"(text));
#else
    asm volatile("swi #0xfc000" ::"r"(text));
#endif
}

int debugf(const char* format, ...)
{
    va_list args;
    char buffer[128]; // increase to support more characters

    if (!DEBUG_OUTPUT_ENABLED || !is_emulator())
        return 0;

    va_start(args, 0);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    output_debug_string_internal(buffer);
    return 1;
}

int debugb(const char* label, u32 val)
{
    char buffer[33];
    buffer[32] = '\0';

    for (uint8_t i = 0; i < 32; i++) {
        buffer[i] = val & 1 ? '1' : '0';
        val >>= 1;
    }

    return debugf("%s: %s\n", label, &buffer);
}

// Enables or disables debug output.
// Returns the previous enabled state.
int enable_debug_output(int enable)
{
    int old = DEBUG_OUTPUT_ENABLED;
    DEBUG_OUTPUT_ENABLED = enable;
    return old;
}
