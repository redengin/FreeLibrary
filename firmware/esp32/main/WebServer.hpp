#pragma once

#include <esp_http_server.h>
#include <esp_https_server.h>


class WebServer
{
public:
    static constexpr char TAG[] = "WebServer";    ///< ESP logging tag

    WebServer();

    void registerUriHandler(const httpd_uri_t& uri);

private:
    httpd_handle_t httpHandle;
    httpd_handle_t httpsHandle;
};
