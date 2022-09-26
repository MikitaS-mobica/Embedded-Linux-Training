#include "mainCycle.h"

#include "gpioHandler.h"
#include <linux/module.h>
#include "userSpaceInterface.h"


int mainCycle_init(void)
{
	pr_info("GPIO module initialization\r\n");

	if(userSpaceInterface_init() != true)
	{
		pr_info("%s: initialization failed\r\n", __func__);
		mainCycle_cleanup();
	}
	
	pr_info("GPIO module is ready\r\n");
	return 0;
}


void mainCycle_cleanup()
{
	userSpaceInterface_close();
	pr_info("GPIO module cleaned up\r\n");

	return;
}
