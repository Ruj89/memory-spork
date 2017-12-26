#include "types.h"

// TODO: defining a better way to size memory e to define types
#define MEMORY_MAX_MEMORY_BYTE_SIZE 2048
#define MEMORY_MAX_SEGMENTS 64
#define MEMORY_SEGMENT_OCCUPIED_FLAG_MASK 0x01

typedef unsigned short msize_t;
typedef unsigned char mindex_t;

typedef struct {
  msize_t index;
  msize_t size;
  byte status;
} mallocation_t;

typedef enum {
  MEMORY_ERROR_SEGMENT_ALL_OCCUPIED = 0x01,
  MEMORY_ERROR_NO_SEGMENTS_FOUND = 0x02,
  MEMORY_ERROR_SEGMENT_UNALLOCABLE = 0x04
} merror_t;

void memory_init(void);

mindex_t memory_allocate(msize_t size);

void memory_free(mindex_t memory_pointer);

bool memory_segment_occupied(mindex_t segment_i);

bool memory_segment_contains_memory_index(mindex_t segment_i, msize_t memory_i);

msize_t memory_find_free(msize_t size);

mindex_t memory_find_segment_from_memory_index(msize_t memory_i);

bool memory_catch_error(merror_t error);

void memory_throw_error(merror_t error);