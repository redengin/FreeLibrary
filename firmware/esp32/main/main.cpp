// #include <esp_event.h>
#include <esp_ota_ops.h>
// #include <nvs_flash.h>
// #include <esp_wifi.h>

// #include <string>
// #include <cstring>
// #include "dns_server.h"
// #include "sneakernet/SneakerNet.hpp"
// #include "sneakernet/WebServer.hpp"

#include "WifiAccessPoint.hpp"


/// create wifi captive portal
// static void start_wifi_ap(std::string& ssid);

extern "C"
void app_main(void)
{
    // create default event bus (required for IDF drivers)
    // ESP_ERROR_CHECK(esp_event_loop_create_default());

    // start the network stack
    // ESP_ERROR_CHECK(esp_netif_init());

    // create the access point
    static WifiAccessPoint wap;

    // provide the backend
    // static Catalog catalog;

    // static WebServer webserver(sneakerNet);

    // start the access point
    // std::string ssid = "Wee Share";
    // start_wifi_ap(ssid);

    // make the wap public
    wap.publish();

    // accept working firmware
    esp_ota_mark_app_valid_cancel_rollback();
}


// static void start_wifi_ap(std::string& ssid)
// {
//     // IDF wifi drivers requires access to non-volatile flash
//     ESP_ERROR_CHECK(nvs_flash_init());

//     // configure netif as wifi access point
//     esp_netif_create_default_wifi_ap();

//     // configure wifi driver per kconfig
//     const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK(esp_wifi_init(&cfg));

//     // put wifi hardware into access point mode
//     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

//     // configure access point
//     wifi_config_t wifi_config = { .ap {
//         .ssid = {0},        // will set below
//         .password = {0},    // not used for WIFI_AUTH_OPEN
//         .ssid_len = 0,      // will set below
//         .channel = 0,       // will set below
//         .authmode = WIFI_AUTH_OPEN,
//         .ssid_hidden = false,
//         .max_connection = ESP_WIFI_MAX_CONN_NUM,
//         .beacon_interval = 100,
//         .csa_count = 0,     // unused
//         .dtim_period = 0,   // unused
//         .pairwise_cipher = WIFI_CIPHER_TYPE_TKIP_CCMP,
//         .ftm_responder = false,
//         .pmf_cfg { .capable = false, .required = false},
//         .sae_pwe_h2e = WPA3_SAE_PWE_UNSPECIFIED
//     }};

//     // set the ssid
//     std::memcpy(wifi_config.ap.ssid, ssid.c_str(), ssid.size());
//     wifi_config.ap.ssid_len = ssid.length();

//     // set a "random" channel based on mac
//     // srand(*reinterpret_cast<int*>(mac + 2));
//     // WIFI channels https://en.wikipedia.org/wiki/List_of_WLAN_channels
//     constexpr int WIFI_CHANNEL_MAX = 11;    // North America limit
//     wifi_config.ap.channel = (rand() % WIFI_CHANNEL_MAX) + 1;
//     ESP_ERROR_CHECK(esp_wifi_set_config(static_cast<wifi_interface_t>(ESP_IF_WIFI_AP), &wifi_config));

//     // handle DNS requests upon connection w/ redirect to captive portal
//     // ESP_ERROR_CHECK(false == xTaskCreate(dns_server_task, "dns_server", 4096, NULL, 5, NULL));

//     ESP_ERROR_CHECK(esp_wifi_start());
// }