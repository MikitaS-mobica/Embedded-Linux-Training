#include "i2cHandler.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/string.h>
//#include <linux/errno.h>

#include <linux/io.h>
#include <linux/ioport.h>


#define TEST_ADAPTER_NAME "RPI-driver-I2C"
#define TEST_SLAVE_NAME "PCF8574"


static s32 testSmbusXfer(struct i2c_adapter *adapter, u16 addr, u16 flags, char readWrite, u8 command, int size, union i2c_smbus_data *data);
static s32 testMasterXfer(struct i2c_adapter *adapter, struct i2c_msg *messages, int amount);
static u32 testFunctionality(struct i2c_adapter *adapter);


// Bus driver environment
static struct i2c_algorithm testAlgorithm = 
{
    //.smbus_xfer = testSmbusXfer,
    .master_xfer = testMasterXfer,
    .functionality = testFunctionality
};
static struct i2c_adapter testSourceAdapter = 
{
    .owner = THIS_MODULE,
    .class = I2C_CLASS_HWMON,
    .algo = &testAlgorithm,
    .name = TEST_ADAPTER_NAME,
    .nr = 2
};

// Device driver environment
static struct i2c_adapter *testAdapter;
static struct i2c_board_info testBoard = {I2C_BOARD_INFO(TEST_SLAVE_NAME, 0x20)};
static struct i2c_client *testClient;
static struct i2c_device_id testDeviceId[] =
{
    {TEST_SLAVE_NAME, 0},
    {}  // Null-terminated structure
};
MODULE_DEVICE_TABLE(i2c, testDeviceId);
static struct i2c_driver testDriver =
{
    .driver =
    {
        .name = TEST_SLAVE_NAME,
        .owner = THIS_MODULE
    },
    .id_table = testDeviceId
};


static s32 testSmbusXfer(struct i2c_adapter *adapter, u16 addr, u16 flags, char readWrite, u8 command, int size, union i2c_smbus_data *data)
{
    pr_info("%s: hello there\r\n", __func__);
    
    return 0;
}


static s32 testMasterXfer(struct i2c_adapter *adapter, struct i2c_msg *messages, int amount)
{
    int i;

    for(i = 0; i < amount; i++)
    {
        static int j;
        static struct i2c_msg *tempMessage;
        tempMessage = &(messages[i]);

        pr_info("%s: [Count: %d] [Addr = 0x%x] [Len = %d] [Data] = ", __func__, i, tempMessage->addr, tempMessage->len);
        for(j = 0; j < tempMessage->len; j++)
            pr_cont("[0x%02x ]", tempMessage->buf[j]);
    }

    return 0;
}


static u32 testFunctionality(struct i2c_adapter *adapter)
{
    u32 result;

    result = I2C_FUNC_I2C;
    /*
    result |= I2C_FUNC_SMBUS_BYTE;
    result |= I2C_FUNC_SMBUS_BYTE_DATA;
    result |= I2C_FUNC_SMBUS_WORD_DATA;
    result |= I2C_FUNC_SMBUS_BLOCK_DATA;
    */

    return result;
}


    static struct resource *ioMemResource;
    static char memoryRegionName[128]; 

