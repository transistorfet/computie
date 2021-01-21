
#include <stddef.h>
#include <kernel/bh.h>


int bh_requested;
static struct bh_handler bh_handlers[SH_MAX];

void init_bh()
{
	bh_requested = 0;
	for (short i = 0; i < SH_MAX; i++) {
		bh_handlers[i].fn = NULL;
		bh_handlers[i].data = NULL;
	}
}

void register_bh(int bhnum, bh_handler_t fn, void *data)
{
	bh_handlers[bhnum].fn = fn;
	bh_handlers[bhnum].data = data;
}

void request_bh_run(int bhnum)
{
	bh_requested |= (0x0001 << bhnum);
}

void run_bh_handlers()
{
	int bit = 0x0001;

	for (short i = 0; i < SH_MAX; i++, bit <<= 1) {
		if (bh_requested & bit) {
			bh_requested &= ~bit;
			if (!bh_handlers[i].fn)
				continue;
			bh_handlers[i].fn(bh_handlers[i].data);
		}
	}
}

