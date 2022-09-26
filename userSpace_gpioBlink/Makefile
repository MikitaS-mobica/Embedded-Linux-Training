CC = ~/Downloads/testInst/crosstool-ng-1.25.0/build/armv8-rpi4-linux-gnueabihf/buildtools/bin/armv8-rpi4-linux-gnueabihf-gcc
BUILD_DIR = build/

_TARGET_NAME := userSpace_gpioBlink
TARGET_NAME := $(patsubst %, $(BUILD_DIR)%, $(_TARGET_NAME))

DEPS := 

_OBJS := main.o
OBJS = $(patsubst %, $(BUILD_DIR)%, $(_OBJS))


$(BUILD_DIR)%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

$(TARGET_NAME): $(OBJS)
	$(CC) -o $@ $^

clean:
	rm $(TARGET_NAME) $(OBJS)
