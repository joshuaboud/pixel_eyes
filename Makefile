TARGET = dist/pixel_eyes
SIM_TARGET = dist/simulate

F_CPU = 8000000UL
DEVICE = atmega32u4

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
PROGRAMMER = dfu-programmer $(DEVICE)
SIZE = avr-size --format=avr --mcu=$(DEVICE)

CFLAGS = -g2 -I/usr/avr/include -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -std=gnu11
CFLAGS += -Os -ffunction-sections -fdata-sections -fpack-struct -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions  
CFLAGS += -Wall -Wno-pointer-to-int-cast

# LDFLAGS = -mmcu=$(DEVICE) -Wl,--relax,--section-start=.text=0,-Map=main.map
LDFLAGS = -Wl,-gc-sections -Wl,-relax -Wl,-Map=$(TARGET).map

SOURCE_FILES := $(shell find src/ -name *.c)
OBJECT_FILES := $(patsubst src/%.c, build/%.o, $(SOURCE_FILES))
HEADER_FILES := $(shell find src/ -name *.h)

SIM_SOURCE_FILES := $(shell find src/ simulate/ -name *.c -not -path "src/Light_WS2812/*")
SIM_OBJECT_FILES := $(patsubst src/%.c, build/%.o, $(SIM_SOURCE_FILES))
SIM_OBJECT_FILES := $(patsubst simulate/%.c, build/simulate/%.o, $(SIM_OBJECT_FILES))

all: $(TARGET).hex

test:
	$(PROGRAMMER) get ID1

build/simulate/%.o: simulate/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJECT_FILES)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(OBJECT_FILES) -Wall -o $@ $(LDFLAGS)

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -R .eeprom -R .fuse -R .lock -R .signature -R .bootloader -O ihex $< $@
	$(SIZE) $<
	$(OBJDUMP) -d -S $< > $(TARGET).lss

flash: $(TARGET).hex
	$(PROGRAMMER) erase --debug 5 --force
	$(PROGRAMMER) flash --debug 5 $(TARGET).hex --suppress-bootloader-mem

run:
	$(PROGRAMMER) reset

clean:
	rm -rf dist/ build/

simulate: set_sim_vars
simulate: $(SIM_TARGET)

set_sim_vars:
	$(eval CC = gcc)
	$(eval CFLAGS = -Wall -std=gnu11 -Os -Isimulate -DSIMULATE -g)

$(SIM_TARGET): $(SIM_OBJECT_FILES)
	@mkdir -p dist
	$(CC) $(SIM_OBJECT_FILES) -Wall $(LIBS) -lpthread -o $@

edit_frames:
	./frames/frames.py ./src/frames.c
