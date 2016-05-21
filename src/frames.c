#include "frames.h"

#define NEXT(n) ((n)+1)
#define MOVE(x,y) (x),(y)
#define SPEED(n) (16-(n))

const AnimDesc Rasky = {
	{ // frames
		{ 0,  0,        MOVE(0,0), "rasky-idle", 0, 8, SPEED(10), FST_IDL|FCWALK},
		{ 1,  0,        MOVE(0,0), "rasky-idle", 1, 8, SPEED(10), FST_IDL|FCWALK},
		{ 2,  0,        MOVE(0,0), "rasky-idle", 2, 8, SPEED(10), FST_IDL|FCWALK},
		{ 3,  0,        MOVE(0,0), "rasky-idle", 3, 8, SPEED(10), FST_IDL|FCWALK},
		{ 4,  0,        MOVE(0,0), "rasky-idle", 4, 8, SPEED(10), FST_IDL|FCWALK},
		{ 5,  0,        MOVE(0,0), "rasky-idle", 5, 8, SPEED(10), FST_IDL|FCWALK},
		{ 6,  0,        MOVE(0,0), "rasky-idle", 6, 8, SPEED(10), FST_IDL|FCWALK},
		{ 7,  NEXT(0),  MOVE(0,0), "rasky-idle", 7, 8, SPEED(10), FST_IDL|FCWALK},
		/******************************************************************/
		{ 8,  0,        MOVE(7,0), "rasky-walk", 0, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD},
		{ 9,  0,        MOVE(7,0), "rasky-walk", 1, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD},
		{ 10, 0,        MOVE(7,0), "rasky-walk", 2, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD},
		{ 11, 0,        MOVE(7,0), "rasky-walk", 3, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD},
		{ 12, 0,        MOVE(7,0), "rasky-walk", 4, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD},
		{ 13, NEXT(8),  MOVE(7,0), "rasky-walk", 5, 6, SPEED(8),  FST_FWD|FCIDLE|FCBWD},
		/******************************************************************/
		{ 14, 0,        MOVE(-7,0), "rasky-walk", 5, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD},
		{ 15, 0,        MOVE(-7,0), "rasky-walk", 4, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD},
		{ 16, 0,        MOVE(-7,0), "rasky-walk", 3, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD},
		{ 17, 0,        MOVE(-7,0), "rasky-walk", 2, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD},
		{ 18, 0,        MOVE(-7,0), "rasky-walk", 1, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD},
		{ 19, NEXT(14), MOVE(-7,0), "rasky-walk", 0, 6, SPEED(13),  FST_BWD|FCIDLE|FCFWD},
	},
	{ // keyframes
		/* idle */ 0,
		/* fwd  */ 8,
		/* bwd  */ 14,
	},
	/* pivot */ 64, 122,
};
