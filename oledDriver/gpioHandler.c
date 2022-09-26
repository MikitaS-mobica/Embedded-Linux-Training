#include "gpioHandler.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>


#define GPIOHANDLER_ONBOARDLEDPIN	42


void gpioHandler_write(uint16_t pinNumber, bool value)
{
	int opRes;

	pr_info("%s: pin %u value %s\r\n", __func__, pinNumber, (value == true) ? "true" : "false");
	
	if(value == true)
		opRes = gpio_direction_output(pinNumber, 1);
	else
		opRes = gpio_direction_output(pinNumber, 0);
	
	if(opRes != 0)
		pr_info("%s: gpio_direction_output operation result is %i\r\n", __func__, opRes);

	return;
}
