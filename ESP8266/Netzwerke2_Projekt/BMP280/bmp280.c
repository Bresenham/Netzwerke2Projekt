/* 
 * Created @ 01.05.2019
 * 
*/
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"

#include "bmp280.h"
#include "bmp280_private.h"

/********************/
/* PRIVATE FUNCTIONS */
/********************/
uint8_t ICACHE_FLASH_ATTR read_SDA(void) {
    return GPIO_INPUT_GET(I2C_MASTER_SDA_GPIO);
}

void ICACHE_FLASH_ATTR set_SDA_high(void) {
    gpio_output_set(1 << I2C_MASTER_SDA_GPIO, 0, 1 << I2C_MASTER_SDA_GPIO, 0);
}

void ICACHE_FLASH_ATTR set_SDA_low(void) {
    gpio_output_set(0, 1 << I2C_MASTER_SDA_GPIO, 1 << I2C_MASTER_SDA_GPIO, 0);
}

void ICACHE_FLASH_ATTR set_SCL_high(void) {
    gpio_output_set(1 << I2C_MASTER_SCL_GPIO, 0, 1 << I2C_MASTER_SCL_GPIO, 0);
}

void ICACHE_FLASH_ATTR set_SCL_low(void) {
    gpio_output_set(0, 1 << I2C_MASTER_SCL_GPIO, 1 << I2C_MASTER_SCL_GPIO, 0);
}

void ICACHE_FLASH_ATTR i2c_master_nack() {
    set_SCL_low();
    os_delay_us(DELAY);
    set_SCL_high();
    os_delay_us(DELAY);
    set_SCL_low();
}

void ICACHE_FLASH_ATTR i2c_start() {
    set_SDA_low();
    os_delay_us(DELAY);
    set_SCL_low();
    os_delay_us(DELAY);
}

void ICACHE_FLASH_ATTR i2c_stop() {
    set_SCL_high();
    os_delay_us(DELAY);
    set_SDA_high();
    os_delay_us(DELAY);
}

void ICACHE_FLASH_ATTR initI2CPorts() {
    GPIO_REG_WRITE(
        GPIO_PIN_ADDR(GPIO_ID_PIN(I2C_MASTER_SDA_GPIO)),
        GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(I2C_MASTER_SDA_GPIO)))
        | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE)); //open drain;

    GPIO_REG_WRITE(GPIO_ENABLE_ADDRESS,
        GPIO_REG_READ(GPIO_ENABLE_ADDRESS) | (1 << I2C_MASTER_SDA_GPIO));

    GPIO_REG_WRITE(
        GPIO_PIN_ADDR(GPIO_ID_PIN(I2C_MASTER_SCL_GPIO)),
        GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(I2C_MASTER_SCL_GPIO)))
        | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE)); //open drain;

    GPIO_REG_WRITE(GPIO_ENABLE_ADDRESS,
        GPIO_REG_READ(GPIO_ENABLE_ADDRESS) | (1 << I2C_MASTER_SCL_GPIO));

    set_SDA_high();
    set_SCL_high();
    os_delay_us(100 * DELAY);
}

uint8_t ICACHE_FLASH_ATTR i2c_read_byte() {
    uint8_t read_byte = 0x00;
    set_SDA_high();
    os_delay_us(DELAY);
    for(int8_t i = 7; i >= 0; i--) {
        os_delay_us(DELAY);
        set_SCL_high();
        os_delay_us(DELAY);
        const uint8_t sda_val = read_SDA();
        if(sda_val == 1)
            read_byte |= (1 << i);
        set_SCL_low();
    }

    return read_byte;
}

bool ICACHE_FLASH_ATTR i2c_send_byte(uint8_t byte) {
    for(int8_t i = 7; i >= 0; i--) {
        if(byte & (1 << i))
            set_SDA_high();
        else
            set_SDA_low();

        os_delay_us(DELAY);
        set_SCL_high();
        os_delay_us(DELAY);
        set_SCL_low();
    }
    set_SDA_high();
    os_delay_us(DELAY);

    /* generate 9th clock for slave to ack */
    set_SCL_high();
    os_delay_us(DELAY);
    const uint8_t slave_ack = read_SDA();
    set_SCL_low();
    set_SDA_low();

    os_delay_us(DELAY);
    
    return slave_ack == 0;
}

uint8_t ICACHE_FLASH_ATTR bmp280GetID(BMP280 *self) {
    uint8_t id = 0x00;

    i2c_start();

    if(i2c_send_byte(ADDRESS_WRITE)) {
        if(i2c_send_byte(REGISTER_ID)){
            /* stop condition */
            set_SCL_high();
            os_delay_us(DELAY);
            set_SDA_high();
            os_delay_us(DELAY);
            i2c_start();
            if(i2c_send_byte(ADDRESS_READ)) {
                id = i2c_read_byte();
                i2c_master_nack();
                os_delay_us(DELAY);
            }
        }
    }

    i2c_stop();
    return id;
}

/********************/
/* PUBLIC FUNCTIONS */
/********************/
void ICACHE_FLASH_ATTR initBMP280(BMP280 *self) {
    initI2CPorts();
    self->getID = &bmp280GetID;
}