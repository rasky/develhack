#pragma once

#define POOL_FIGHT           (0 << 8)
#define POOL_FIGHT_UNKOWN    (POOL_FIGHT | 0)
#define POOL_FIGHT_SPECIAL   (POOL_FIGHT | 1)
#define POOL_FIGHT_ANIM      (POOL_FIGHT | 2)
#define POOL_FIGHT_STAGE     (POOL_FIGHT | 3)

// Initial allocation
void poolInit(void);

// Reset a memory pool (free all allocations)
void poolReset(int pool);

// Allocate memory from a pool
void* poolMalloc_(int pool, int size);

#define poolMalloc(pool, size) ({ \
	void *buf = poolMalloc_(pool, size); \
	sassert(buf != 0, "memory overflow in pool %x", pool); \
	buf; \
	})


// Load a file from FAT into memory allocated from the pool
void* poolSlurp_(int pool, char *fn);

#define poolSlurp(pool, fn) ({ \
	void *buf = poolSlurp_(pool, fn); \
	sassert(buf != 0, "memory overflow in pool %x", pool); \
	buf; \
	})
