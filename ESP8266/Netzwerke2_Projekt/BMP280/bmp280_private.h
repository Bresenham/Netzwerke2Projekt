/* 
 * Created @ 01.05.2019
 * 
*/

#define I2C_MASTER_SDA_GPIO 4
#define I2C_MASTER_SCL_GPIO 5
#define I2C_MASTER_SDA_FUNC FUNC_GPIO4
#define I2C_MASTER_SCL_FUNC FUNC_GPIO5

#define DELAY               1000

#define ADDRESS_WRITE       0xEE
#define ADDRESS_READ        0xEF

#define REGISTER_ID         0xD0

extern uint8_t ICACHE_FLASH_ATTR bmp280GetID(BMP280 *self);