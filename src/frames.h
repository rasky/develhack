#pragma once

#include <stdint.h>

typedef struct {
	uint8_t idx;
	int8_t movex;
	int8_t movey;
	char *filename;
	uint8_t animidx : 4;
	uint8_t animsz : 4;
	uint8_t speed;
} AnimFrame;

extern const AnimFrame Rasky[];
extern const int RaskyCount;
