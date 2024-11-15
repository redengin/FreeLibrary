#include "WifiAccessPoint.hpp"

#include <esp_log.h>

#include <esp_wifi.h>
#include <nvs_flash.h>

#include <freertos/task.h>
static void dns_service_task(void *pvParameters);
#include <lwip/sockets.h>

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

    // handle DNS requests, redirecting to captive portal
    ESP_ERROR_CHECK(!(pdPASS == xTaskCreate(
                                dns_service_task,
                                "dns_service",
                                4096 /* stack depth */, // TODO reduce to minimum
                                nullptr /* parameters (none) */,
                                tskIDLE_PRIORITY /* priority */,
                                nullptr /* handle (not used) */
                            )
    ));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "started");
}

void WifiAccessPoint::setSsid(const std::string ssid)
{
    wifi_config_t wifi_config;
    esp_wifi_get_config(WIFI_IF_AP, &wifi_config);
    std::memcpy(&wifi_config.ap.ssid, ssid.c_str(), ssid.size());
    wifi_config.ap.ssid_len = ssid.size();

    ESP_ERROR_CHECK(esp_wifi_set_config(
        static_cast<wifi_interface_t>(ESP_IF_WIFI_AP), &wifi_config));

    ESP_LOGI(TAG, "publishing SSID: %s", ssid.c_str());
}

void dns_service_task(void *)
{
    // create the listening socket
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    ESP_ERROR_CHECK(sock <= 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(53 /* DNS PORT */);
    ESP_ERROR_CHECK(!(0 == bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))));
    ESP_LOGI(WifiAccessPoint::TAG, "DNS started");

    while(true) {
        char request[128];
        struct sockaddr from;
        socklen_t from_sz = sizeof(from);
        int sz = recvfrom(sock, request, sizeof(request), 0 /* flags (none) */,
            &from, &from_sz);
        ESP_LOGI(WifiAccessPoint::TAG, "handling DNS request");
        // create reply
        auto reply = WifiAccessPoint::dnsReply(request, sz);
        sendto(sock, reply.data(), reply.size(), 0 /* flags (none) */, &from, from_sz);
    }

    // remove thread from scheduler
    vTaskDelete(nullptr);
}

std::vector<char> WifiAccessPoint::dnsReply(const void* const request, const size_t sz)
{
    return std::vector<char>{};
}
