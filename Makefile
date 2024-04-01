# The MCU model, see https://www.nongnu.org/avr-libc/user-manual/using_tools.html for list
MCU=attiny24

# Set to TYPE_ATMEGA48 or TYPE_ATTINYx4
TYPE=TYPE_ATTINYx4

# Cable type for avrdude
CABLE=arduino

# Extra options for avrdude
AVRDUDE_EXTRA=-P /dev/ttyACM0

# Clock speed used (after divider), mostly for delay functions
CLK=12000000UL
# Name of project output file
PROJ=usbadapter
# List of source files
SOURCES=main.c usbdrv/usbdrv.c usbdrv/usbdrvasm.S usbdrv/oddebug.c

all: $(PROJ).hex

# Generate executable
$(PROJ).elf:
	avr-gcc -o $(PROJ).elf -flto -O2 -mmcu=$(MCU) -D$(TYPE)=1 -Iusbdrv -I. -DF_CPU=$(CLK) $(SOURCES)

# Create Intel hex format for programming
$(PROJ).hex: $(PROJ).elf
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature $(PROJ).elf $(PROJ).hex

clean:
	rm -f *.hex *.elf *.o

# Install flashes the code
install:
	avrdude -c $(CABLE) -p $(MCU) $(AVRDUDE_EXTRA) -U flash:w:$(PROJ).hex


