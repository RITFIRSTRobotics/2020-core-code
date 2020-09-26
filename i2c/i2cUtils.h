//
// Created by Alex Kneipp on 10/12/19.
//

#ifndef INC_2020_CORE_CODE_I2CUTILS_H
#define INC_2020_CORE_CODE_I2CUTILS_H

#define DEFAULT_NUM_SLAVES  5
#define BYTES_PER_WORD 2

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>

typedef struct i2cBus_s
{
    int fd;
    uint8_t currentSlave;
} * i2cBus;

i2cBus i2cBus_create(char* busName);

int i2cBus_initSlave(i2cBus bus, uint8_t slaveAddr);

int8_t i2cBus_readByte(i2cBus bus, uint8_t addr, uint8_t reg);
int16_t i2cBus_readWord(i2cBus bus, uint8_t addr, uint8_t reg);
int i2cBus_readBytes(i2cBus bus, uint8_t addr, int8_t* buf, int n);

int8_t i2cBus_writeByte(i2cBus bus, uint8_t addr, uint8_t reg, int8_t byte);
int16_t i2cBus_writeWord (i2cBus bus, uint8_t addr, uint8_t reg, int16_t word);
int i2cBus_writeBytes(i2cBus bus, uint8_t addr, int8_t* buf, int n);

void i2cBus_destroy(i2cBus);

#endif //INC_2020_CORE_CODE_I2CUTILS_H
