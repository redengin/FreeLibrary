#include "WebServer.hpp"

#include <esp_log.h>

extern "C" esp_err_t redirect(httpd_req_t *req, httpd_err_code_t err);

extern "C" esp_err_t PORTAL(httpd_req_t*);

WebServer::WebServer()
{
    // create HTTP server
    httpd_config_t httpConfig = HTTPD_DEFAULT_CONFIG();
    // TODO support more users (max_open_sockets)
    ESP_ERROR_CHECK(httpd_start(&httpHandle, &httpConfig));
    // upon 404, redirect to index
    ESP_ERROR_CHECK(httpd_register_err_handler(httpHandle, HTTPD_404_NOT_FOUND, redirect));

    // create HTTPS server
    // FIXME
    // httpd_ssl_config_t httpsConfig = HTTPD_SSL_CONFIG_DEFAULT();
    // httpsConfig.httpd.max_open_sockets = 1; // only allow one admin at a time
    // ESP_ERROR_CHECK(httpsd_start(&httpHandle, &httpConfig));

    // FIXME
    // android URLS:
    //      /generate_204
    //      /gen_204

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

/// provides captive portal redirect
esp_err_t redirect(httpd_req_t *request, httpd_err_code_t err)
{
    ESP_LOGV(WebServer::TAG, "Serving 302 redirect");
    // Set status
    httpd_resp_set_status(request, "302 Temporary Redirect");
    httpd_resp_set_hdr(request, "Location", "/");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(request, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
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
