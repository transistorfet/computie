
#ifndef _KERNEL_BH_H
#define _KERNEL_BH_H

#define SH_MAX			4
#define SH_TTY68681		0
#define SH_TTY			1

typedef void (*bh_handler_t)(void *);

struct bh_handler {
	bh_handler_t fn;
	void *data;
};


void init_bh();
void register_bh(int bhnum, bh_handler_t fn, void *data);
void request_bh_run(int bhnum);
void run_bh_handlers();

#endif
