#ifndef DS3231_H
#define DS3231_H

// Register map
#define DS3231_REG_SECOND           0x00
#define DS3231_REG_MINUTE           0x01
#define DS3231_REG_HOUR             0x02
#define DS3231_REG_WEEKDAY          0x03
#define DS3231_REG_MONTHDAY         0x04
#define DS3231_REG_MONTH_CENTURY    0x05
#define DS3231_REG_YEAR             0x06
#define DS3231_REG_ALARM_1_SECOND   0x07
#define DS3231_REG_ALARM_1_MINUTE   0x08
#define DS3231_REG_ALARM_1_HOUR     0x09
#define DS3231_REG_ALARM_1_DAY      0x0A
#define DS3231_REG_ALARM_2_MINUTE   0x0B
#define DS3231_REG_ALARM_2_HOUR     0x0C
#define DS3231_REG_ALARM_2_DAY      0x0D
#define DS3231_REG_CONTROL          0x0E
#define DS3231_REG_CONTROL_STATUS   0x0f
#define DS3231_REG_AGINGOFFSET      0x10
#define DS3231_REG_TEMP_MSB         0x11
#define DS3231_REG_TEMP_LSB         0x12

#define DS3231_CURRENT_TIME_REGS_AMOUNT 7

#define DS3231_STARTYEAR    1900

#define DS3231_regToSeconds(reg)    (((reg >> 4) * 10) + (reg & 0x0F))
#define DS3231_regToMinutes(reg)    (((reg >> 4) * 10) + (reg & 0x0F))
#define DS3231_regToHours(reg)      ((((reg >> 4) & 0x01) * 10) + (reg & 0x0F))
#define DS3231_regToMonthDay(reg)   ((((reg >> 4) & 0x03) * 10) + (reg & 0x0F))
#define DS3231_regToMonth(reg)      ((((reg >> 4) & 0x01) * 10) + (reg & 0x0F))
#define DS3231_regToWeekday(reg)    (reg & 0x07)

// monthReg[7] = century
#define DS3231_regToYear(monthReg, yearReg)     \
    ((100 * (monthReg >> 7)) \
    + (10 * (yearReg >> 4)) + (yearReg & 0x0F))


#define DS3231_secondsToReg(seconds)        (((seconds / 10) << 4) | (seconds % 10))
#define DS3231_minutesToReg(minutes)        (((minutes / 10) << 4) | (minutes % 10))
#define DS3231_hoursToReg(hours)            (((hours / 10) << 4) | (hours % 10))
#define DS3231_weekdayToReg(weekday)        (weekday & 0x07)
#define DS3231_monthdayToReg(monthday)      (((monthday / 10) << 4) | (monthday % 10))
#define DS3231_monthYearToReg(month, year)  (((year / 100) << 7) | (((month / 10) & 0x01) << 4) | (month % 10))
#define DS3231_yearToReg(year)              ((((year / 10) % 10) << 4) | (year % 10))

#endif
