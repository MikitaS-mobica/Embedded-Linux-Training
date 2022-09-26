#ifndef I2CHANDLER_H
#define I2CHANDLER_H

#include <linux/types.h>


void i2cHandler_init(void);
void i2cHandler_read(void);
void i2cHandler_write(void);
void i2cHandler_clean(void);

#endif
