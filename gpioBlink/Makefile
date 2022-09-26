BUILD_DIR = build/

_TARGET_NAME := gpioTest
TARGET_NAME := $(patsubst %, $(BUILD_DIR)%, $(_TARGET_NAME))

obj-m += testModule.o

testModule-objs := kernelWrapper.o
testModule-objs += mainCycle.o gpioHandler.o userSpaceInterface.o


PREFIX = arm-linux-gnueabihf-
SRC := $(CURDIR)
LINUX_5_15_32 = ~/rpi_kernel/linux/


all:
	make ARCH=arm CROSS_COMPILE=${PREFIX} -C ${LINUX_5_15_32} M=$(SRC) modules

clean:
	make -C ${LINUX_5_15_32} M=$(SRC) clean
