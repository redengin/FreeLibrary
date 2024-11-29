#include "catalog.hpp"

#include "utils.hpp"


extern "C" esp_err_t GET(httpd_req_t*);
extern "C" esp_err_t PUT(httpd_req_t*);
extern "C" esp_err_t DELETE(httpd_req_t*);

struct Context {
    WebServer& webserver;        
    Catalog& catalog;        
};

void rest::registerCatalog(WebServer& webserver, Catalog& catalog)
{
    static Context context{
        .webserver = webserver,
        .catalog = catalog,
    };

    webserver.registerUriHandler(
        httpd_uri_t{
            .uri = uri_wildcard.begin(),
            .method = HTTP_GET,
            .handler = GET,
            .user_ctx = &context
        }
    );
    webserver.registerUriHandler(
        httpd_uri_t{
            .uri = uri_wildcard.begin(),
            .method = HTTP_PUT,
            .handler = PUT,
            .user_ctx = &context
        }
    );
    webserver.registerUriHandler(
        httpd_uri_t{
            .uri = uri_wildcard.begin(),
            .method = HTTP_DELETE,
            .handler = DELETE,
            .user_ctx = &context
        }
    );
}

static std::string catalogUri(const char* const requestUri)
{
    // omit the base uri
    return std::string(requestUri + rest::uri_wildcard.length() - sizeof('*'));
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
    // disallow relative paths
    if (uri.contains("../"))
        return UriType::ILLEGAL;

    if (uri.ends_with("?icon"))
        return UriType::ICON;

    if (uri.contains("?title="))
        return UriType::TITLE;

    if (uri.back() == '/')
        return UriType::FOLDER;
    else
        return UriType::FILE;
}

esp_err_t GET(httpd_req_t* const request)
{
    const Context& context = reinterpret_cast<const Context&>(request->user_ctx);
    const auto uri = catalogUri(request->uri);
    switch(uriType(uri))
    {
        case UriType::FILE: {
            if (! context.catalog.exists(uri))
                return httpd_resp_send_404(request);

            // set headers
            auto timestamp = context.catalog.timestamp(uri);
            std::tm tm;
            gmtime_r(&timestamp, &tm);
            char headerField[30];
            strftime(headerField, sizeof(headerField), rest::ISO_8601_FORMAT, &tm);
            httpd_resp_set_hdr(request, "X-FileTimestamp", headerField);

            // send the data 
            // auto fis = context.catalog.readContent(uri);
            // if (! fis.is_open())
            //     return httpd_resp_send_404(request);

            // std::unique_ptr<char[]> buffer = std::make_unique<char[]>(rest::CHUNK_SIZE);
            // if (! buffer)
            //     return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Too many requests (try again)");

            // httpd_resp_set_type(request, "application/octet-stream");
            // return rest::httpSendData(request, fis, buffer);

            return ESP_OK;
        
            break;
        }

        // TODO
        default: return ESP_FAIL;
    }
}

esp_err_t PUT(httpd_req_t* const request)
{
    // const Context& context = reinterpret_cast<Context&>(request->user_ctx);
    // const auto uri = catalogUri(request->uri);

    // switch(uriType(uri))
    // {
    //     case UriType::FILE : {
    //         const auto path = std::filesystem::path(uri).parent_path();
    //         if (context.catalog.isLocked(path))
    //         {

    //         }
    //         break;
    //     }
    //     // TODO
    //     default: return ESP_FAIL;
    // }

    return ESP_FAIL;
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

