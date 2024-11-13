#include <esp_ota_ops.h>

#include "WifiAccessPoint.hpp"
#include "WebServer.hpp"

extern "C"
void app_main(void)
{
    // create the access point
    static WifiAccessPoint wap;

    // provide the backend
    // static Catalog catalog;

    // provide the frontends
    static WebServer webserver;

    // start publishing the ssid
    wap.publish();

    // accept working firmware
    esp_ota_mark_app_valid_cancel_rollback();
}
