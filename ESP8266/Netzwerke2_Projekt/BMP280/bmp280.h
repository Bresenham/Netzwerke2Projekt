/* 
 * Created @ 01.05.2019
 * 
*/
#include "stdint.h"

typedef struct BMP280 {
    uint8_t (*getID)(struct BMP280*);
} BMP280;

void ICACHE_FLASH_ATTR initBMP280(BMP280 *self);