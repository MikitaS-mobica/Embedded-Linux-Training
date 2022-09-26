#include "i2cHandler.h"

#include "bscRegisterMap.h"
#include "gpioRegisterMap.h"
#include <linux/device.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include "raspberryPIPeripherals.h"
#include <linux/string.h>


#define TEST_ADAPTER_NAME "RPI-driver-I2C"
#define TEST_SLAVE_NAME "PCF8574"


static s32 testMasterXfer(struct i2c_adapter *adapter, struct i2c_msg *messages, int amount);
static u32 testFunctionality(struct i2c_adapter *adapter);
static bool i2cHandler_gpioI2CFunctionSet(void);
static bool i2cHandler_bscInit(void);


static struct resource *_ioMemResource;
static char memoryRegionName[128];


// Bus driver environment
static struct i2c_algorithm testAlgorithm = 
{
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
    {}  // Null-terminated structure array
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

static u32 _bscControlRegister;


static s32 testMasterXfer(struct i2c_adapter *adapter, struct i2c_msg *messages, int amount)
{
    int i;
    __iomem void *bscAddress;
    __iomem u32 *regControl;
    __iomem u32 *regAddr;
    __iomem u32 *regDLen;
    __iomem u32 *regFifo;
    __iomem u32 *regStatus;

    // Get pointer to the BSC1
    bscAddress = ioremap((void *)(RASPBERRYPIPERIPHERALS_BASEOFFSET + BSCREGISTERMAP_OFFSET3), BSCREGISTERMAP_MAPSIZE);
    if(bscAddress == NULL)
    {
        pr_err("%s %s: unable to ioremap\r\n", THIS_MODULE->name, __func__);
        return -1;
    }

    // Get pointer to the required registers
    regControl = (__iomem u32 *)((void *)bscAddress + BSCREGISTERMAP_C);
    regAddr = (__iomem u32 *)((void *)bscAddress + BSCREGISTERMAP_A);
    regDLen = (__iomem u32 *)((void *)bscAddress + BSCREGISTERMAP_DLEN);
    regFifo = (__iomem u32 *)((void *)bscAddress + BSCREGISTERMAP_FIFO);
    regStatus = (__iomem u32 *)((void *)bscAddress + BSCREGISTERMAP_S);

    // Messages transfer
    for(i = 0; i < amount; i++)
    {
        //static int j;
        static struct i2c_msg *tempMessage;
        tempMessage = &(messages[i]);

        pr_info("%s %s: transmit\r\n", THIS_MODULE->name, __func__);
        if(tempMessage->len == 1)
        {
            pr_info("%s %s: tempMessage->addr = %x\r\n", THIS_MODULE->name, __func__, tempMessage->addr);
            iowrite32(tempMessage->addr, regAddr);
            pr_info("%s %s: BSC status: 0x%x\r\n", THIS_MODULE->name, __func__, ioread32(regStatus));
            pr_info("%s %s: tempMessage->buf[0] = %x\r\n", THIS_MODULE->name, __func__, tempMessage->buf[0]);
            iowrite32(tempMessage->buf[0], regFifo);
            iowrite32(tempMessage->buf[0], regFifo);
            pr_info("%s %s: tempMessage->len = %x\r\n", THIS_MODULE->name, __func__, tempMessage->len);
            iowrite32(tempMessage->len + 1, regDLen);
            
            
            pr_info("%s %s: BSC status: 0x%x\r\n", THIS_MODULE->name, __func__, ioread32(regStatus));
            pr_info("%s %s: *regControl = %x\r\n", THIS_MODULE->name, __func__, ioread32(regControl));
            iowrite32(_bscControlRegister | BSCREGISTERMAP_CONTROL_ST, regControl);
            pr_info("%s %s: BSC status: 0x%x\r\n", THIS_MODULE->name, __func__, ioread32(regStatus));
            pr_info("%s %s: after start *regControl = %x\r\n", THIS_MODULE->name, __func__, ioread32(regControl));
        }

        /*
        pr_info("%s: [Count: %d] [Addr = 0x%x] [Len = %d] [Data] = ", __func__, i, tempMessage->addr, tempMessage->len);
        for(j = 0; j < tempMessage->len; j++)
            pr_cont("[0x%02x ]", tempMessage->buf[j]);
        */
    }
    
    // Unmap @bscAddress
    iounmap(bscAddress);

    return 0;
}


static u32 testFunctionality(struct i2c_adapter *adapter)
{
    return I2C_FUNC_I2C;
}


static bool i2cHandler_gpioI2CFunctionSet(void)
{
    __iomem void *gpioAddress;
    __iomem u32 *gpioFunctionSelect0;
    __iomem u32 *gpioPuppdn0;
    u32 tempRegister;

    // Get pointer to the GPIO
    gpioAddress = ioremap((void *)(RASPBERRYPIPERIPHERALS_BASEOFFSET + GPIOREGISTERMAP_OFFSET), GPIOREGISTERMAP_MAPSIZE);
    if(gpioAddress == NULL)
    {
        pr_err("%s %s: unable to ioremap\r\n", THIS_MODULE->name, __func__);
        return false;
    }

    // Get pointer to the required registers
    gpioFunctionSelect0 = (__iomem u32 *)((void *)gpioAddress + GPIOREGISTERMAP_GPFSEL0);
    gpioPuppdn0 = (__iomem u32 *)((void *)gpioAddress + GPIOREGISTERMAP_GPIO_PUP_PDN_CNTRL_REG0);

    // Set GPIO2 and GPIO3 to alternative function 0 - GPFSEL0 register
    tempRegister = ioread32(gpioFunctionSelect0);
    tempRegister &= ~(0x3f << 6);   // Clear bits 11-6 (GPIO 2-3 function)
    tempRegister |= 0x12 << 6;      // Set alternative function 5 for GPIO 2-3
    iowrite32(tempRegister, gpioFunctionSelect0);
    pr_info("%s %s: control register = %x\r\n", THIS_MODULE->name, __func__, tempRegister);

    // Set pullup for gpio2 and gpio3
    tempRegister = ioread32(gpioPuppdn0);
    tempRegister &= ~(0x0f << 4);   // Clear bits 7-4
    //tempRegister |= 0x0a << 4;      // Set pullup for GPIO 2-3
    iowrite32(tempRegister, gpioPuppdn0);

    // Unmap @gpioAddress
    iounmap(gpioAddress);

    return true;
}


static bool i2cHandler_bscInit(void)
{
    __iomem void *bscAddress;
    __iomem u32 *regControl;
    __iomem u32 *regClkDiv;

    // Get pointer to the BSC1
    bscAddress = ioremap((void *)(RASPBERRYPIPERIPHERALS_BASEOFFSET + BSCREGISTERMAP_OFFSET3), BSCREGISTERMAP_MAPSIZE);
    if(bscAddress == NULL)
    {
        pr_err("%s %s: unable to ioremap\r\n", THIS_MODULE->name, __func__);
        return false;
    }

    // Get pointer to the required registers
    regControl = (__iomem u32 *)((void *)bscAddress + BSCREGISTERMAP_C);
    regClkDiv = (__iomem u32 *)((void *)bscAddress + BSCREGISTERMAP_DIV);

    // Write configuration into the registers
    _bscControlRegister = BSCREGISTERMAP_CONTROL_I2CEN;
    //_bscControlRegister |= BSCREGISTERMAP_CONTROL_INTR;
    
    //iowrite32(0x07D0, regClkDiv);   // Divide by 2000, 100kHz transfer rate, confirmed by logic analyzer
    iowrite32(0xfffe, regClkDiv);   // 
    iowrite32(_bscControlRegister, regControl);

    _bscControlRegister |= BSCREGISTERMAP_CONTROL_CLEAR;
    iowrite32(_bscControlRegister, regControl);
    
    // Unmap @bscAddress
    iounmap(bscAddress);

    return true;
}


void i2cHandler_init(void)
{
    __iomem u32 *ioMemory;
    void *bsc1Address = (void *)(RASPBERRYPIPERIPHERALS_BASEOFFSET + BSCREGISTERMAP_OFFSET3);

    // Claim BSC1 IO memory for this driver

    sprintf(memoryRegionName, "RPI_Learning_I2C_bus@0x%x", (unsigned int)bsc1Address);
    _ioMemResource = request_mem_region(bsc1Address, BSCREGISTERMAP_MAPSIZE, memoryRegionName);
    if(_ioMemResource == NULL) // Null result
    {
        pr_err("%s %s: unable to claim IO memory region\r\n", THIS_MODULE->name, __func__);
        return;
    }

    // Initialize BSC (I2C controller)

    if(i2cHandler_gpioI2CFunctionSet() == false)
    {
        pr_err("%s %s: i2cHandler_gpioI2CFunctionSet failed", THIS_MODULE->name, __func__);
        return;
    }

    if(i2cHandler_bscInit() == false)
    {
        pr_err("%s %s: i2cHandler_bscInit failed", THIS_MODULE->name, __func__);
        return;
    }


    // Adapter creation required here
    i2c_add_numbered_adapter(&testSourceAdapter);

    // Adapter is created - driver can be used just like with RPI I2C module

    testAdapter = i2c_get_adapter(2);
    if(testAdapter == NULL)
    {
        pr_info("%s: test adapter %u\r\n", __func__, testAdapter);
        return;
    }

    testClient = i2c_new_client_device(testAdapter, &testBoard);
    if(testClient == NULL)
    {
        pr_info("%s: test client %u\r\n", __func__, testClient);
        return;
    }

    i2c_add_driver(&testDriver);

    i2c_put_adapter(testAdapter);

    return;
}


void i2cHandler_clean(void)
{
    if(_ioMemResource != NULL)
        release_mem_region(_ioMemResource->start, BSCREGISTERMAP_MAPSIZE);

    if(testClient != NULL)
        i2c_unregister_device(testClient);
    i2c_del_driver(&testDriver);

    i2c_del_adapter(&testSourceAdapter);

    return;
}


void i2cHandler_read(void)
{
    return;
}


void i2cHandler_write(void)
{
    //*
    char testBuffer = 0xEF;
    int testCount = 1;

    if(testClient != NULL)
    {
        pr_info("%s: i2c_master_send\r\n", __func__);
        i2c_master_send(testClient, &testBuffer, testCount);
    }
    //*/

    return;
}
