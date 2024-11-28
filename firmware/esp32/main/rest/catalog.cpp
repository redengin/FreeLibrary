#include "catalog.hpp"

constexpr char uri[] = "/catalog/*";

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
            .uri = uri,
            .method = HTTP_GET,
            .handler = GET,
            .user_ctx = &context
        }
    );
    webserver.registerUriHandler(
        httpd_uri_t{
            .uri = uri,
            .method = HTTP_PUT,
            .handler = PUT,
            .user_ctx = &context
        }
    );
    webserver.registerUriHandler(
        httpd_uri_t{
            .uri = uri,
            .method = HTTP_DELETE,
            .handler = DELETE,
            .user_ctx = &context
        }
    );
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

    const std::string uri{request->uri};
    switch(uriType(uri))
    {

        default: return ESP_FAIL;
    }
}

esp_err_t PUT(httpd_req_t* const request)
{
    // const Context& context = reinterpret_cast<Context&>(request->user_ctx);
    // TODO
    return ESP_FAIL;
}

esp_err_t DELETE(httpd_req_t* const request)
{
    // const Context& context = reinterpret_cast<Context&>(request->user_ctx);
    // TODO
    return ESP_FAIL;
}

