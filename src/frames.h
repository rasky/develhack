#pragma once

#include <stdint.h>

#define ANIM_DESC_MAX_FRAMES 64

typedef struct {
	uint8_t idx;
	int8_t movex;
	int8_t movey;
	const char *filename;
	uint8_t animidx : 4;
	uint8_t animsz : 4;
	uint8_t speed;
} AnimFrame;

typedef struct {
	AnimFrame frames[ANIM_DESC_MAX_FRAMES];
	struct {
		uint8_t idle;
		uint8_t forward;
		uint8_t backward;
	} keyframes;
	uint8_t pivotx, pivoty;

} AnimDesc;

extern const AnimDesc Rasky;
