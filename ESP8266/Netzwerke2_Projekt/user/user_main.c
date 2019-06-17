#include "user_interface.h"
#include "ets_sys.h"
#include "osapi.h"

#include "../BMP280/bmp280.h"
#include "../WIFI/wifi.h"
#include "../MQTT/mqtt.h"

LOCAL BMP280 bmp280;
LOCAL Wifi wifi;
LOCAL os_timer_t mqtt_publish_timer;

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

void ICACHE_FLASH_ATTR mqtt_publish_data() {
    const int32_t temp = bmp280.getTemperature(&bmp280);

    os_printf("Read temperature %d\r\n", temp);

    mqttSize = mqtt.createPacket(&mqtt, temp);
    os_printf("Created MQTT-Packet of size: %d\r\n", mqttSize);
    os_printf("FIXED HEADER:\r\n");
    os_printf("\t%d\r\n", mqtt.fix_header[0]);
    os_printf("\t%d\r\n", mqtt.fix_header[1]);
    os_printf("VARIABLE HEADER:\r\n");
    os_printf("\t%d\r\n", mqtt.var_header[0]);
    os_printf("\t%d\r\n", mqtt.var_header[1]);
    const uint8_t var_header_len = (mqttSize - 2 - 4);
    for(uint8_t i = 2; i < mqtt.var_header_size; i++)
        os_printf("\t%c\r\n", mqtt.var_header[i]);
    os_printf("PAYLOAD AS TEMP\r\n");
    const uint32_t t = mqtt.payload[0] << 24 | mqtt.payload[1] << 16 | mqtt.payload[2] << 8 | mqtt.payload[3];
    os_printf("\t%d\r\n", t);

    wifi.publishData(&wifi);
}

void ICACHE_FLASH_ATTR user_init(void) {
    initBMP280(&bmp280);
    const uint8_t bmp280_id = bmp280.getID();

    initMQTT(&mqtt);

    if(bmp280_id == BMP280_EXPECTED_ID) {
        os_printf("Found BMP280\r\n");
        initWifi(&wifi);

        /* Veroeffentliche alle 1s neuen Temperaturwert */
        os_timer_disarm(&mqtt_publish_timer);
        os_timer_setfn(&mqtt_publish_timer, (os_timer_func_t*)mqtt_publish_data, NULL);
        os_timer_arm(&mqtt_publish_timer, 10000, true);
    } else {
        os_printf("BMP280-ID GOT %d, expected %d - END.\r\n", bmp280_id, BMP280_EXPECTED_ID);
    }
}
