#include "pool.h"
#include "debug.h"

#include <nds.h>
#include <stdio.h>

static u8 bufFight[16*1024];

typedef struct {
	u8 *mem;
	int size;

	u8 *cur;
	int stats[16];
} Pool;

static Pool pools[1] = {
	{ bufFight, sizeof(bufFight) },
};

void poolInit() {
	for (int i=0;i<sizeof(pools)/sizeof(pools[0]);i++) {
		poolReset(i);
	}
}

void poolReset(int p) {
	pools[p].cur = pools[p].mem;
	memset(pools[p].stats, 0, sizeof(pools[p].stats));
}


void* poolMalloc_(int px, int size) {
	int p = px >> 8;
	int s = px & 0xFF;

	if (pools[p].cur + size > pools[p].mem + pools[p].size) {
		return 0;
	}

	void *buf = pools[p].cur;
	pools[p].cur += size;
	pools[p].stats[s] += size;
	return buf;
}

void* poolSlurp_(int px, char *fn) {
	FILE *fp = fopen(fn, "rb");
	sassert(fp, "file not found: %s", fn);
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	void *buf = poolMalloc_(px, size);
	if (!buf) return 0;

	fread(buf, 1, size, fp);
	fclose(fp);
	return buf;
}
