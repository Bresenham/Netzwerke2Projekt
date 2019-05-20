#include "user_interface.h"
#include "ets_sys.h"
#include "osapi.h"

#include "../MQTT/MQTT.h"
#include "../BMP280/bmp280.h"
#include "../WIFI/wifi.h"

#if ((SPI_FLASH_SIZE_MAP == 0) || (SPI_FLASH_SIZE_MAP == 1))
#error "The flash map is not supported"
#elif (SPI_FLASH_SIZE_MAP == 2)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0xfb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0xfc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0xfd000
#elif (SPI_FLASH_SIZE_MAP == 3)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x1fd000
#elif (SPI_FLASH_SIZE_MAP == 4)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x81000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x3fd000
#elif (SPI_FLASH_SIZE_MAP == 5)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x1fd000
#elif (SPI_FLASH_SIZE_MAP == 6)
#define SYSTEM_PARTITION_OTA_SIZE							0x6A000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x3fd000
#else
#error "The flash map is not supported"
#endif

static const partition_item_t at_partition_table[] = {
    { SYSTEM_PARTITION_RF_CAL,  						SYSTEM_PARTITION_RF_CAL_ADDR, 						0x1000},
    { SYSTEM_PARTITION_PHY_DATA, 						SYSTEM_PARTITION_PHY_DATA_ADDR, 					0x1000},
    { SYSTEM_PARTITION_SYSTEM_PARAMETER, 				SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR, 			0x3000},
};

void ICACHE_FLASH_ATTR user_pre_init(void) {
    if(!system_partition_table_regist(at_partition_table, sizeof(at_partition_table)/sizeof(at_partition_table[0]),SPI_FLASH_SIZE_MAP)) {
		os_printf("system_partition_table_regist fail\r\n");
		while(1);
	}
}

BMP280 bmp280;
Wifi wifi;
LOCAL os_timer_t mqtt_publish_timer;

void ICACHE_FLASH_ATTR mqtt_publish_data() {
    uint8_t data[8];

    const int32_t temp = bmp280.getTemperature(&bmp280);
    const uint8_t msb = (temp >> 24) & 0xFF;
    const uint8_t lsb = (temp >> 16) & 0xFF;
    const uint8_t xlsb = (temp >> 8) & 0xFF;
    const uint8_t xxlsb = temp & 0xFF;

    os_printf("Read temperature %d\r\n", temp);

    /*
     TODO: MQTT-Header aufbauen, Laenge berechnen, Payload hinzufuegen...
    */
   MQTT mqtt;
   wifi.publishData(&wifi, &mqtt);
}

void ICACHE_FLASH_ATTR user_init(void) {
    system_update_cpu_freq(160);
    while(system_get_cpu_freq() != 160);

    initBMP280(&bmp280);
    const uint8_t bmp280_id = bmp280.getID();

    if(bmp280_id == BMP280_EXPECTED_ID) {
        os_printf("Found BMP280\r\n");
        initWifi(&wifi);

        /* Veroeffentliche alle 1s neuen Temperaturwert */
        os_timer_disarm(&mqtt_publish_timer);
        os_timer_setfn(&mqtt_publish_timer, (os_timer_func_t*)mqtt_publish_data, NULL);
        os_timer_arm(&mqtt_publish_timer, 1000, true);
    } else {
        os_printf("BMP280-ID GOT %d, expected %d - END.\r\n", bmp280_id, BMP280_EXPECTED_ID);
    }
}
