
#ifndef STDDEF_H
#define STDDEF_H

#define NULL	0

typedef signed long ssize_t;
typedef unsigned long size_t;

#define offsetof(type, member)	\
	((size_t) &(((type *) NULL)->(member)))

#endif

