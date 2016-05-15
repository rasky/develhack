#pragma once

#include <nds.h>

// Prints formatted output to the iDeaS debug console
// Returns false when text has not been printed, true otherwise.
int debugf(const char* format, ...);

// Prints a value as a binary string, with the given label prefix.
int debugb(const char* label, u32 val);

// Trigger a breakpoint in the emulator
void debugBreakpoint();
