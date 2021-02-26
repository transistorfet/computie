
#ifndef _SRC_KERNEL_MISC_CIRCLEBUF_H
#define _SRC_KERNEL_MISC_CIRCLEBUF_H

#define CIRCLE_BUFFER_SIZE	512

struct circular_buffer {
	short max;
	volatile short in;
	volatile short out;
	volatile unsigned char buffer[CIRCLE_BUFFER_SIZE];
};


static inline void _buf_init(struct circular_buffer *cb, short total_size)
{
	cb->max = total_size ? total_size - sizeof(short) * 3 : CIRCLE_BUFFER_SIZE;
	cb->in = 0;
	cb->out = 0;
}

static inline short _buf_next_in(struct circular_buffer *cb)
{
	return cb->in + 1 < cb->max ? cb->in + 1 : 0;
}

static inline int _buf_is_full(struct circular_buffer *cb)
{
	return _buf_next_in(cb) == cb->out;
}

static inline int _buf_is_empty(struct circular_buffer *cb)
{
	return cb->in == cb->out;
}

static inline short _buf_used_space(struct circular_buffer *cb)
{
	if (cb->in >= cb->out)
		return cb->in - cb->out;
	else
		return cb->max - cb->out + cb->in;
}

static inline short _buf_free_space(struct circular_buffer *cb)
{
	if (cb->out > cb->in)
		return cb->out - cb->in - 1;
	else
		return cb->max - cb->in + cb->out - 1;
}

static inline int _buf_get_char(struct circular_buffer *cb)
{
	register unsigned char ch;

	if (cb->out == cb->in)
		return -1;
	ch = cb->buffer[cb->out++];
	if (cb->out >= cb->max)
		cb->out = 0;
	return ch;
}

static inline int _buf_put_char(struct circular_buffer *cb, unsigned char ch)
{
	register short next;

	next = _buf_next_in(cb);
	if (next == cb->out)
		return -1;
	cb->buffer[cb->in] = ch;
	cb->in = next;
	return ch;
}


static inline short _buf_get(struct circular_buffer *cb, unsigned char *data, short size)
{
	short i;

	for (i = 0; i < size; i++) {
		if (cb->out == cb->in)
			return i;
		data[i] = cb->buffer[cb->out++];
		if (cb->out >= cb->max)
			cb->out = 0;
	}
	return i;
}

static inline short _buf_peek(struct circular_buffer *cb, unsigned char *data, short size)
{
	short i, j;

	for (i = 0, j = cb->out; i < size; i++) {
		if (j == cb->in)
			return i;
		data[i] = cb->buffer[j++];
		if (j >= cb->max)
			j = 0;
	}
	return i;
}

static inline short _buf_put(struct circular_buffer *cb, const unsigned char *data, short size)
{
	short i;
	register short next;

	for (i = 0; i < size; i++) {
		cb->buffer[cb->in] = data[i];
		next = _buf_next_in(cb);
		if (next == cb->out)
			return i;
		cb->in = next;
	}
	return i;
}

static inline short _buf_drop(struct circular_buffer *cb, short size)
{
	short avail = _buf_used_space(cb);

	if (size > avail)
		size = avail;

	cb->out += size;
	if (cb->out > cb->max)
		cb->out -= cb->max;
	return size;
}


#endif

