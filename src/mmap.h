#pragma once

// This file defines the overall static allocations of VRAM / CRAM / OAM.
// Allcations of those memories are made statically as we don't need a
// dynamic allocator for most things.

#define MMAP_OAM_FIGHTERS    0    // 8 required
#define MMAP_OAM_SPECIALS   10    // 8 required
#define MMAP_OAM_BACKGROUND 20    // 10 required
