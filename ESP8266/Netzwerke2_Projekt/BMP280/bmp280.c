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

void ICACHE_FLASH_ATTR i2c_master_ack() {
    set_SDA_low();
    os_delay_us(DELAY);
    set_SCL_high();
    os_delay_us(DELAY);
    set_SCL_low();
    os_delay_us(DELAY);
    set_SDA_high();
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

void ICACHE_FLASH_ATTR init_i2c_ports() {
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

void ICACHE_FLASH_ATTR get_reg_content_continous(uint8_t reg_start_addr, uint8_t amount, uint8_t *storage) {
    i2c_start();

    if(i2c_send_byte(ADDRESS_WRITE)) {
        if(i2c_send_byte(reg_start_addr)){
            /* stop condition */
            set_SCL_high();
            os_delay_us(DELAY);
            set_SDA_high();
            os_delay_us(DELAY);
            i2c_start();
            if(i2c_send_byte(ADDRESS_READ)) {
                for(uint8_t i = 0; i < amount; i++) {
                    const bool is_last_reg = i == amount - 1;

                    storage[i] = i2c_read_byte();

                    if(is_last_reg)
                        i2c_master_nack();
                    else
                        i2c_master_ack();
                    os_delay_us(DELAY);                        
                }
            }
        }
    }

    i2c_stop();
}

uint8_t ICACHE_FLASH_ATTR get_reg_content(uint8_t reg_addr) {
    uint8_t single_reg_data[1];
    get_reg_content_continous(reg_addr, 1, single_reg_data);
    return single_reg_data[0];
}

void ICACHE_FLASH_ATTR write_reg_content(uint8_t reg_addr, uint8_t reg_content) {
    i2c_start();

    if(i2c_send_byte(ADDRESS_WRITE)) {
        if(i2c_send_byte(reg_addr)) {
            if(i2c_send_byte(reg_content)) {

            }
        }
    }

    i2c_stop();
}

int32_t calculate_temp_from_raw_value(int16_t dig_T1, uint16_t dig_T2, uint16_t dig_T3, uint32_t value) {
    int32_t part1 = (value >> 3) - (dig_T1 << 1);
    int32_t var1  = (part1 * dig_T2) >> 11;

    int32_t part2 = (value >> 4) - dig_T1;
    int32_t var2  = (((part2 * part2) >> 12) * dig_T3 ) >> 14;

    int32_t t_fine = var1 + var2;
    
    return (t_fine * 5 + 128) >> 8;
}

void load_trimming_values(BMP280 *self) {
    const uint8_t dig_T1_LSB = get_reg_content(REGISTER_DIG_T1_LSB);
    const uint8_t dig_T1_MSB = get_reg_content(REGISTER_DIG_T1_MSB);
    const uint16_t dig_T1 = (dig_T1_MSB << 8) | dig_T1_LSB;

    const uint8_t dig_T2_LSB = get_reg_content(REGISTER_DIG_T2_LSB);
    const uint8_t dig_T2_MSB = get_reg_content(REGISTER_DIG_T2_MSB);
    const int16_t dig_T2 = (int16_t)((dig_T2_MSB << 8) | dig_T2_LSB);

    const uint8_t dig_T3_LSB = get_reg_content(REGISTER_DIG_T3_LSB);
    const uint8_t dig_T3_MSB = get_reg_content(REGISTER_DIG_T3_MSB);
    const int16_t dig_T3 = (int16_t)((dig_T3_MSB << 8) | dig_T3_LSB);

    self->dig_T1 = dig_T1;
    self->dig_T2 = dig_T2;
    self->dig_T3 = dig_T3;
}

uint8_t ICACHE_FLASH_ATTR bmp280GetID() {
    return get_reg_content(REGISTER_ID);
}

int32_t ICACHE_FLASH_ATTR bmp280GetTemperature(BMP280 *self) {
    uint8_t t_raw[3];
    get_reg_content_continous(REGISTER_TEMP_MSB, 3, t_raw);

    const uint32_t value = (uint32_t)(((t_raw[0] << 16) | (t_raw[1] << 8) | t_raw[2]) >> 4);

    return calculate_temp_from_raw_value(self->dig_T1, self->dig_T2, self->dig_T3, value);
}

/********************/
/* PUBLIC FUNCTIONS */
/********************/
void ICACHE_FLASH_ATTR initBMP280(BMP280 *self) {
    init_i2c_ports();
    write_reg_content(REGISTER_CTRL_MEAS, CTRL_MEAS_TEMP_OVERSAMPLING | CTRL_MEAS_PRESSURE_SKIP | CTRL_MEAS_NORMAL_MODE);
    write_reg_content(REGISTER_CONFIG, CONFIG_STANDBY_1000MS);

    load_trimming_values(self);

    self->getID = &bmp280GetID;
    self->getTemperature = &bmp280GetTemperature;
}