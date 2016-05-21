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
		{ 20, 0,        MOVE(8,-12), "rasky-idle", 0, 8, SPEED(0), FST_JMP},
		{ 21, 0,        MOVE(8,-5), "rasky-idle", 0, 8, SPEED(0), FST_JMP},
		{ 22, 0,        MOVE(8,5),  "rasky-idle", 0, 8, SPEED(0), FST_JMP},
		{ 23, NEXT(0),  MOVE(8,12),  "rasky-idle", 0, 8, SPEED(0), FST_JMP},
		{ 0, 0, },
	},
	{ // keyframes
		/* idle */ 0,
		/* fwd  */ 8,
		/* bwd  */ 14,
		/* jump */ 20,
	},
	/* pivot */ 64, 122,
};
