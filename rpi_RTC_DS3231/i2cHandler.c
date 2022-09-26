#include "i2cHandler.h"

#include "DS3231.h"
#include <linux/device.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>


#define _I2C_BUS_NUMBER     1
#define TEST_SLAVE_NAME "DS3231_RTC"


static bool i2cHandler_rtcTimeNormalize(struct rtc_time *time);


// Device driver environment
static struct i2c_adapter *testAdapter;
static struct i2c_board_info testBoard = {I2C_BOARD_INFO(TEST_SLAVE_NAME, 0x68)};
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


bool i2cHandler_init(void)
{
    testAdapter = i2c_get_adapter(_I2C_BUS_NUMBER);
    if(testAdapter == NULL)
    {
        pr_info("%s: i2c_get_adapter failed\r\n", __func__);
        return false;
    }

    testClient = i2c_new_client_device(testAdapter, &testBoard);
    if(testClient == NULL)
    {
        pr_info("%s: i2c_new_client_device failed\r\n", __func__);
        return false;
    }

    i2c_add_driver(&testDriver);

    i2c_put_adapter(testAdapter);

    return true;
}


void i2cHandler_clean(void)
{
    if(testClient != NULL)
        i2c_unregister_device(testClient);
    i2c_del_driver(&testDriver);

    return;
}


int i2cHandler_read_time(struct device *dev, struct rtc_time *time)
{
    const char startRegisterNumber = 0;
    char readBuffer[DS3231_CURRENT_TIME_REGS_AMOUNT];

    if(time == NULL)
    {
        pr_err("%s %s: time == NULL\r\n", THIS_MODULE->name, __func__);
        return -1;
    }
    
    i2c_master_send(testClient, &startRegisterNumber, sizeof(startRegisterNumber));
    i2c_master_recv(testClient, readBuffer, sizeof(readBuffer));

    time->tm_sec = DS3231_regToSeconds(readBuffer[DS3231_REG_SECOND]);
    time->tm_min = DS3231_regToMinutes(readBuffer[DS3231_REG_MINUTE]);
    time->tm_hour = DS3231_regToHours(readBuffer[DS3231_REG_HOUR]);
    time->tm_mday = DS3231_regToMonthDay(readBuffer[DS3231_REG_MONTHDAY]);
    time->tm_mon = DS3231_regToMonth(readBuffer[DS3231_REG_MONTH_CENTURY]) - 1; // In Linux January = 0 for some reason
    time->tm_year = DS3231_regToYear(readBuffer[DS3231_REG_MONTH_CENTURY], readBuffer[DS3231_REG_YEAR]);
    
    return 0;
}


int i2cHandler_set_time(struct device *dev, struct rtc_time *time)
{
    char writeBuffer[DS3231_CURRENT_TIME_REGS_AMOUNT + 1] = {0};
    struct rtc_time newTime;
    
    memcpy(&newTime, time, sizeof(newTime));
    i2cHandler_rtcTimeNormalize(&newTime);

    writeBuffer[DS3231_REG_SECOND + 1] = DS3231_secondsToReg(time->tm_sec);
    writeBuffer[DS3231_REG_MINUTE + 1] = DS3231_minutesToReg(time->tm_min);
    writeBuffer[DS3231_REG_HOUR + 1] = DS3231_hoursToReg(time->tm_hour);
    writeBuffer[DS3231_REG_WEEKDAY + 1] = DS3231_weekdayToReg(time->tm_wday);
    writeBuffer[DS3231_REG_MONTHDAY + 1] = DS3231_monthdayToReg(time->tm_mday);
    writeBuffer[DS3231_REG_MONTH_CENTURY + 1] = DS3231_monthYearToReg(time->tm_mon + 1, time->tm_year);  // In Linux January = 0 for some reason
    writeBuffer[DS3231_REG_YEAR + 1] = DS3231_yearToReg(time->tm_year);

    i2c_master_send(testClient, writeBuffer, sizeof(writeBuffer));

    return 0;
}


int i2cHandler_read_offset(struct device *dev, long *offset)
{
    pr_info("%s %s: hello\r\n", THIS_MODULE->name, __func__);
    return 0;
}


int i2cHandler_set_offset(struct device *dev, long offset)
{
    pr_info("%s %s: hello\r\n", THIS_MODULE->name, __func__);
    return 0;
}


static bool i2cHandler_rtcMonthNormalize(int *day, int *month, int *year, int daysInThisMonth)
{
    if(day == NULL)
        return false;
    if(month == NULL)
        return false;

    if(*day > daysInThisMonth)
    {
        (*month)++;     // Without brackets pointer is incremented somehow instead of value
        *day -= daysInThisMonth;
        return false;
    }

    *year += *month / 12;
    *month %= 12;

    return true;
}


static inline bool i2cHandler_isLeapYear(int year)
{
    if((year % 400) == 0)
        return true;
    if((year % 100) == 0)
        return false;
    if((year % 4) == 0)     // year & 0x03 == 0
        return true;
    
    return false;
}


static bool i2cHandler_rtcTimeNormalize(struct rtc_time *time)
{
    int daysInAMonth;
    int initialDay = 1;
    const int initialDaysInAMonth = 2;
    bool monthIsNormalized = false;

    if(time == NULL)
        return false;

    time->tm_year += DS3231_STARTYEAR;  // This is required for correct leap year calculation

    time->tm_min += time->tm_sec / 60;
    time->tm_hour += time->tm_min / 60;
    time->tm_mday += time->tm_hour / 24;
    
    // Normalize months number first
    i2cHandler_rtcMonthNormalize(&initialDay, &time->tm_mon, &time->tm_year, initialDaysInAMonth);

    // Normalize days
    while(monthIsNormalized == false)
    {
        switch(time->tm_mon)
        {
            case 0:
                daysInAMonth = 31;
            break;

            case 1:
                if(i2cHandler_isLeapYear(time->tm_year) == true)
                    daysInAMonth = 29;
                else
                    daysInAMonth = 28;
            break;

            case 2:
                daysInAMonth = 31;
            break;

            case 3:
                daysInAMonth = 30;
            break;

            case 4:
                daysInAMonth = 31;
            break;

            case 5:
                daysInAMonth = 30;
            break;

            case 6 ... 7:
                daysInAMonth = 31;
            break;

            case 8:
                daysInAMonth = 30;
            break;

            case 9:
                daysInAMonth = 31;
            break;

            case 10:
                daysInAMonth = 30;
            break;

            default:
                daysInAMonth = 31;
            break;
        }
            
        monthIsNormalized = i2cHandler_rtcMonthNormalize(&time->tm_mday, &time->tm_mon, &time->tm_year, daysInAMonth);
    }
    
    time->tm_year -= DS3231_STARTYEAR;

    return true;
}
