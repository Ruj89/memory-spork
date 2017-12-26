#include "memory.h"

static byte heap[MEMORY_MAX_MEMORY_BYTE_SIZE];
static mallocation_t segments[MEMORY_MAX_SEGMENTS];
static byte errors;

void memory_init(void) {
  msize_t memory_i;
  mindex_t segment_i;

  for (memory_i = 0; memory_i < MEMORY_MAX_MEMORY_BYTE_SIZE; memory_i++) {
    heap[memory_i] = 0;
  }

  for (segment_i = 0; segment_i < MEMORY_MAX_SEGMENTS; segment_i++) {
    segments[segment_i].index = 0;
    segments[segment_i].size = 0;
    segments[segment_i].status = 0x00;
  }
}

mindex_t memory_allocate(msize_t size) {
  mindex_t segment_i;
  for (segment_i = 0; segment_i < MEMORY_MAX_SEGMENTS; segment_i++) {
    if (!memory_segment_occupied(segment_i)) {
      segments[segment_i].index = memory_find_free(size);
      if (memory_catch_error(MEMORY_ERROR_SEGMENT_UNALLOCABLE)) {
        // TODO defrag
        memory_throw_error(MEMORY_ERROR_SEGMENT_UNALLOCABLE);
        return 0;
      } else {
        segments[segment_i].size = size;
        segments[segment_i].status |= MEMORY_SEGMENT_OCCUPIED_FLAG_MASK;
        return segment_i;
      }
    }
  }
  memory_throw_error(MEMORY_ERROR_SEGMENT_ALL_OCCUPIED);
  return 0;
}

void memory_free(mindex_t segment_i) { segments[segment_i].status = 0x00; }

bool memory_segment_occupied(mindex_t segment_i) {
  return segments[segment_i].status & MEMORY_SEGMENT_OCCUPIED_FLAG_MASK;
}

bool memory_segment_contains_memory_index(mindex_t segment_i,
                                          msize_t memory_i) {
  return segments[segment_i].index <= memory_i &&
         segments[segment_i].index + segments[segment_i].size > memory_i;
}

msize_t memory_find_free(msize_t size) {
  msize_t i = 0;
  msize_t last_free_memory_i = 0;
  msize_t analyzed_size = 0;
  mindex_t segment_i;

  while (i < MEMORY_MAX_MEMORY_BYTE_SIZE) {
    segment_i = memory_find_segment_from_memory_index(i);
    if (memory_catch_error(MEMORY_ERROR_NO_SEGMENTS_FOUND)) {
      if (analyzed_size == 0)
        last_free_memory_i = i;
      analyzed_size++;
      i++;
      if (analyzed_size == size)
        return last_free_memory_i;
    } else {
      i = segments[segment_i].index + segments[segment_i].size;
      analyzed_size = 0;
    }
  }
  memory_throw_error(MEMORY_ERROR_SEGMENT_UNALLOCABLE);
  return 0;
}

mindex_t memory_find_segment_from_memory_index(msize_t memory_i) {
  mindex_t segment_i;

  for (segment_i = 0; segment_i < MEMORY_MAX_SEGMENTS; segment_i++) {
    if (memory_segment_occupied(segment_i) &&
        memory_segment_contains_memory_index(segment_i, memory_i))
      return segment_i;
  }
  memory_throw_error(MEMORY_ERROR_NO_SEGMENTS_FOUND);
  return 0;
}

bool memory_catch_error(merror_t error) {
  bool catched;
  catched = errors & error;
  errors &= ~error;
  return catched;
}

void memory_throw_error(merror_t error) { errors |= error; }