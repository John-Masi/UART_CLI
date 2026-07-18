
# Variables 
MCU = atmega2560
F_CPU = 16000000UL
BAUD = 115200
BAUD_2 = 9600

# Requires main.hex to build
all: main.hex

main.elf: main.c
	avr-gcc -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -o main.elf main.c 
 					
main.hex: main.elf 
	avr-objcopy -O ihex main.elf main.hex 

object: 
	avr-objdump -d main.elf

cli:
	screen /dev/ttyACM0 $(9600)

flash: main.hex 
	avrdude -c wiring -p m2560 -P /dev/ttyACM0 -b 115200 -D -U flash:w:main.hex:i

clean:
	rm -f *.elf *.hex
