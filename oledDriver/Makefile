TARGET_NAME := rpi_oled_driver

obj-m += $(TARGET_NAME).o
$(TARGET_NAME)-objs := kernelWrapper.o
$(TARGET_NAME)-objs += mainCycle.o gpioHandler.o userSpaceInterface.o oledHandler.o


#PREFIX = arm-linux-gnueabihf-
PREFIX = aarch64-linux-gnu-
SRC := $(CURDIR)
LINUX_5_15_32 = ~/rpi_kernel/linux/
LINUX_5_15_56_V8 = ~/rpi_kernel/5.15.56-v8+/linux/


all:
	make ARCH=arm64 CROSS_COMPILE=${PREFIX} -C ${LINUX_5_15_56_V8} M=$(SRC) modules

clean:
	make -C ${LINUX_5_15_56_V8} M=$(SRC) clean
