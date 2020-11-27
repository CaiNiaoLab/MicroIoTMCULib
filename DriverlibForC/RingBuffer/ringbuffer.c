#include <ringbuffer.h>

bool bool_compare_and_swap(volatile uint32_t *dest_ptr,
                           uint32_t old_value, uint32_t new_value)
{
    if (*dest_ptr == old_value) {
        *dest_ptr = new_value;
        return true;
    }
    return false;
}

void ring_buffer_init(ring_buffer_t rb, uint8_t *buf, uint32_t size)
{
    rb->buf = buf;
    rb->size = size;
    rb->head = rb->tail = 0;
}

void ring_buffer_deinit(ring_buffer_t rb)
{
}

uint32_t ring_buffer_available(ring_buffer_t rb)
{
    /* use a copy on stack to avoid race conditions of @tail being updated by
     * the writer thread */
    uint32_t _tail = rb->tail;

    if (rb->head > _tail) {
        return rb->size - rb->head + _tail;
    }
    return _tail - rb->head;
}

void ring_buffer_clear(ring_buffer_t rb)
{
    rb->head = rb->tail = 0;
}

uint32_t ring_buffer_space(ring_buffer_t rb)
{
    if (rb->size == 0) {
        return 0;
    }

    /* use a copy on stack to avoid race conditions of @head being updated by
     * the reader thread */
    uint32_t _head = rb->head;
    uint32_t ret = 0;

    if (_head <= rb->tail) {
        ret = rb->size;
    }

    ret += _head - rb->tail - 1;

    return ret;
}

bool ring_buffer_empty(ring_buffer_t rb) 
{
    return rb->head == rb->tail;
}

uint32_t ring_buffer_write(ring_buffer_t rb, const uint8_t *data, uint32_t len)
{
    IoVec vec[2];
    const uint8_t n_vec = ring_buffer_reserve(rb, vec, len);
    uint32_t ret = 0;
    int i;

    for (i = 0; i < n_vec; i++) {
        memcpy(vec[i].data, data + ret, vec[i].len);
        ret += vec[i].len;
    }

    ring_buffer_commit(rb, ret);
    return ret;
}

uint32_t ring_buffer_read(ring_buffer_t rb, uint8_t *data, uint32_t len)
{
    uint32_t ret = ring_buffer_peekbytes(rb, data, len);
    ring_buffer_advance(rb, ret);
    return ret;
}

bool ring_buffer_read_byte(ring_buffer_t rb, uint8_t *data)
{
    if (!data) {
        return false;
    }

    int16_t ret = ring_buffer_peek(rb, 0);
    if (ret < 0) {
        return false;
    }

    *data = ret;

    return ring_buffer_advance(rb, 1);
}

bool ring_buffer_update(ring_buffer_t rb, const uint8_t *data, uint32_t len)
{
    if (len > ring_buffer_available(rb)) {
        return false;
    }
    // perform as two memcpy calls
    uint32_t n = rb->size - rb->head;
    if (n > len) {
        n = len;
    }
    memcpy(&rb->buf[rb->head], data, n);
    data += n;
    if (len > n) {
        memcpy(&rb->buf[0], data, len-n);
    }
    return true;
}

// return size of ringbuffer
uint32_t ring_buffer_get_size(ring_buffer_t rb) 
{
    return rb->size; 
}

bool ring_buffer_advance(ring_buffer_t rb, uint32_t n)
{
    if (n > ring_buffer_available(rb)) {
        return false;
    }
#if 0
    rb->head = (rb->head + n) % rb->size;
    return true;
#else
    return bool_compare_and_swap(&rb->head, rb->head, (rb->head + n) % rb->size);
#endif
}

uint8_t *ring_buffer_readptr(ring_buffer_t rb, uint32_t *available_bytes)
{
    uint32_t _tail = rb->tail;
    *available_bytes = (rb->head > _tail) ? rb->size - rb->head : _tail - rb->head;

    return available_bytes ? &rb->buf[rb->head] : NULL;
}

int16_t ring_buffer_peek(ring_buffer_t rb, uint32_t ofs)
{
    if (ofs >= ring_buffer_available(rb)) {
        return -1;
    }
    return rb->buf[(rb->head+ofs)%rb->size];
}

uint32_t ring_buffer_peekbytes(ring_buffer_t rb, uint8_t *data, uint32_t len)
{
    IoVec vec[2];
    const uint8_t n_vec = ring_buffer_peekiovec(rb, vec, len);
    uint32_t ret = 0;
    int i;

    for (i = 0; i < n_vec; i++) {
        memcpy(data + ret, vec[i].data, vec[i].len);
        ret += vec[i].len;
    }

    return ret;
}

uint8_t ring_buffer_peekiovec(ring_buffer_t rb, IoVec iovec[2], uint32_t len)
{
    uint32_t n = ring_buffer_available(rb);

    if (len > n) {
        len = n;
    }
    if (len == 0) {
        return 0;
    }

    uint8_t *b = ring_buffer_readptr(rb, &n);
    if (n > len) {
        n = len;
    }

    iovec[0].data = (uint8_t *)b;
    iovec[0].len = n;

    if (len <= n) {
        return 1;
    }

    iovec[1].data = rb->buf;
    iovec[1].len = len - n;

    return 2;
}


uint8_t ring_buffer_reserve(ring_buffer_t rb, IoVec iovec[2], uint32_t len)
{
    uint32_t n = ring_buffer_space(rb);

    if (len > n) {
        len = n;
    }

    if (!len) {
        return 0;
    }

    iovec[0].data = &rb->buf[rb->tail];

    n = rb->size - rb->tail;
    if (len <= n) {
        iovec[0].len = len;
        return 1;
    }

    iovec[0].len = n;

    iovec[1].data = rb->buf;
    iovec[1].len = len - n;

    return 2;
}

bool ring_buffer_commit(ring_buffer_t rb, uint32_t len)
{
    if (len > ring_buffer_space(rb)) {
        return false; //Someone broke the agreement
    }
#if 0
    rb->tail = (rb->tail + len) % rb->size;
    return true;
#else
    return bool_compare_and_swap(&rb->tail, rb->tail, (rb->tail + len) % rb->size);
#endif
}
