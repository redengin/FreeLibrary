#include "WifiAccessPoint.hpp"

#include <esp_wifi.h>
#include <nvs_flash.h>

#include <cstring>

WifiAccessPoint::WifiAccessPoint(const std::string ssid)
    : ssid(ssid)
{
    // start the network stack
    ESP_ERROR_CHECK(esp_netif_init());

    // configure netif for wifi access point
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    // initialize wifi
    const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    // set the ssid
    setSsid(ssid);
}

void WifiAccessPoint::publish()
{
    ESP_ERROR_CHECK(esp_wifi_start());
}

void WifiAccessPoint::setSsid(const std::string ssid)
{
    wifi_config_t wifi_config;
    esp_wifi_get_config(WIFI_IF_AP, &wifi_config);
    std::memcpy(&wifi_config.ap.ssid, ssid.c_str(), ssid.size());
    wifi_config.ap.ssid_len = ssid.size();

    ESP_ERROR_CHECK(esp_wifi_set_config(
        static_cast<wifi_interface_t>(ESP_IF_WIFI_AP), &wifi_config));
}
