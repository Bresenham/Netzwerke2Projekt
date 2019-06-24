/* 
 * Created @ 11.05.2019
 * 
*/

#include "wifi.h"

/********************/
/* PRIVATE FUNCTIONS */
/********************/
LOCAL os_timer_t test_timer;
LOCAL struct _esp_tcp user_tcp;
LOCAL struct espconn user_tcp_conn;

void ICACHE_FLASH_ATTR user_tcp_recv_cb(void *arg, char *pusrdata, unsigned short length) {
    os_printf("Received data string\r\n\t");
    for(uint16_t i = 0; i < length; i++) {
        os_printf("%d", pusrdata[i]);
    }
    os_printf("\r\n");
}

void ICACHE_FLASH_ATTR user_tcp_sent_cb(void *arg) {
    os_printf("Sent callback: data sent successfully.\r\n");
}

void ICACHE_FLASH_ATTR user_tcp_discon_cb(void *arg) {
    os_printf("Disconnected from server.\r\n");
}

void ICACHE_FLASH_ATTR user_sent_data(struct espconn *pespconn) {
    os_printf("In user_sent_data\r\n");
    os_printf("Sending CONNECT...\r\n");
    char *pbuf = (char*)os_zalloc(sizeof(uint8_t) * mqtt.connectPacketLength);
    mqtt.fillConnectPacket(&mqtt, pbuf);
    espconn_send(pespconn, (uint8_t*)pbuf, mqtt.connectPacketLength);
    os_free(pbuf);
    mqtt.hasSentConnect = true;
}

LOCAL void ICACHE_FLASH_ATTR user_tcp_write_finish(void *arg) {
    struct espconn *pespconn = arg;
    os_printf("In tcp_write_finish\r\n");
    if(mqtt.hasNewTempData) {
        os_printf("Sending PUBLISH...\r\n");
        char *pbuf = (char*)os_zalloc(sizeof(uint8_t) * mqttSize);
        mqtt.fillPublishPacket(&mqtt, pbuf);
        espconn_send(pespconn, (uint8_t*)pbuf, mqttSize);
        os_free(pbuf);
        mqtt.hasNewTempData = false;
    }
    espconn_disconnect(pespconn);
}

LOCAL void ICACHE_FLASH_ATTR user_tcp_connect_cb(void *arg) {
    struct espconn *pespconn = arg;
 
    os_printf("connect succeed !!! \r\n");
 
    espconn_regist_recvcb(pespconn, user_tcp_recv_cb);
    espconn_regist_sentcb(pespconn, user_tcp_sent_cb);
    espconn_regist_disconcb(pespconn, user_tcp_discon_cb);
 
    espconn_set_opt(pespconn, ESPCONN_COPY | ESPCONN_KEEPALIVE); // enable write buffer
 
    espconn_regist_write_finish(pespconn, user_tcp_write_finish); // register write finish callback
    
    user_sent_data(pespconn);
}

void ICACHE_FLASH_ATTR user_tcp_recon_cb(void *arg, sint8 err) {
    os_printf("Reconnect callback called, error code: %d !!! \r\n", err);
}

void ICACHE_FLASH_ATTR user_dns_check_cb(void *arg) {
    os_printf("DNS CHECK CB CALLED\r\n");
}

void ICACHE_FLASH_ATTR user_check_ip(void) {
    struct ip_info ipconfig;
 
   //disarm timer first
    os_timer_disarm(&test_timer);
 
   //get ip info of ESP8266 station
    wifi_get_ip_info(STATION_IF, &ipconfig);
 
    if (wifi_station_get_connect_status() == STATION_GOT_IP && ipconfig.ip.addr != 0) {
      os_printf("got ip !!! \r\n");
 
      // Connect to tcp server as NET_DOMAIN
      user_tcp_conn.proto.tcp = &user_tcp;
      user_tcp_conn.type = ESPCONN_TCP;
      user_tcp_conn.state = ESPCONN_NONE;
       
      const char esp_server_ip[4] = {192, 168, 137, 37}; // remote IP of tcp server
 
      os_memcpy(user_tcp_conn.proto.tcp->remote_ip, esp_server_ip, 4); // remote ip of tcp server 
       
      user_tcp_conn.proto.tcp->remote_port = 1883; // remote port of tcp server
       
      user_tcp_conn.proto.tcp->local_port = espconn_port(); //local port of ESP8266
       
      espconn_regist_connectcb(&user_tcp_conn, user_tcp_connect_cb); // register connect callback
      espconn_regist_reconcb(&user_tcp_conn, user_tcp_recon_cb); // register reconnect callback as error handler
       
      espconn_connect(&user_tcp_conn); // tcp connect
 
 
    } else {
        if ((wifi_station_get_connect_status() == STATION_WRONG_PASSWORD ||
                wifi_station_get_connect_status() == STATION_NO_AP_FOUND ||
                wifi_station_get_connect_status() == STATION_CONNECT_FAIL)) 
        {
         os_printf("connect fail !!! \r\n");
        } 
      else
      {
           //re-arm timer to check ip
            os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
            os_timer_arm(&test_timer, 100, 0);
        }
    }
}

void ICACHE_FLASH_ATTR wifiConnect(Wifi *self) {
   // Wifi configuration 
   char ssid[32] = "networkap"; 
   char password[64] = "qwertzuiop"; 
   struct station_config stationConf; 
 
   //need not mac address
   stationConf.bssid_set = 0; 
    
   //Set ap settings 
   os_memcpy(&stationConf.ssid, ssid, 32); 
   os_memcpy(&stationConf.password, password, 64); 
   wifi_station_set_config(&stationConf);
   wifi_station_connect();
 
   //set a timer to check whether got ip from router succeed or not.
    os_timer_disarm(&test_timer);
    os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
    os_timer_arm(&test_timer, 100, 0);
}


/********************/
/* PUBLIC FUNCTIONS */
/********************/

void ICACHE_FLASH_ATTR initWifi(Wifi *self) {
    /* Wir wollen nicht, dass ich der ESP mit einem
       zuvor gespeicherten Wifi automatisch verbindet.
       Wir wollen auch nicht, dass er Verbindungsdaten speichert.
     */
    wifi_station_ap_number_set(0);
    wifi_station_set_auto_connect(false);
    wifi_set_opmode(STATIONAP_MODE);

    self->publishData = &wifiConnect;
    os_printf("Initialized Wifi\r\n");
}