void i2cHandler_init(void)
{
    // Adapter creation required here
/*
    i2c_add_numbered_adapter(&testSourceAdapter);

    // Adapter is created - driver can be used just like with RPI I2C module

    testAdapter = i2c_get_adapter(2);
    if(testAdapter == NULL)
    {
        pr_info("%s: test adapter %u\r\n", __func__, testAdapter);
        return;
    }

    //testClient = i2c_new_dummy_device(testAdapter, 0x20);
    testClient = i2c_new_client_device(testAdapter, &testBoard);
    if(testClient == NULL)
    {
        pr_info("%s: test client %u\r\n", __func__, testClient);
        return;
    }

    i2c_add_driver(&testDriver);

    i2c_put_adapter(testAdapter);
*/

    // GPIO Base address test

    //const u32 rpiPeriphBase = 0x7e000000; // Datasheet address - doesn't work
    //const u32 rpiPeriphBase = 0x3e000000; // Some address just floated up in memory - no reaction to r/w
    //const u32 rpiPeriphBase = 0xfe000000; // Address from kernel sources - no r/w access
    //const u32 rpiPeriphBase = 0x20000000; // Probably address from examples - doesn't work
    const u32 rpiPeriphBase = 0xfe000000;
    //const u32 rpiGpioOffset = 0x200000;
    const u32 rpiGpioOffset = 0x20C010;

    const void *rpiGpioAddress = rpiPeriphBase + rpiGpioOffset;

    __iomem u32 *ioMemory;

    sprintf(memoryRegionName, "GPIO_registers@0x%x", rpiGpioAddress);

    //check_mem_region(rpiGpioAddress, 0x40);
    ioMemResource = request_mem_region(rpiGpioAddress, 0x40, memoryRegionName);
    //if(ioMemResource != NULL) // Null result
    {
        ioMemory = ioremap(rpiGpioAddress, 0x40 * 8);
        //ioMemory = memremap(rpiGpioAddress, 0x40);
        //if(ioMemory != NULL)
        {
            /*
            u32 *gpioBaseAddr;
            gpioBaseAddr = (u32 *)ioMemory;

            u32 *gpioActDirection = (u32 *)((void *)gpioBaseAddr + 0x10);
            const u32 *const gpioActValue = (u32 *)((void *)gpioBaseAddr + 0x38);   // Read only
            u32 *gpioActSet = (u32 *)((void *)gpioBaseAddr + 0x20);
            u32 *gpioActClear = (u32 *)((void *)gpioBaseAddr + 0x2C);

            
            gpioActDirection = ioremap(gpioActDirection, 32);
            gpioActSet = ioremap(gpioActSet, 32);
            gpioActClear = ioremap(gpioActClear, 32);
            */

            pr_info("%s: rpiGpioAddress = 0x%x\r\n", __func__, rpiGpioAddress);
            pr_info("%s: ioMemResource = 0x%x\r\n", __func__, ioMemResource);
            pr_info("%s: ioMemory = 0x%x\r\n", __func__, ioMemory);
            
            pr_info("%s: i2cControl = 0x%x\r\n", __func__, ioMemory);
            //pr_info("%s: *i2cControl = 0x%x\r\n", __func__, ioread32(ioMemory));

            /*
            pr_info("%s: gpioActDirection = 0x%x\r\n", __func__, gpioActDirection);
            pr_info("%s: gpioActSet = 0x%x\r\n", __func__, gpioActSet);
            pr_info("%s: gpioActClear = 0x%x\r\n", __func__, gpioActClear);
            */

            /*
            pr_info("%s: gpioActDirection = 0x%x\r\n", __func__, *gpioActDirection);
            pr_info("%s: gpioActValue = 0x%x\r\n", __func__, *gpioActValue);
            
            *gpioActDirection &= ~(0x7 << 6);   // Clear pin 42 function
            *gpioActDirection |= 1 << 6;    // Make pin 42 output
            
            *gpioActClear |= 1 << 10;
            *gpioActSet |= 1 << 10;
            //*/
            
            pr_info("IO unmapping now\r\n");
            if(ioMemory != NULL)
                iounmap(ioMemory);
                //memunmap(ioMemory);
            pr_info("IO unmapped\r\n");
        }
        //if(ioMemResource != NULL)
            //release_mem_region(ioMemResource->start, 0x40);
    }

    return;
}


void i2cHandler_clean(void)
{
    if(ioMemResource != NULL)
        release_mem_region(ioMemResource->start, 0x40);

    /*
    if(testClient != NULL)
        i2c_unregister_device(testClient);
    i2c_del_driver(&testDriver);

    i2c_del_adapter(&testSourceAdapter);
*/
    return;
}


void i2cHandler_read(void)
{
    return;
}


void i2cHandler_write(void)
{
    char testBuffer = 0xFF;
    int testCount = 1;

    /*
    if(testClient != NULL)
    {
        pr_info("%s: i2c_master_send\r\n", __func__);
        i2c_master_send(testClient, &testBuffer, testCount);
    }
    */

    /*
    struct i2c_msg testMsg;
    char testBuffer = 0xFF;

    testMsg.addr = 0x40;
    testMsg.buf = &testBuffer;
    testMsg.len = sizeof(testBuffer);
    //testMsg.flags;

    if(testAdapter != NULL)
        i2c_transfer(testAdapter, &testMsg, 1);
    */

    return;
}
