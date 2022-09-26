#include "mainCycle.h"

#include "gpioHandler.h"
#include <linux/module.h>
#include "userSpaceInterface.h"

#include "i2cHandler.h"


int mainCycle_init(void)
{
	pr_info("GPIO module initialization\r\n");
/*
	if(userSpaceInterface_init() != true)
	{
		pr_info("%s: initialization failed\r\n", __func__);
		mainCycle_cleanup();
	}
	
	pr_info("GPIO module is ready\r\n");
*/
	i2cHandler_init();
	i2cHandler_write();

	return 0;
}


void mainCycle_cleanup()
{
	i2cHandler_clean();
	//userSpaceInterface_close();
	pr_info("GPIO module cleaned up\r\n");

	return;
}
