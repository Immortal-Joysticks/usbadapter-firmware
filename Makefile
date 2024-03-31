# The MCU model, see https://www.nongnu.org/avr-libc/user-manual/using_tools.html for list
MCU=atmega328p

# Set to TYPE_ATMEGA48 or TYPE_ATTINYx4
TYPE=TYPE_ATMEGA48

# Cable type for avrdude
CABLE=arduino

# Extra options for avrdude
AVRDUDE_EXTRA=-P /dev/ttyACM0

# Clock speed used (after divider), mostly for delay functions
CLK=16000000
# Name of project output file
PROJ=usbadapter
# List of source files
SOURCES=main.c usbdrv/usbdrv.c usbdrv/usbdrvasm.S usbdrv/oddebug.c

# List of object files generated from source files
OBJ=$(SOURCES:.c=.o)

all: $(PROJ).hex

# Generated object files
.c.o:
	avr-gcc -w -Os -DF_CPU=$(CLK) -D$(TYPE)=1 -mmcu=$(MCU) -I. -Iusbdrv -c -o $@ $<

# Generate executable
$(PROJ).elf: $(OBJ)
	avr-gcc -w -mmcu=$(MCU) $(OBJ) -DF_CPU=$(CLK) -D$(TYPE)=1 -I. -Iusbdrv -o $(PROJ).elf

# Create Intel hex format for programming
$(PROJ).hex: $(PROJ).elf
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature $(PROJ).elf $(PROJ).hex

clean:
	rm -f *.hex *.elf *.o

# Install flashes the code
install:
	avrdude -c $(CABLE) -p $(MCU) $(AVRDUDE_EXTRA) -U flash:w:$(PROJ).hex


