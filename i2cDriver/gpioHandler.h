#ifndef GPIOHANDLER_H
#define GPIOHANDLER_H

#include <linux/types.h>


void gpioHandler_write(uint16_t pinNumber, bool value);

#endif
