#include "userSpaceInterface.h"

#include <linux/device.h>
#include <linux/fs.h>
#include "gpioHandler.h"
#include <linux/string.h>


static const char _interfaceClassName[] = "rpi-learning-class";
static const char _interfaceGpioDeviceName[] = "rpi-gpio-module";


static ssize_t userSpaceInterface_show(struct device *dev, struct device_attribute *attr, char *buffer);
static ssize_t userSpaceInterface_store(struct device *dev, struct device_attribute *attr, const char *buffer, size_t bufferLength);


static struct class *_interfaceClass;
struct device *_interfaceDevice;
static dev_t _interfaceDeviceFirst;
static struct device_attribute _interfaceDeviceValueAttr = 
{
    .attr = 
    {
        .name = "value",
        .mode = (S_IWUGO | S_IRUGO)
    },
    .show = userSpaceInterface_show,
    .store = userSpaceInterface_store
};

static bool _initialized = false;


/*
 * @brief Initialize SysFS - create device files, etc.
 */
bool userSpaceInterface_init(void)
{
    if(_initialized == true)
        return _initialized;

    _interfaceClass = class_create(THIS_MODULE, _interfaceClassName);
    if(_interfaceClass == NULL)
    {
        pr_warn("%s: unable to create class %s\r\n", __func__, _interfaceClassName);
        return _initialized;
    }
    
    alloc_chrdev_region(&_interfaceDeviceFirst, 0, 1, _interfaceGpioDeviceName);
    _interfaceDevice = device_create(_interfaceClass, NULL, _interfaceDeviceFirst, NULL, _interfaceGpioDeviceName);
    device_create_file(_interfaceDevice, &_interfaceDeviceValueAttr);

    _initialized = true;
    pr_info("%s: interface created\r\n", __func__);

    return _initialized;
}


/*
 * @brief Deinitialize SysFS - remove device files and directories
 */
void userSpaceInterface_close(void)
{
    if(_initialized == false)
        return;

    if(_interfaceDevice != NULL)
        device_remove_file(_interfaceDevice, &_interfaceDeviceValueAttr);

    if(_interfaceClass != NULL)
        device_destroy(_interfaceClass, _interfaceDeviceFirst);

    unregister_chrdev_region(_interfaceDeviceFirst, 1);
    
    if(_interfaceClass != NULL)
        class_destroy(_interfaceClass);

    _initialized = false;
    pr_info("%s: interface closed\r\n", __func__);

    return;
}


static ssize_t userSpaceInterface_show(struct device *dev, struct device_attribute *attr, char *buffer)
{
    pr_info("%s: read from the interface\r\n", __func__);
    return sprintf(buffer, "Welcome to The Module\r\n");
}


static ssize_t userSpaceInterface_store(struct device *dev, struct device_attribute *attr, const char *buffer, size_t bufferLength)
{
    char inputSymbol;
    
    if(bufferLength == 0)
    {
        pr_info("%s: no data entered\r\n", __func__);
        return bufferLength;
    }

    inputSymbol = buffer[0];

    switch(inputSymbol)
    {
        case '0':
            gpioHandler_write(42, false);
            break;
        
        case '1':
            gpioHandler_write(42, true);
            break;
        
        default:
            pr_info("%s: unrecognized symbol\r\n", __func__);
            break;
    }

    return bufferLength;
}
