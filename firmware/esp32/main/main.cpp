#include <esp_ota_ops.h>

#include "WifiAccessPoint.hpp"
#include "WebServer.hpp"
#include "SdCard.hpp"

extern "C"
void app_main(void)
{
    // create the access point
    static WifiAccessPoint wap;

    // provide the frontends
    static WebServer webserver;

    // provide the storage
    static SdCard sdcard;

    // accept working firmware
    esp_ota_mark_app_valid_cancel_rollback();
}
