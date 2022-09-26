# Embedded-Linux-Training
Embedded Linux training modules and user space programs

These modules are written for the Raspberry PI 4B with 64-bit Raspbian OS.

Quick modules description:
- gpioBlink - kernel module to change state of the ACT LED. It provides SysFS interface for user-space access.
- userSpace_gpioBlink - user-space program to change state of the ACT LED, uses gpioBlink module.
- i2cDriver - adapter driver for the I2C bus. Doesn't work correctly. Transfers are interrupted after the address byte.
- rpi_RTC_DS3231 - I2C Real-Time Clock device driver, uses I2C adapter bus provided with the Raspbian OS. Correctly reads RTC values, writes to the RTC registers and synchronizes with the Linux system clock (due to implementation of the standard RTC interface). Warning: can't be removed properly from the kernel. Removing will lead to errors and crashes (dev/rtc0 just refuses to delete itself after module cleanup).
- oledDriver - SPI OLED Display device driver. Doesn't work. No SPI transfers occur at all, no matter which SPI controller is used. Bus master, provided with the Raspbian OS, doesn't work, writing to the SPI controller registers directly doesn't improve situation even a little bit.
