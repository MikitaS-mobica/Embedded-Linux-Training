#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "mainCycle.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Screenshifter");
MODULE_DESCRIPTION("Hehe, Linux Kernel Module, please don't crash");


int init_module()
{
	return mainCycle_init();
}


void cleanup_module(void)
{
	mainCycle_cleanup();
	return;
}

