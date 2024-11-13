#include "WebServer.hpp"

#include <esp_log.h>

extern "C" esp_err_t PORTAL(httpd_req_t*);

WebServer::WebServer()
{
    // create HTTP server
    httpd_config_t httpConfig = HTTPD_DEFAULT_CONFIG();
    // TODO support more users (max_open_sockets)
    ESP_ERROR_CHECK(httpd_start(&httpHandle, &httpConfig));

    // create HTTPS server
    // FIXME
    // httpd_ssl_config_t httpsConfig = HTTPD_SSL_CONFIG_DEFAULT();
    // httpsConfig.httpd.max_open_sockets = 1; // only allow one admin at a time
    // ESP_ERROR_CHECK(httpsd_start(&httpHandle, &httpConfig));

    // provide captive portal
    registerUriHandler(
        httpd_uri_t{
            .uri = "/",
            .method = HTTP_GET,
            .handler = PORTAL,
            .user_ctx = this
        }
    );
}

void WebServer::registerUriHandler(const httpd_uri_t& handler)
{
    ESP_ERROR_CHECK(httpd_register_uri_handler(httpHandle, &handler));
}

extern "C" const char portalHtml_start[] asm("_binary_portal_html_start");
extern "C" const char portalHtml_end[] asm("_binary_portal_html_end");
esp_err_t PORTAL(httpd_req_t* request)
{
    auto response = request;
    ESP_ERROR_CHECK(httpd_resp_set_type(response, "text/html"));
    const size_t portalHtml_sz = portalHtml_end - portalHtml_start;
    return httpd_resp_send(response, portalHtml_start, portalHtml_sz);
}
