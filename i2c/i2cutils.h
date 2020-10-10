//
// Created by Alex Kneipp on 10/12/19.
//

#ifndef INC_2020_CORE_CODE_I2CUTILS_H
#define INC_2020_CORE_CODE_I2CUTILS_H

// allow C++ to parse this
#ifdef __cplusplus
extern "C" {
#endif

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

/**
 * Opens a new I2C bus according to the OS's name for the bus, usually something like '/dev/i2c-n'
 * on *nix systems.
 * @param busName
 * 	The name of the I2C bus to create.
 * @return
 *  An I2C bus struct corresponding to \p busName
 */
i2cBus i2cBus_create(char* busName);

/**
 * Setup the I2C bus to talk to a specific slave.
 * @param bus
 * 	The bus to talk on.
 * @param slaveAddr
 * 	The read address of the slave.
 * @return 0 on success, non-zero otherwise.
 */
int i2cBus_initSlave(i2cBus bus, uint8_t slaveAddr);

/**
 * Read a byte from a bus slave
 * @param bus
 * 	The bus to talk on.
 * @param addr
 * 	The address of the slave to talk to
 * @param reg
 * 	The regsister of the slave to read from
 * @return
 * 	The value read from the slave, or -1 on failure.
 */
int8_t i2cBus_readByte(i2cBus bus, uint8_t addr, uint8_t reg);
/**
 * Read a 16 bit word from a bus slave
 * @param bus
 * 	The bus to talk on.
 * @param addr
 * 	The address of the slave to talk to
 * @param reg
 * 	The regsister of the slave to read from
 */
int16_t i2cBus_readWord(i2cBus bus, uint8_t addr, uint8_t reg);
/**
 * Read a multiple bytes from a bus slave.
 * @param bus
 * 	The bus to talk on.
 * @param addr
 * 	The address of the slave to talk to
 * @param buf
 * 	Pre-allocated buffer to read values into.
 * @param n
 * 	The number of bytes to read.
 * @return
 * 	The number of bytes read, or -1 if we fail to talk to the slave.
 */
int i2cBus_readBytes(i2cBus bus, uint8_t addr, int8_t* buf, int n);

/**
 * Write a byte to a slave.
 * @param bus
 * 	The bus to talk on.
 * @param addr
 * 	The address of the slave to talk to
 * @param reg
 * 	The register to write to.
 * @param byte
 * 	The byte to write
 * @return
 * 	0 on success, -1 otherwise.
 */
int8_t i2cBus_writeByte(i2cBus bus, uint8_t addr, uint8_t reg, int8_t byte);
/**
 * Write a 16-bit wordk to a slave.
 * @param bus
 * 	The bus to talk on.
 * @param addr
 * 	The address of the slave to talk to
 * @param reg
 * 	The register to write to.
 * @param word
 * 	The word to write
 * @return
 * 	0 on success, -1 otherwise.
 */
int16_t i2cBus_writeWord (i2cBus bus, uint8_t addr, uint8_t reg, int16_t word);
/**
 * Write multiple bytes to a slave.
 * @param bus
 * 	The bus to talk on
 * @param addr
 * 	The address of the slave to talk to
 * @param buf
 * 	Buffer of bytes to write.
 * @param n
 * 	The number of bytes in \p buf
 * @return The number of bytes written, or -1 if we cannot talk to the slave.
 */
int i2cBus_writeBytes(i2cBus bus, uint8_t addr, int8_t* buf, int n);

/**
 * Free the resources used for the i2cbus.
 * @param i2cBus
 * 	The bus to destroy.
 */
void i2cBus_destroy(i2cBus);

#ifdef __cplusplus
}
#endif

#endif //INC_2020_CORE_CODE_I2CUTILS_H
