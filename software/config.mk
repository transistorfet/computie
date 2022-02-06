
# Configuration

BOARD = k30
#BORAD = 68k

# End of Configuration

ifeq ($(BOARD), k30)
#CFLAGS_ARCH = -m68030 -msoft-float
#ASFLAGS_ARCH = -m68030 --defsym __mc68010__=1
CFLAGS_ARCH = -m68010 -DBOARD_K30
ASFLAGS_ARCH = -m68010 --defsym __mc68010__=1
else
CFLAGS_ARCH = -m68010
ASFLAGS_ARCH = -m68010 --defsym __mc68010__=1
endif

BUILD = build
AS = m68k-linux-gnu-as
AR = m68k-linux-gnu-ar
CC = m68k-linux-gnu-gcc
OBJDUMP = m68k-linux-gnu-objdump
OBJCOPY = m68k-linux-gnu-objcopy
RANLIB = m68k-linux-gnu-ranlib
INCLUDE = $(ROOT)/include
ASFLAGS = $(ASFLAGS_ARCH)
CFLAGS = -I$(INCLUDE) $(CFLAGS_ARCH) -nostartfiles -nostdlib -fno-zero-initialized-in-bss -g -Os
LDFLAGS = -Wl,--build-id=none

# reduce the elf output size by reducing the alignment
CFLAGS += -z max-page-size=0x10

