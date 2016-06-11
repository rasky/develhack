#include "frames.h"

#define NEXT(n) ((n)+1)
#define MOVE(x,y) (x),(y)
#define SPEED(n) (16-(n))

const AnimDesc Rasky = {
	{ // frames
		{ 0,  0,        MOVE(0,0), "rasky-idle", 0, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
		{ 1,  0,        MOVE(0,0), "rasky-idle", 1, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
		{ 2,  0,        MOVE(0,0), "rasky-idle", 2, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
		{ 3,  0,        MOVE(0,0), "rasky-idle", 3, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
		{ 4,  0,        MOVE(0,0), "rasky-idle", 4, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
		{ 5,  0,        MOVE(0,0), "rasky-idle", 5, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
		{ 6,  0,        MOVE(0,0), "rasky-idle", 6, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
		{ 7,  NEXT(0),  MOVE(0,0), "rasky-idle", 7, 8, SPEED(10), FST_IDL|FCWALK|FCJMP},
		/******************************************************************/
		{ 8,  0,        MOVE(7,0), "rasky-walk", 0, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		{ 9,  0,        MOVE(7,0), "rasky-walk", 1, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		{ 10, 0,        MOVE(7,0), "rasky-walk", 2, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		{ 11, 0,        MOVE(7,0), "rasky-walk", 3, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		{ 12, 0,        MOVE(7,0), "rasky-walk", 4, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		{ 13, NEXT(8),  MOVE(7,0), "rasky-walk", 5, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		/******************************************************************/
		{ 14, 0,        MOVE(-7,0), "rasky-walk", 5, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		{ 15, 0,        MOVE(-7,0), "rasky-walk", 4, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		{ 16, 0,        MOVE(-7,0), "rasky-walk", 3, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		{ 17, 0,        MOVE(-7,0), "rasky-walk", 2, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		{ 18, 0,        MOVE(-7,0), "rasky-walk", 1, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		{ 19, NEXT(14), MOVE(-7,0), "rasky-walk", 0, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		/******************************************************************/
		{ 20, 0,        MOVE(8,-15), "rasky-idle", 0, 8, SPEED(8), FST_JMP},
		{ 21, 0,        MOVE(8,-10), "rasky-idle", 0, 8, SPEED(8), FST_JMP},
		{ 22, 0,        MOVE(8,-5),  "rasky-idle", 0, 8, SPEED(8), FST_JMP},
		{ 23, 0,        MOVE(8,0),   "rasky-idle", 0, 8, SPEED(8), FST_JMP},
		{ 24, 0,        MOVE(8,5),   "rasky-idle", 0, 8, SPEED(8), FST_JMP},
		{ 25, 0,        MOVE(8,10),  "rasky-idle", 0, 8, SPEED(8), FST_JMP},
		{ 26, NEXT(0),  MOVE(8,15),  "rasky-idle", 0, 8, SPEED(8), FST_JMP},
		{ 0, 0, },
	},
	{ // keyframes
		/* idle */ 0,
		/* fwd  */ 8,
		/* bwd  */ 14,
		/* jump */ 20,
	},
	/* pivot */ 64, 122,
	/* palette */ "rasky-idle.pal.bin",
};

const AnimDesc Dummy = {
	{ // frames
		{ 0,  0,        MOVE(0,0), "dummy-idle", 0, 5, SPEED(10), FST_IDL|FCWALK|FCJMP},
		{ 1,  0,        MOVE(0,0), "dummy-idle", 1, 5, SPEED(10), FST_IDL|FCWALK|FCJMP},
		{ 2,  0,        MOVE(0,0), "dummy-idle", 2, 5, SPEED(10), FST_IDL|FCWALK|FCJMP},
		{ 3,  0,        MOVE(0,0), "dummy-idle", 3, 5, SPEED(10), FST_IDL|FCWALK|FCJMP},
		{ 4,  NEXT(0),  MOVE(0,0), "dummy-idle", 4, 5, SPEED(10), FST_IDL|FCWALK|FCJMP},
		/******************************************************************/
		{ 5,  0,        MOVE(7,0), "dummy-walk", 0, 8, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		{ 6,  0,        MOVE(7,0), "dummy-walk", 1, 8, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		{ 7, 0,        MOVE(7,0), "dummy-walk", 2, 8, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		{ 8, 0,        MOVE(7,0), "dummy-walk", 3, 8, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		{ 9, 0,        MOVE(7,0), "dummy-walk", 4, 8, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		{ 10, 0,        MOVE(7,0), "dummy-walk", 5, 8, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		{ 11, 0,        MOVE(7,0), "dummy-walk", 6, 8, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		{ 12, NEXT(5),  MOVE(7,0), "dummy-walk", 7, 8, SPEED(8),  FST_FWD|FCIDLE|FCBWD|FCJMP},
		/******************************************************************/
		{ 13, 0,        MOVE(-7,0), "dummy-walk", 7, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		{ 14, 0,        MOVE(-7,0), "dummy-walk", 6, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		{ 15, 0,        MOVE(-7,0), "dummy-walk", 5, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		{ 16, 0,        MOVE(-7,0), "dummy-walk", 4, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		{ 17, 0,        MOVE(-7,0), "dummy-walk", 3, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		{ 18, 0,        MOVE(-7,0), "dummy-walk", 2, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		{ 19, 0,        MOVE(-7,0), "dummy-walk", 1, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		{ 20, NEXT(13), MOVE(-7,0), "dummy-walk", 0, 8, SPEED(13),  FST_BWD|FCIDLE|FCFWD|FCJMP},
		/******************************************************************/
		{ 21, 0,        MOVE(8,-15), "dummy-jump", 0, 6, SPEED(8), FST_JMP},
		{ 22, 0,        MOVE(8,-10), "dummy-jump", 1, 6, SPEED(8), FST_JMP},
		{ 23, 0,        MOVE(8,-5),  "dummy-jump", 2, 6, SPEED(8), FST_JMP},
		{ 24, 0,        MOVE(8,5),   "dummy-jump", 3, 6, SPEED(8), FST_JMP},
		{ 25, 0,        MOVE(8,10),  "dummy-jump", 4, 6, SPEED(8), FST_JMP},
		{ 26, NEXT(0),  MOVE(8,15),  "dummy-jump", 5, 6, SPEED(8), FST_JMP},
		/******************************************************************/
		{ 27,  0,        MOVE(0,0), "dummy-punch", 0, 3, SPEED(14), 0},
		{ 28,  0,        MOVE(0,0), "dummy-punch", 1, 3, SPEED(14), 0},
		{ 29,  0,        MOVE(0,0), "dummy-punch", 2, 3, SPEED(14), 0},
		{ 30,  0,        MOVE(0,0), "dummy-punch", 1, 3, SPEED(14), 0},
		{ 31,  NEXT(0),  MOVE(0,0), "dummy-punch", 0, 3, SPEED(14), 0},
		/******************************************************************/
		{ 32,  0,        MOVE(0,0), "dummy-kick", 0, 5, SPEED(12), 0},
		{ 33,  0,        MOVE(0,0), "dummy-kick", 1, 5, SPEED(12), 0},
		{ 34,  0,        MOVE(0,0), "dummy-kick", 2, 5, SPEED(12), 0},
		{ 35,  0,        MOVE(0,0), "dummy-kick", 3, 5, SPEED(12), 0},
		{ 36,  NEXT(0),  MOVE(0,0), "dummy-kick", 4, 5, SPEED(12), 0},

		{ 0, 0, },
	},
	{ // keyframes
		/* idle */ 0,
		/* fwd  */ 5,
		/* bwd  */ 13,
		/* jump */ 21,
		/* punch */ 27,
		/* kick */ 32,
	},
	/* pivot */ 45,119,
	/* palette */ "dummy-idle.pal.bin",
};
