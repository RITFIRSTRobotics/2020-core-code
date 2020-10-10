//
// Created by Alex Kneipp on 10/12/19.
//
#include "i2cUtils.h"
#include <errno.h>

static int slaveIsOnBus(i2cBus bus, uint8_t slaveAddr)
{
    return bus->currentSlave == slaveAddr;
}

i2cBus i2cBus_create(char* busName)
{
    int fd = 0;
    i2cBus rval = (i2cBus)malloc(sizeof(struct i2cBus_s));
    if ((fd = open(busName, O_RDWR)) < 0)
    {
        free(rval);
        return NULL;
    }
    rval->fd = fd;
    rval->currentSlave = 0;
    return rval;
}

int i2cBus_initSlave(i2cBus bus, uint8_t slaveAddr)
{
    errno = 0;
    if(ioctl(bus->fd, I2C_SLAVE, slaveAddr) < 0)
    {
        //Failed to acquire bus access or talk to slave - log this
        return errno;
    }
    bus->currentSlave = slaveAddr;
    return 0;
}

int8_t i2cBus_readByte(i2cBus bus, uint8_t addr, uint8_t reg)
{
	//TODO set errno instead of returning an error value
    if(i2cBus_writeBytes(bus, addr, (int8_t*)&reg, 1) != 1)
    {
        //TODO error handling
        return -1;
    }
    int8_t buf;
    if(i2cBus_readBytes(bus, addr, &buf, 1) != 1)
    {
        //TODO error handling
        return -1;
    }
    return buf;
}

int16_t i2cBus_readWord(i2cBus bus, uint8_t addr, uint8_t reg)
{
    if(i2cBus_writeBytes(bus, addr, (int8_t*)&reg, 1) != 1)
    {
        //TODO error handling
        return -1;
    }
    int8_t buf[BYTES_PER_WORD] = {0};
    int rval = 0;
    if (i2cBus_readBytes(bus, addr, buf, BYTES_PER_WORD) != BYTES_PER_WORD)
    {
        //AHH error handling
        //TODO
        rval = -1;
    }
    else
    {
        rval = buf[0] << 8 | buf[1];
    }
    return rval;
}

int i2cBus_readBytes(i2cBus bus, uint8_t addr, int8_t* buf, int n)
{
    if(!slaveIsOnBus(bus, addr))
    {
        if(i2cBus_initSlave(bus, addr) != 0)
        {
            //TODO log an error
            return -1;
        }
    }
    return read(bus->fd, buf, n);
}

int8_t i2cBus_writeByte(i2cBus bus, uint8_t addr, uint8_t reg, int8_t byte)
{
    if(i2cBus_writeBytes(bus, addr, (int8_t*)&reg, 1) != 1)
    {
        //TODO error handling
        return -1;
    }
    return i2cBus_writeBytes(bus, addr, &byte, 1) == 0 ? 0: -1;
}

int16_t i2cBus_writeWord (i2cBus bus, uint8_t addr, uint8_t reg, int16_t word)
{
    if(i2cBus_writeBytes(bus, addr, (int8_t*)&reg, 1) != 1)
    {
        //TODO error handling
        return -1;
    }
    return i2cBus_writeBytes(bus, addr, (int8_t*)&word, BYTES_PER_WORD) == BYTES_PER_WORD? 0 : -1;
}

int i2cBus_writeBytes(i2cBus bus, uint8_t addr, int8_t* buf, int n)
{
    if(!slaveIsOnBus(bus, addr))
    {
        if(i2cBus_initSlave(bus, addr) != 0)
        {
            //TODO log an error
            return -1;
        }
    }
    return write(bus->fd, buf, n);
}

void i2cBus_destroy(i2cBus bus) {
    errno = 0;
    if(close(bus->fd) < 0)
    {
        //Other errno values:
        //EINTR - we were interrupted
        //EIO   - and I/O error occurred
        //ENOSPC, EDQUOT -  On NFS, these errors are not normally reported against the first write which exceeds the
        //available storage space, but instead against a subsequent write(2), fsync(2), or close(2)

        //bus->fd does not refer to an open file descriptor or an IO error occurred, stop retrying
        if(errno == EBADF)
        { /* TODO log this error */ }
    }
    free(bus);
}
