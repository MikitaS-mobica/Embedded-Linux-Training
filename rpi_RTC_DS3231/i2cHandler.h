#ifndef I2CHANDLER_H
#define I2CHANDLER_H

#include <linux/types.h>
#include <linux/rtc.h>


bool i2cHandler_init(void);
void i2cHandler_clean(void);

int i2cHandler_read_time(struct device *, struct rtc_time *);
int i2cHandler_set_time(struct device *, struct rtc_time *);
int i2cHandler_read_offset(struct device *, long *offset);
int i2cHandler_set_offset(struct device *, long offset);

#endif
