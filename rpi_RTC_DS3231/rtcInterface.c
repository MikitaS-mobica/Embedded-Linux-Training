#include "rtcInterface.h"

#include <linux/module.h>
#include <linux/rtc.h>
#include "i2cHandler.h"


#define TEST_CLASS_NAME "rpi-learning-class"
#define TEST_SLAVE_NAME "DS3231_RTC"


static dev_t deviceNumber;     // Both Major and Minor numbers combined
static struct class *rtcInterface_deviceClass;
static struct device *rtcInterface_device;
static struct rtc_class_ops rtcInterface_rtcOps =
{
    .read_time = i2cHandler_read_time,
    .set_time = i2cHandler_set_time,
    .read_offset = i2cHandler_read_offset,
    .set_offset = i2cHandler_set_offset
};

static bool _initialized = false;


/*
 *
 */
bool rtcInterface_init(void)
{
    int status;

    _initialized = false;
    if(i2cHandler_init() == false)
        return false;

    // Create char device
    rtcInterface_deviceClass = class_create(THIS_MODULE, TEST_CLASS_NAME);
    status = alloc_chrdev_region(&deviceNumber, 2, 1, TEST_SLAVE_NAME);
    if(status != 0)
    {
        pr_err("%s %s: alloc_chrdev_region failed\r\n", THIS_MODULE->name, __func__);
    }
    rtcInterface_device = device_create(rtcInterface_deviceClass, NULL, deviceNumber, NULL, TEST_SLAVE_NAME);

    // Create RTC device, it will be removed automatically (probably, I hope)
    devm_rtc_device_register(rtcInterface_device, TEST_SLAVE_NAME, &rtcInterface_rtcOps, THIS_MODULE);

    _initialized = true;
    return true;
}


/*
 *
 */
void rtcInterface_clean(void)
{
    i2cHandler_clean();

    if(_initialized == false)
        return;

    // Remove char device
    //device_release_driver(rtcInterface_device);       // Makes nothing, not even a funny sound
    device_destroy(rtcInterface_deviceClass, deviceNumber);
    unregister_chrdev_region(deviceNumber, 1);
    class_destroy(rtcInterface_deviceClass);

    return;
}
