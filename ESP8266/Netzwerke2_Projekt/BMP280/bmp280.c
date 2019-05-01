/* 
 * Created @ 01.05.2019
 * 
*/

#include "bmp280.h"
#include "bmp280_private.h"

/********************/
/* PRIVATE FUNCTIONS */
/********************/
uint8_t bmp280GetID(BMP280 *self) {
    return 0x00;
}

/********************/
/* PUBLIC FUNCTIONS */
/********************/
void initBMP280(BMP280 *self) {
    self->getID = &bmp280GetID;
}