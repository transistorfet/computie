
#ifndef _SRC_KERNEL_PROC_CONTEXT_H
#define _SRC_KERNEL_PROC_CONTEXT_H

extern void *create_context(void *user_stack, void *entry, void *exit);
extern void *drop_context(void *user_stack);
extern void _exit();
extern void _sigreturn();

#endif
