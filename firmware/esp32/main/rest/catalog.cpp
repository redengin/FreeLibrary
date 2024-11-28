#include "catalog.hpp"

constexpr std::string uri_wildcard{"/catalog/*"};

extern "C" esp_err_t GET(httpd_req_t*);
extern "C" esp_err_t PUT(httpd_req_t*);
extern "C" esp_err_t DELETE(httpd_req_t*);

struct Context {
    WebServer* webserver;        
    Catalog* catalog;        
};

void rest::registerCatalog(WebServer& webserver, Catalog& catalog)
{
    // FIXME should use smart pointers in parameters and creation of context
    static Context context;
    context.webserver = &webserver;
    context.catalog = &catalog;

    webserver.registerUriHandler(
        httpd_uri_t{
            .uri = uri_wildcard.c_str(),
            .method = HTTP_GET,
            .handler = GET,
            .user_ctx = &context
        }
    );
    webserver.registerUriHandler(
        httpd_uri_t{
            .uri = uri_wildcard.c_str(),
            .method = HTTP_PUT,
            .handler = PUT,
            .user_ctx = &context
        }
    );
    webserver.registerUriHandler(
        httpd_uri_t{
            .uri = uri_wildcard.c_str(),
            .method = HTTP_DELETE,
            .handler = DELETE,
            .user_ctx = &context
        }
    );
}

static std::string catalogUri(const char* const requestUri)
{
    // omit the base uri
    return std::string(requestUri + uri_wildcard.length() - sizeof('*'));
}

enum class UriType {
    ILLEGAL,
    FOLDER,
    FILE,
    ICON,
    TITLE
};

UriType uriType(const std::string uri)
{
    const std::filesystem::path path(uri);
    // disallow relative paths
    if (path.is_relative())
        return UriType::ILLEGAL;

    if (uri.ends_with("?icon"))
        return UriType::ICON;

    if (uri.contains("?title="))
        return UriType::TITLE;

    if (path.has_filename())
        return UriType::FILE;
    else
        return UriType::FOLDER;
}

esp_err_t GET(httpd_req_t* const request)
{
    // const Context& context = reinterpret_cast<const Context&>(request->user_ctx);
    const auto uri = catalogUri(request->uri);
    switch(uriType(uri))
    {
        // TODO
        default: return ESP_FAIL;
    }
}

esp_err_t PUT(httpd_req_t* const request)
{
    // const Context& context = reinterpret_cast<Context&>(request->user_ctx);
    const auto uri = catalogUri(request->uri);
    switch(uriType(uri))
    {
        // TODO
        default: return ESP_FAIL;
    }
}

esp_err_t DELETE(httpd_req_t* const request)
{
    // const Context& context = reinterpret_cast<Context&>(request->user_ctx);
    const auto uri = catalogUri(request->uri);
    switch(uriType(uri))
    {
        // TODO
        default: return ESP_FAIL;
    }
}

