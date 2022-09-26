#include "mainCycle.h"

#include "gpioHandler.h"
#include <linux/module.h>
#include "userSpaceInterface.h"

//#include "i2cHandler.h"
#include "oledHandler.h"


int mainCycle_init(void)
{
	pr_info("%s %s: started\r\n", THIS_MODULE->name, __func__);
/*
	if(userSpaceInterface_init() != true)
	{
		pr_info("%s: initialization failed\r\n", __func__);
		mainCycle_cleanup();
	}
	
	pr_info("GPIO module is ready\r\n");
*/
	oledHadler_init();

	return 0;
}


void mainCycle_cleanup()
{
	oledHandler_clean();
	//userSpaceInterface_close();
	pr_info("%s %s: finished\r\n", THIS_MODULE->name, __func__);

	return;
}
