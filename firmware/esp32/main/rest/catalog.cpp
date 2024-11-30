#include "catalog.hpp"
#include "utils.hpp"

#include <esp_log.h>
using rest::catalog::TAG;
#include <cJSON.h>



extern "C" esp_err_t GET(httpd_req_t*);
extern "C" esp_err_t PUT(httpd_req_t*);
extern "C" esp_err_t DELETE(httpd_req_t*);

struct Context {
    WebServer& webserver;        
    Catalog& catalog;        
};

void rest::catalog::registerHandlers(WebServer& webserver, Catalog& catalog)
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

    // disallow fragments
    if (uri.contains("#"))
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

static std::string catalogPath(const char* const requestUri)
{
    // omit the base uri
    auto path =  std::string(requestUri + rest::catalog::uri_wildcard.length() - sizeof('*'));
    // remove query
    auto pos = path.find('?');
    path.erase(pos);
    // remove fragment
    pos = path.find('#');
    path.erase(pos);

    // decode http tokens (in-place)
    rest::httpDecode(path);

    return path;
}


esp_err_t GET(httpd_req_t* const request)
{
    const Context& context = reinterpret_cast<const Context&>(request->user_ctx);
    switch(uriType(request->uri))
    {
        case UriType::FOLDER: {
            const auto path = catalogPath(request->uri);
            ESP_LOGD(TAG, "handling %s for FOLDER %s", request->uri, path.c_str());

            if (! context.catalog.hasFolder(path))
                return httpd_resp_send_404(request);

            // send the data
            auto data = context.catalog.folderInfo(path);


            // auto response = cJSON_CreateObject();
            // auto subfolders = cJSON_CreateArray();
            // auto files = cJSON_CreateArray();
            // for(const auto& entry : context.catalog.folderIterator(uri))
            // {
            //     if (entry.path().filename().string().front() == Catalog::HIDDEN_PREFIX)
            //         // ignore hidden files
            //         continue;

            //     if (entry.is_directory())
            //     {
            //         cJSON_AddItemToArray(subfolders,
            //             cJSON_CreateString(entry.path().parent_path().filename().c_str()));
            //     }
            // }
            return ESP_FAIL;
        }

        // case UriType::FILE: {
        //     if (! context.catalog.hasContent(uri))
        //         return httpd_resp_send_404(request);

        //     // set headers
        //     auto timestamp = context.catalog.timestamp(uri);
        //     std::tm tm;
        //     gmtime_r(&timestamp, &tm);
        //     char headerField[30];
        //     strftime(headerField, sizeof(headerField), rest::ISO_8601_FORMAT, &tm);
        //     httpd_resp_set_hdr(request, "X-FileTimestamp", headerField);

        //     // send the data 
        //     auto fis = context.catalog.readContent(uri);
        //     if (! fis.is_open())
        //         return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Too many requests (try again)");
        //     return rest::sendOctetStream(request, fis);
        // }

        // TODO
        default: return ESP_FAIL;
    }
}

esp_err_t PUT(httpd_req_t* const request)
{
    // TODO
    return ESP_FAIL;
}

esp_err_t DELETE(httpd_req_t* const request)
{
    // TODO
    return ESP_FAIL;
}

