
#ifndef _SRC_KERNEL_QUEUE_H
#define _SRC_KERNEL_QUEUE_H

#include <stddef.h>

struct queue {
	struct queue_node *head;
	struct queue_node *tail;
};

struct queue_node {
	struct queue_node *next;
	struct queue_node *prev;
};


static inline void _queue_init(struct queue *queue)
{
	queue->head = NULL;
	queue->tail = NULL;
}

static inline void _queue_insert(struct queue *queue, struct queue_node *node)
{
	if (queue->head)
		queue->head->prev = node;
	else
		queue->tail = node;
	node->next = queue->head;
	node->prev = NULL;
	queue->head = node;
}

static inline void _queue_remove(struct queue *queue, struct queue_node *node)
{
	if (node->next)
		node->next->prev = node->prev;
	else
		queue->tail = node->prev;

	if (node->prev)
		node->prev->next = node->next;
	else
		queue->head = node->next;

	node->next = NULL;
	node->prev = NULL;
}

#endif

