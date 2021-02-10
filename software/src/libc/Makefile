
ROOT = ../..

AS = m68k-linux-gnu-as
AR = m68k-linux-gnu-ar
CC = m68k-linux-gnu-gcc
OBJDUMP = m68k-linux-gnu-objdump
OBJCOPY = m68k-linux-gnu-objcopy
RANLIB = m68k-linux-gnu-ranlib
INCLUDE = $(ROOT)/include
ASFLAGS = -m68010 --defsym __mc68010__=1
CFLAGS = -I$(INCLUDE) -m68010 -nostartfiles -nostdlib -g -Os

# TODO This works, but you still need a separate place to put object files for each version
#libc-68k-os.a: CFLAGS += -fPIE
$(ROOT)/libc-68k-os.a: CFLAGS += -mpcrel
#libc-68k-os.a: ARCH=68k-os
$(ROOT)/libc-68k-none.a: CFLAGS += -mpcrel
#libc-68k-none.a: ARCH=68k-none

#libc-$(ARCH).a: LIBC_SRC=$(filter-out arch/* $(wildcard **/*.[cs])) $(wildcard arch/$(ARCH)/**/*.[cs])
#libc-$(ARCH).a: LIBC_OBJ=$(patsubst %.c,$(OBJDIR)/%.o, $(patsubst %.s,$(OBJDIR)/%.o, $LIBC_SRC))

#libc-68k-none.a: ARCH = 68k-none
#libc-68k-none.a: OBJDIR = objs/$(ARCH)
#libc-68k-none.a: LIBC_SRCS = $(filter-out arch/*, $(wildcard **/*.[cs])) $(wildcard arch/$(ARCH)/**/*.[cs])
#libc-68k-none.a: LIBC_OBJS = $(patsubst %.c,$(OBJDIR)/%.o, $(patsubst %.s,$(OBJDIR)/%.o, $(LIBC_SRCS)))
#libc-68k-none.a: $(LIBC_OBJS)

ARCH = 68k-os
OBJDIR = objs/$(ARCH)

LIBC_68K_NONE_SRCS = $(wildcard 68k/**/*.[cs]) $(wildcard none/**/*.[cs]) $(wildcard common/**/*.[cs])
LIBC_68K_NONE_OBJS = $(patsubst %.c,%.o, $(patsubst %.s,%.o, $(LIBC_68K_NONE_SRCS)))

LIBC_68K_OS_SRCS = $(wildcard 68k/**/*.[cs]) $(wildcard os/**/*.[cs]) $(wildcard common/**/*.[cs])
LIBC_68K_OS_OBJS = $(patsubst %.c,%.o, $(patsubst %.s,%.o, $(LIBC_68K_OS_SRCS)))

$(ROOT)/libc-68k-none.a: $(LIBC_68K_NONE_OBJS)
$(ROOT)/libc-68k-os.a: $(LIBC_68K_OS_OBJS)


%.a: $^
	$(AR) rc $@ $^
	$(RANLIB) $@

%.o: %.c
	$(shell mkdir -p $(@D))
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -c -o $@ $<

clean:
	find . \( -name "*.o" -or -name "*.a" -or -name "*.bin" -or -name "*.elf" -or -name "*.load" \) -delete -print

