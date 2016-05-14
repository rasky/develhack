#include "frames.h"

#define MOVE(x,y) (x),(y)
#define SPEED(n) (16-(n))

const AnimFrame Rasky[] = {
	{ 0,  MOVE(0,0), "rasky-idle", 0, 8, SPEED(10)},
	{ 1,  MOVE(0,0), "rasky-idle", 1, 8, SPEED(10)},
	{ 2,  MOVE(0,0), "rasky-idle", 2, 8, SPEED(10)},
	{ 3,  MOVE(0,0), "rasky-idle", 3, 8, SPEED(10)},
	{ 4,  MOVE(0,0), "rasky-idle", 4, 8, SPEED(10)},
	{ 5,  MOVE(0,0), "rasky-idle", 5, 8, SPEED(10)},
	{ 6,  MOVE(0,0), "rasky-idle", 6, 8, SPEED(10)},
	{ 7,  MOVE(0,0), "rasky-idle", 7, 8, SPEED(10)},
	/******************************************************************/
	{ 8,  MOVE(4,0), "rasky-walk", 0, 6, SPEED(8)},
	{ 9,  MOVE(4,0), "rasky-walk", 1, 6, SPEED(8)},
	{ 10, MOVE(4,0), "rasky-walk", 2, 6, SPEED(8)},
	{ 11, MOVE(4,0), "rasky-walk", 3, 6, SPEED(8)},
	{ 12, MOVE(4,0), "rasky-walk", 4, 6, SPEED(8)},
	{ 13, MOVE(4,0), "rasky-walk", 5, 6, SPEED(8)},
};

const int RaskyCount = sizeof(Rasky) / sizeof(AnimFrame);
