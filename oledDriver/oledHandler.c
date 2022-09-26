#include "oledHandler.h"

#include "linux/module.h"
#include <linux/spi/spi.h>


static int spiProbe(struct spi_device *dev);
static int spiRemove(struct spi_device *dev);


static struct spi_device *_displayDevice = NULL;
static struct spi_board_info _displayInfo = 
{
    .modalias = "rpi-ssd1306-display-ddriver",
    .max_speed_hz = 400000,
    .bus_num = 1,   // SPI_BUS_NUM from the tutorial
    .chip_select = 0,
    .mode = SPI_MODE_0
};


static struct spi_driver oledDisplayDriver = 
{
    .driver = 
    {
        .name = "rpi_ssd1306_spi_driver"
    },
    .probe = spiProbe,
    .remove = spiRemove
};


static int spiProbe(struct spi_device *dev)
{
    return 0;
}


static int spiRemove(struct spi_device *dev)
{
    return 0;
}

/*
static void oledHandler_spiWriteCommand(u8 command)
{
    __iomem void *spiRegisterMap;
    __iomem u32 *spiStatusControlReg;
    __iomem u32 *spiFifoReg;
    __iomem u32 *spiDlenReg;

    u32 controlState;
    
    spiRegisterMap = ioremap(0xfe204000, 0x20);
    spiStatusControlReg = spiRegisterMap;
    spiFifoReg = (void *)(spiRegisterMap + 0x04);
    spiDlenReg = (void *)(spiRegisterMap + 0x0C);

    controlState = ioread32(spiStatusControlReg);

    //iowrite32(0x01, spiDlenReg);
    
    controlState |= 1 << 7; // Transfer Active bit
    iowrite32(controlState, spiStatusControlReg);   // Start transfer
    iowrite32(command, spiFifoReg);
    while(((ioread32(spiStatusControlReg) >> 16) & 0x01) == 0); // While DONE == 0
    controlState &= ~(1 << 7);
    iowrite32(controlState, spiStatusControlReg);   // Finish transfer

    iounmap(spiRegisterMap);

    return;
}
*/


static void oledHandler_spi0PinsConfigure(void)
{
    __iomem void *gpioRegisterMap;
    __iomem u32 *gpioFSel0;
    __iomem u32 *gpioFSel1;
    __iomem u32 *gpioClr0;
    u32 temp;

    gpioRegisterMap = ioremap(0xfe200000, 0x90);
    
    gpioFSel0 = (void *)(gpioRegisterMap);
    gpioFSel1 = (void *)(gpioRegisterMap + 0x04);
    gpioClr0 = (void *)(gpioRegisterMap + 0x28);

    temp = ioread32(gpioFSel0);
    temp &= ~(0x07 << 24);
    temp |= 0x01 << 24;
    iowrite32(temp, gpioFSel0);

    iowrite32(1 << 8, gpioClr0);

    temp = ioread32(gpioFSel1);
    temp &= ~(0x07);
    temp &= ~(0x07 << 3);
    iowrite32(temp, gpioFSel1);

    iounmap(gpioRegisterMap);

    return;
}

void oledHadler_init(void)
{
    struct spi_master *masterBus;
    int status;
    struct spi_transfer testTransfer;
    static u8 tx_buf[1];
    static u8 rx_buf[1];

/*
    __iomem void *spiRegisterMap;
    __iomem u32 *spiStatusControlReg;
    __iomem u32 *spiFifoReg;
    __iomem u32 *spiClkReg;
    __iomem u32 *spiDlenReg;

    u32 temp;
*/

    if(memcmp(&oledDisplayDriver, &oledDisplayDriver, sizeof(oledDisplayDriver)) == 0); // Just to get rid of the unused variable warning
    //if(memcmp(&_displayInfo, &_displayInfo, sizeof(_displayInfo)) == 0); // Just to get rid of the unused variable warning

    
    oledHandler_spi0PinsConfigure();
    
    masterBus = spi_busnum_to_master(_displayInfo.bus_num);
    if(masterBus == NULL)
    {
        pr_err("%s %s: no master bus found\r\n", THIS_MODULE->name, __func__);
        return;
    }

    _displayDevice = spi_new_device(masterBus, &_displayInfo);
    if(_displayDevice == NULL)
    {
        pr_err("%s %s: device creation failed\r\n", THIS_MODULE->name, __func__);
        return;
    }

    _displayDevice->bits_per_word = 8;
    status = spi_setup(_displayDevice);
    if(status != 0)
    {
        pr_err("%s %s: spi setup failed\r\n", THIS_MODULE->name, __func__);
        spi_unregister_device(_displayDevice);
        return;
    }


    testTransfer.len = 1;
    testTransfer.tx_buf = tx_buf;
    testTransfer.rx_buf = rx_buf;

    tx_buf[0] = 0xAF;
    spi_sync_transfer(_displayDevice, &testTransfer, 1);
    
    tx_buf[0] = 0xA4;
    spi_sync_transfer(_displayDevice, &testTransfer, 1);


/*
    // Let's try to write something baremetal style
    spiRegisterMap = ioremap(0xfe204000, 0x20);

    if(spiRegisterMap == NULL)
    {
        pr_err("%s %s: ioremap to 0x%x failed\r\n", THIS_MODULE->name, __func__, 0xfe204000);
        return;
    }


    // Configure SPI controller
    spiStatusControlReg = spiRegisterMap;
    spiFifoReg = (void *)(spiRegisterMap + 0x04);
    spiClkReg = (void *)(spiRegisterMap + 0x08);
    spiDlenReg = (void *)(spiRegisterMap + 0x0C);

    iowrite32(0xF800, spiClkReg); // Set clock divider to 65536

    //temp = 1 << 2; // SPI settings: CPHA = 1
    iowrite32(temp, spiStatusControlReg);   // This is valid configuration
    iounmap(spiRegisterMap);

    oledHandler_spiWriteCommand(0xAF);  // Turn the display ON
    oledHandler_spiWriteCommand(0xA4);  // Light up the entire display
*/
    return;
}


void oledHandler_clean(void)
{
    if(_displayDevice != NULL)
        spi_unregister_device(_displayDevice);

    return;
}


void oledHandler_helloWorld(void)
{
    return;
}
