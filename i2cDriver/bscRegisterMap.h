#ifndef BSCREGISTERMAP_H
#define BSCREGISTERMAP_H

#define BSCREGISTERMAP_OFFSET0      0x205000
#define BSCREGISTERMAP_OFFSET1      0x804000
#define BSCREGISTERMAP_OFFSET3      0x205600
#define BSCREGISTERMAP_OFFSET4      0x205800
#define BSCREGISTERMAP_OFFSET5      0x205A80
#define BSCREGISTERMAP_OFFSET6      0x205C00

#define BSCREGISTERMAP_MAPSIZE      0x20

#define BSCREGISTERMAP_C        0
#define BSCREGISTERMAP_S        0x04
#define BSCREGISTERMAP_DLEN     0x08
#define BSCREGISTERMAP_A        0x0C
#define BSCREGISTERMAP_FIFO     0x10
#define BSCREGISTERMAP_DIV      0x14
#define BSCREGISTERMAP_DEL      0x18
#define BSCREGISTERMAP_CLKT     0x1C


#define BSCREGISTERMAP_CONTROL_I2CEN    (1 << 15)
#define BSCREGISTERMAP_CONTROL_INTR     (1 << 10)
#define BSCREGISTERMAP_CONTROL_INTT     (1 << 9)
#define BSCREGISTERMAP_CONTROL_INTD     (1 << 8)
#define BSCREGISTERMAP_CONTROL_ST       (1 << 7)
#define BSCREGISTERMAP_CONTROL_CLEAR    (1 << 5)
#define BSCREGISTERMAP_CONTROL_READ     (1 << 0)
#define BSCREGISTERMAP_STATUS_CLKT      (1 << 9)
#define BSCREGISTERMAP_STATUS_ERR       (1 << 8)
#define BSCREGISTERMAP_STATUS_RXF       (1 << 7)
#define BSCREGISTERMAP_STATUS_TXE       (1 << 6)
#define BSCREGISTERMAP_STATUS_RXD       (1 << 5)
#define BSCREGISTERMAP_STATUS_TXD       (1 << 4)
#define BSCREGISTERMAP_STATUS_RXR       (1 << 3)
#define BSCREGISTERMAP_STATUS_TXW       (1 << 2)
#define BSCREGISTERMAP_STATUS_DONE      (1 << 1)
#define BSCREGISTERMAP_STATUS_TA        (1 << 0)

#endif
