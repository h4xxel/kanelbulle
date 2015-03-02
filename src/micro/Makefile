TOOLCHAIN	= arm-none-eabi-
CC		= $(TOOLCHAIN)gcc
AS		= $(TOOLCHAIN)as
LD		= $(TOOLCHAIN)ld
OBJCOPY		= $(TOOLCHAIN)objcopy

LPC21ISP	= lpc21isp

RM		= rm -f
CP		= cp
MV		= mv
MKDIR		= mkdir

CFILES		= $(wildcard *.c)
ASFILES		= $(wildcard *.s)
OBJFILES	= $(CFILES:.c=.o)
OBJFILES	+=$(ASFILES:.s=.o)
LINKSCRIPT	= link.ld
BINFILE		= kanelbulle

TERM-DEVICE	= /dev/ttyUSB0
UART-BAUDRATE	= 115200
TERM-BAUDRATE	= 115200
#UART-BAUDRATE	= 230400
#CHIP-XTAL	= 60000
CHIP-XTAL	= 48000

ASFLAGS		= -mthumb -mcpu=cortex-m0
CFLAGS		= -O3 -Wall $(ASFLAGS) -D__NEWLIB__ -DUART_BAUD_RATE=$(UART-BAUDRATE) -DSYSTEM_CLOCK=$(CHIP-XTAL)000
CFLAGS		+= -DDEBUG
LDFLAGS		= $(CFLAGS) -nostartfiles

.PHONY: all clean install install-term extract-code

all: $(OBJFILES) system/startup.o
	@echo " [ LD ] $(BINFILE).elf"
	@$(CC) $(LDFLAGS) system/startup.c $(OBJFILES) -o $(BINFILE).elf -T $(LINKSCRIPT)

extract-code: all
	@echo " [OBCP] $(BINFILE).hex"
	@$(OBJCOPY) -O ihex $(BINFILE).elf $(BINFILE).hex

install-term: extract-code
	@echo " [INST] Flashing micro and opening terminal with baudrate $(TERM-BAUDRATE)"
	@$(LPC21ISP) -control -term $(BINFILE).hex $(TERM-DEVICE) $(TERM-BAUDRATE) $(CHIP-XTAL)

install: extract-code
	@echo " [INST] Flashing micro with baudrate $(TERM-BAUDRATE)"
	@$(LPC21ISP) $(BINFILE).hex $(TERM-DEVICE) $(TERM-BAUDRATE) $(CHIP-XTAL)

clean:
	@echo " [ RM ] $(OBJFILES) system/startup.o $(BINFILE).hex $(BINFILE).elf"
	@$(RM) $(OBJFILES) system/startup.o $(BINFILE).hex $(BINFILE).elf

system/startup.o:
	@echo " [ CC ] $@"
	@$(CC) $(CFLAGS) -c -o $@ $*.c

%.o: %.c %.h
	@echo " [ CC ] $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	@echo " [ AS ] $@"
	@$(AS) $(ASFLAGS) -o $@ $<
