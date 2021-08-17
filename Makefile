TARGET = dist/pixel_eyes

F_CPU = 16000000UL
DEVICE = atmega32u4
PROGRAMMER = avrisp2

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRDUDE = avrdude -c $(PROGRAMMER) -p $(DEVICE)
SIZE = avr-size --format=avr --mcu=$(DEVICE)

CFLAGS = -g2 -I/usr/avr/include -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -std=gnu11
CFLAGS += -Os -ffunction-sections -fdata-sections -fpack-struct -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions  
CFLAGS += -Wall -Wno-pointer-to-int-cast

LDFLAGS = -mmcu=$(DEVICE) -Wl,--relax,--section-start=.text=0,-Map=main.map

SOURCE_FILES := $(shell find src/ -name *.c)
OBJECT_FILES := $(patsubst src/%.c, build/%.o, $(SOURCE_FILES))
HEADER_FILES := $(shell find src/ -name *.h)

all: $(TARGET).hex

test:
	$(AVRDUDE) -v

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJECT_FILES)
	@mkdir -p $(dir $@)
	$(CC) $(OBJECT_FILES) -Wall $(LIBS) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	$(SIZE) $<
	$(OBJDUMP) -d -S $< > $(TARGET).lss

flash: $(TARGET).hex
	$(AVRDUDE) -U flash:w:$(TARGET).hex:i

clean:
	rm -rf dist/ build/
