TARGET_NAME := rpi_driver_RTC_DS3231

obj-m += $(TARGET_NAME).o
$(TARGET_NAME)-objs := kernelWrapper.o
$(TARGET_NAME)-objs += mainCycle.o i2cHandler.o rtcInterface.o


#PREFIX = arm-linux-gnueabihf-
PREFIX = aarch64-linux-gnu-
SRC := $(CURDIR)
LINUX_5_15_32 = ~/rpi_kernel/linux/
LINUX_5_15_56_V8 = ~/rpi_kernel/5.15.56-v8+/linux/


all:
	make ARCH=arm64 CROSS_COMPILE=${PREFIX} -C ${LINUX_5_15_56_V8} M=$(SRC) modules

clean:
	make -C ${LINUX_5_15_56_V8} M=$(SRC) clean



# For compilation on the PI

#all:
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

#clean:
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
