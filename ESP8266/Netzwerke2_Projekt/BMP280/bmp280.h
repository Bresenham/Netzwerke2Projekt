/* 
 * Created @ 01.05.2019
 * 
*/
#include "stdint.h"

typedef struct BMP280 {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;


    uint8_t (*getID)();
    int32_t (*getTemperature)(struct BMP280*);
} BMP280;

void ICACHE_FLASH_ATTR initBMP280();