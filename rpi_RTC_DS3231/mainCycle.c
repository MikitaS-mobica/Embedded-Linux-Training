#include "mainCycle.h"

#include <linux/module.h>
#include "rtcInterface.h"


int mainCycle_init(void)
{
	if(rtcInterface_init() == false)
		pr_err("%s %s: rtcInterface_init failed\r\n", THIS_MODULE->name, __func__);

	return 0;
}


void mainCycle_cleanup()
{
	rtcInterface_clean();

	return;
}
