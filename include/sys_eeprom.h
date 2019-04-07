/*
 * SYS_EEPROM_SIZE: size of usable eeprom
 * SYS_EEPROM_I2C_DEVICE: i2c-bus
 * SYS_EEPROM_I2C_ADDR: address on the bus
 * SYS_EEPROM_I2C_MEM_ADDR_BITS: number of bits to address a byte in eeprom
 * SYS_EEPROM_MTD_DEVICE: MTD device name of the eeprom
 * SYS_EEPROM_MAX_SIZE: Total size of the eeprom
 * SYS_EEPROM_OFFSET: offset from where the ONIE header starts
 */
#define SYS_EEPROM_SIZE              2048
// #define SYS_EEPROM_I2C_DEVICE        "/dev/i2c-1"
#define SYS_EEPROM_I2C_ADDR          0x50
#define CPLD_EEPROM_I2C_ADDR         0x41

#define SYS_EEPROM_I2C_MEM_ADDR_BITS 16

// char *SYS_EEPROM_I2C_DEVICE = "/dev/i2c-1";
// constexpr auto SYS_EEPROM_OFFSET = 0;
// constexpr auto SYS_EEPROM_SIZE = 256;
// constexpr auto SYS_EEPROM_I2C_MEM_ADDR_BITS = 8;
//#define SYS_EEPROM_MTD_DEVICE        CONFIG_SYS_EEPROM_MTD_DEVICE
//#define SYS_EEPROM_SYSFS_FILE_PATH   CONFIG_SYS_EEPROM_SYSFS_FILE_PATH
#define SYS_EEPROM_MAX_SIZE          32768
#define SYS_EEPROM_OFFSET            0


/*
 * SYS_EEPROM_I2C_WRITE_1B_DELAY: delay number of microseconds after writing 1 byte
 * SYS_EEPROM_I2C_WRITE_2B_DELAY: delay number of microseconds after writing 2 bytes
 * SYS_EEPROM_I2C_WRITE_3B_DELAY: delay number of microseconds after writing 3 bytes
 */
#define SYS_EEPROM_I2C_WRITE_1B_DELAY   5000
#define SYS_EEPROM_I2C_WRITE_2B_DELAY   5000
#define SYS_EEPROM_I2C_WRITE_3B_DELAY   5000

// #if CONFIG_SYS_EEPROM_SIZE == 0
//   #error CONFIG_SYS_EEPROM_SIZE not specified!
// #endif

#ifdef CONFIG_SYS_EEPROM_DEVICE_I2C
  #if CONFIG_SYS_EEPROM_I2C_ADDR == 0
    #error CONFIG_SYS_EEPROM_I2C_ADDR not specified!
  #endif
#endif

#if (SYS_EEPROM_SIZE + SYS_EEPROM_OFFSET > SYS_EEPROM_MAX_SIZE)
  #error SYS_EEPROM_SIZE + SYS_EEPROM_OFFSET is greater than SYS_EEPROM_MAX_SIZE
#endif

int read_sys_eeprom(void *eeprom_data, int offset, int len);
int write_sys_eeprom(void *eeprom_data, int len);
