####INSTRUCTIONS####
#Copy this file to it's dedicated location (right next to the source file)
#Rename it to 'makefile' or 'MAKEFILE'
#Enter the name of the sourcefile
#use it and save time.

# Project source, will handle .S & .c files
SOURCE          = wdt_pwm_methods.c
# MSP430 MCU to compile for
CPU             = msp430g2553
# Optimization level
OPTIMIZATION    = -O0
# Extra variables
CFLAGS          = -mmcu=$(CPU) $(OPTIMIZATION) -Wall -g
# Libemb library link flags
LIBEMB          = -lshell -lconio -lserial
# Get base name so we can create .elf file
NAME            = $(basename $(SOURCE))
# Compiler to be used
CC              = msp430-gcc

# Build and link executable
$(NAME).elf: $(SOURCE)
ifeq (,$(findstring libemb,$(shell cat $(SOURCE))))
	$(CC) $(CFLAGS) -o $@ $(SOURCE)
else
	$(CC) $(CFLAGS) -o $@ $(SOURCE) $(LIBEMB)
endif

# Flash to board with mspdebug
flash: $(NAME).elf
	mspdebug rf2500 "prog $(NAME).elf"

# Erase board
erase:
	mspdebug rf2500 erase

# Clean up temporary files
clean:
	rm -f *.elf

# Remote debug board
debug: $(NAME).elf
	( mspdebug rf2500 "gdb" 1>/dev/null & ); msp430-gdb $(NAME).elf -ex "target remote :2000"
