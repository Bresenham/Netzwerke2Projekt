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
        os_printf("%c ", pusrdata[i]);
    }
    os_printf("\r\n");
}

void ICACHE_FLASH_ATTR user_tcp_sent_cb(void *arg) {
    os_printf("Sent callback: data sent successfully.\r\n");
}

void ICACHE_FLASH_ATTR user_tcp_discon_cb(void *arg) {
    os_printf("Disconnected from server.\r\n");
}

void ICACHE_FLASH_ATTR user_send_data(struct espconn *pespconn) {
/*     char *pbuf = (char *)os_zalloc(PACKET_SIZE);
  
    os_sprintf(pbuf, REQUEST_TEXT, data_to_send);

    os_printf("-------------- HTTP REQUEST CONTENT --------------\r\n");
    os_printf(pbuf);
    os_printf("-------------- END --------------\r\n");
  
    espconn_send(pespconn, (uint8_t*)pbuf, os_strlen(pbuf));
     
    os_free(pbuf); */
}

void ICACHE_FLASH_ATTR user_tcp_connect_cb(void *arg) {
    struct espconn *pespconn = (struct espconn *)arg;
  
    os_printf("Connected to server...\r\n");
  
    espconn_regist_recvcb(pespconn, user_tcp_recv_cb);
    espconn_regist_sentcb(pespconn, user_tcp_sent_cb);
    espconn_regist_disconcb(pespconn, user_tcp_discon_cb);
     
    user_send_data(pespconn);
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
        os_printf("Connected to router and assigned IP!\r\n");
  
        // Connect to tcp server as NET_DOMAIN
        user_tcp_conn.proto.tcp = &user_tcp;
        user_tcp_conn.type = ESPCONN_TCP;
        user_tcp_conn.state = ESPCONN_NONE;
        
        /* Get IP of remote server */
        /* TODO: HIER STEHT DIE IP-ADRESSE DES RASPI IM LOKALEN NETZWERK 
           GGF. AUCH DYNAMISCHE IP UEBER NAMEN DES RASPI MOEGLICH (GLAUB ICH)
        */
        const char esp_tcp_server_ip[4] = {1, 3, 3, 7}; // remote IP of TCP server

        os_memcpy(user_tcp_conn.proto.tcp->remote_ip, esp_tcp_server_ip, 4);

        /* TODO: HIER STEHT DER PORT, UEBER DEN MIT DEM RASPI KOMMUNIZIERT WIRD.
           AKTUELL NOCH KEINEN PLAN, WELCHEN PORT MOSQUITTO BENUTZT
         */
        user_tcp_conn.proto.tcp->remote_port = 80;  // remote port

        user_tcp_conn.proto.tcp->local_port = espconn_port(); //local port of ESP8266

        espconn_regist_connectcb(&user_tcp_conn, user_tcp_connect_cb); // register connect callback
        espconn_regist_reconcb(&user_tcp_conn, user_tcp_recon_cb); // register reconnect callback as error handler
        espconn_connect(&user_tcp_conn); 
    } else {
        if ((wifi_station_get_connect_status() == STATION_WRONG_PASSWORD ||
                wifi_station_get_connect_status() == STATION_NO_AP_FOUND ||
                wifi_station_get_connect_status() == STATION_CONNECT_FAIL)) {
            os_printf("Connection to router failed!\r\n");
        } 
        else {
            //re-arm timer to check ip
            os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
            os_timer_arm(&test_timer, 100, false);
        }
    }
}

void ICACHE_FLASH_ATTR wifiConnect(Wifi *self, MQTT *packet) {
    // Wifi configuration 
    struct station_config stationConf; 
        
    os_memset(stationConf.ssid, 0, 32);
    os_memset(stationConf.password, 0, 64);
        
    // No MAC-specific scanning
    stationConf.bssid_set = 0; 
        
    //Set ap settings 
    os_memcpy(&stationConf.ssid, self->SSID, 32); 
    os_memcpy(&stationConf.password, self->PW, 64); 
    wifi_station_set_config(&stationConf);
    wifi_station_connect();

    // Set timer to check whether router allotted an IP 
    os_timer_disarm(&test_timer);
    os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
    os_timer_arm(&test_timer, 100, false);
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
    wifi_set_opmode(STATION_MODE);

    self->SSID = "";
    self->PW = "";

    self->publishData = &wifiConnect;
    os_printf("Initialized Wifi\r\n");
}