#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


typedef struct ring_buffer {
    uint8_t *buf;
    uint32_t size;

    uint32_t head;
    uint32_t tail;
} ring_buffer, *ring_buffer_t;

void ring_buffer_init(ring_buffer_t rb, uint8_t *buf, uint32_t size);

void ring_buffer_deinit(ring_buffer_t rb);

// number of bytes available to be read
uint32_t ring_buffer_available(ring_buffer_t rb);

// Discards the buffer content, emptying it.
void ring_buffer_clear(ring_buffer_t rb);

// number of bytes space available to write
uint32_t ring_buffer_space(ring_buffer_t rb);

// true if available() is zero
bool ring_buffer_empty(ring_buffer_t rb);

// write bytes to ringbuffer. Returns number of bytes written
uint32_t ring_buffer_write(ring_buffer_t rb, const uint8_t *data, uint32_t len);

// read bytes from ringbuffer. Returns number of bytes read
uint32_t ring_buffer_read(ring_buffer_t rb, uint8_t *data, uint32_t len);

// read a byte from ring buffer. Returns true on success, false otherwise
bool ring_buffer_read_byte(ring_buffer_t rb, uint8_t *data);

/*
  update bytes at the read pointer. Used to update an object without
  popping it
*/
bool ring_buffer_update(ring_buffer_t rb, const uint8_t *data, uint32_t len);

// return size of ringbuffer
uint32_t ring_buffer_get_size(ring_buffer_t rb);

// advance the read pointer (discarding bytes)
bool ring_buffer_advance(ring_buffer_t rb, uint32_t n);

// Returns the pointer and size to a contiguous read of the next available data
uint8_t *ring_buffer_readptr(ring_buffer_t rb, uint32_t *available_bytes);

// peek one byte without advancing read pointer. Return byte
// or -1 if none available
int16_t ring_buffer_peek(ring_buffer_t rb, uint32_t ofs);

/*
  read len bytes without advancing the read pointer
*/
uint32_t ring_buffer_peekbytes(ring_buffer_t rb, uint8_t *data, uint32_t len);

// Similar to peekbytes(), but will fill out IoVec struct with
// both parts of the ring buffer if wraparound is happening, or
// just one part. Returns the number of parts written to.
typedef struct _IoVec {
    uint8_t *data;
    uint32_t len;
}IoVec;
uint8_t ring_buffer_peekiovec(ring_buffer_t rb, IoVec vec[2], uint32_t len);

// Reserve `len` bytes and fills out `vec` with both parts of the
// ring buffer (if wraparound is happening), or just one contiguous
// part. Returns the number of `vec` elements filled out. Can be used
// with system calls such as `readv()`.
//
// After a call to 'reserve()', 'write()' should never be called
// until 'commit()' is called!
uint8_t ring_buffer_reserve(ring_buffer_t rb, IoVec vec[2], uint32_t len);

/*
 * "Releases" the memory previously reserved by 'reserve()' to be read.
 * Committer must inform how many bytes were actually written in 'len'.
 */
bool ring_buffer_commit(ring_buffer_t rb, uint32_t len);
