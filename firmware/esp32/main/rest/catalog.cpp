#include "catalog.hpp"
#include "utils.hpp"

#include <esp_log.h>
using rest::catalog::TAG;
#include <cJSON.h>


struct Context {
    WebServer& webserver;        
    Catalog& catalog;
};
extern "C" esp_err_t handler(httpd_req_t*);

void rest::catalog::registerHandlers(WebServer& webserver, Catalog& catalog)
{
    static Context context{
        .webserver = webserver,
        .catalog = catalog,
    };

    webserver.registerUriHandler(
        httpd_uri_t{
            .uri = uri_wildcard.begin(),
            .method = static_cast<http_method>(HTTP_ANY),
            .handler = handler,
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
static UriType uriType(const std::string& uri)
{
    if (uri.ends_with("?icon"))
        return UriType::ICON;

    if (uri.contains("?title="))
        return UriType::TITLE;

    // no other queries allowed
    if (uri.contains("?"))
        return UriType::ILLEGAL;

    // fragments not allowed
    if (uri.contains("#"))
        return UriType::ILLEGAL;

    if (uri.back() == '/')
        return UriType::FOLDER;
    else
        return UriType::FILE;
}

// forward declarations
static esp_err_t ILLEGAL_REQUEST(httpd_req_t* const request);
static esp_err_t GET_FOLDER(httpd_req_t* const request);
static esp_err_t DELETE_FOLDER(httpd_req_t* const request);
static esp_err_t GET_FILE(httpd_req_t* const request);
static esp_err_t PUT_FILE(httpd_req_t* const request);
static esp_err_t DELETE_FILE(httpd_req_t* const request);
static esp_err_t GET_ICON(httpd_req_t* const request);
static esp_err_t PUT_ICON(httpd_req_t* const request);
static esp_err_t PUT_TITLE(httpd_req_t* const request);

esp_err_t handler(httpd_req_t* request)
{
    switch(uriType(request->uri))
    {
        case UriType::FOLDER :
        {
            switch(request->method)
            {
                case HTTP_GET : return GET_FOLDER(request);
                case HTTP_DELETE: return DELETE_FOLDER(request);
                default:
                    return ILLEGAL_REQUEST(request);
            }
        }

        case UriType::FILE :
        {
            switch(request->method)
            {
                case HTTP_GET : return GET_FILE(request);
                case HTTP_PUT: return PUT_FILE(request);
                case HTTP_DELETE: return DELETE_FILE(request);

                default:
                    return ILLEGAL_REQUEST(request);
            }
        }

        case UriType::ICON:
        {
            switch(request->method)
            {
                case HTTP_GET : return GET_ICON(request);
                case HTTP_PUT: return PUT_ICON(request);

                default:
                    return ILLEGAL_REQUEST(request);
            }
        }

        case UriType::TITLE:
        {
            switch(request->method)
            {
                case HTTP_PUT: return PUT_TITLE(request);

                default:
                    return ILLEGAL_REQUEST(request);
            }
        }

        default:
        {
            ESP_LOGI(TAG, "unhandled request %s", request->uri);
            return ILLEGAL_REQUEST(request);
        }
    }
}

static esp_err_t ILLEGAL_REQUEST(httpd_req_t* request)
{
    return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, nullptr);
}

static esp_err_t TOO_MANY_REQUESTS(httpd_req_t* request)
{
    return httpd_resp_send_custom_err(request, "429 Too many requests", "try-again");
}

static std::string catalogPath(const char* const requestUri)
{
    // omit the base uri
    auto path =  std::string(requestUri + rest::catalog::uri_wildcard.length() - sizeof('*'));
    // remove query
    auto pos = path.find('?');
    if (pos != std::string::npos) path.erase(pos);
    // remove fragment
    pos = path.find('#');
    if (pos != std::string::npos) path.erase(pos);

    // decode http tokens (in-place)
    rest::httpDecode(path);

    return path;
}

esp_err_t GET_FOLDER(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto folderpath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for FOLDER [/%s]", request->uri, folderpath.c_str());

    if (! context->catalog.hasFolder(folderpath))
        return httpd_resp_send_404(request);

    // send the data
    auto subfolders = cJSON_CreateArray();
    auto files = cJSON_CreateArray();
    for (auto& entry : context->catalog.entries(folderpath))
    {
        if (entry.is_directory())
            cJSON_AddItemToArray(subfolders, cJSON_CreateString(entry.path().filename().c_str()));

        else if (entry.is_regular_file())
        {
            auto fileInfo = cJSON_CreateObject();
            cJSON_AddItemToObject(fileInfo, "name", cJSON_CreateString(entry.path().filename().c_str()));

            // FIXME file size is incorrect
            cJSON_AddNumberToObject(fileInfo, "size", entry.file_size());

            // FIXME timestamp is incorrect
            auto fileTime = entry.last_write_time().time_since_epoch().count();
            char buffer[20];
            rest::timestamp(fileTime, buffer);
            cJSON_AddItemToObject(fileInfo, "timestamp", cJSON_CreateString(buffer));

            auto filepath = (std::filesystem::path(folderpath) / entry.path().filename()).string();
            auto title = context->catalog.getTitle(filepath);
            if (title)
                cJSON_AddItemToObject(fileInfo, "timestamp", cJSON_CreateString(title.value().c_str()));

            cJSON_AddBoolToObject(fileInfo, "hasIcon", context->catalog.hasIcon(filepath));

            cJSON_AddItemToArray(files, fileInfo);
        }
    }
    auto response = cJSON_CreateObject();
    cJSON_AddBoolToObject(response, "locked", context->catalog.isLocked(folderpath));
    cJSON_AddItemToObject(response, "subfolders", subfolders);
    cJSON_AddItemToObject(response, "files", files);

    char *const data = cJSON_PrintUnformatted(response);
    httpd_resp_set_type(request, "application/json");
    auto ret = httpd_resp_send(request, data, strlen(data));

    cJSON_Delete(response);
    cJSON_free(data);

    return ret;
}

esp_err_t DELETE_FOLDER(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto folderpath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for DELETE FOLDER [/%s]", request->uri, folderpath.c_str());

    if (! context->catalog.hasFolder(folderpath))
        return httpd_resp_send_404(request);

    // FIXME should check if parent is also locked
    if (context->catalog.isLocked(folderpath))
    {
        // TODO determine if caller has admin credentials
        return httpd_resp_send_err(request, HTTPD_401_UNAUTHORIZED, "folder is locked by admin");
    }

    if (context->catalog.removeFolder(folderpath))
        return httpd_resp_sendstr(request, "OK");
    else
        return httpd_resp_send_custom_err(request, "409 - Directory not empty", "failed");
}

esp_err_t GET_FILE(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto filepath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for GET FILE [/%s]", request->uri, filepath.c_str());

    if (! context->catalog.hasFile(filepath))
        return httpd_resp_send_404(request);

    // create a chunk buffer
    std::unique_ptr<char[]> buf(new char[rest::CHUNK_SIZE]);
    if (! buf)
        return TOO_MANY_REQUESTS(request);

    // set timestamp header
    // FIXME
    // auto timestamp = context->catalog.timestamp(filepath);
    // char buffer[20];
    // rest::timestamp(timestamp, buffer);
    // httpd_resp_set_hdr(request, rest::catalog::XFILETIMESTAMP, buffer);

    auto fis = context->catalog.readContent(filepath);
    return rest::sendOctetStream(request, fis);
}

esp_err_t PUT_FILE(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto filepath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for PUT FILE [/%s]", request->uri, filepath.c_str());

    // must supply Content-Length header
    if (request->content_len <= 0)
        return httpd_resp_send_err(request, HTTPD_411_LENGTH_REQUIRED, nullptr);

    char s_timestamp[20];
    if (ESP_OK != httpd_req_get_hdr_value_str(request, rest::catalog::XFILETIMESTAMP, s_timestamp, sizeof(s_timestamp)))
        return httpd_resp_send_custom_err(request, "412 X-FileTimeStamp required", "failed");

    if (context->catalog.isLocked(filepath))
    {
        // TODO determine if caller has admin credentials
        return httpd_resp_send_err(request, HTTPD_401_UNAUTHORIZED, "folder is locked by admin");
    }

    // receive the data
    std::unique_ptr<char[]> buf(new char[rest::CHUNK_SIZE]);
    if (buf == nullptr) return TOO_MANY_REQUESTS(request);
    // auto timestamp = rest::timestamp(s_timestamp);
    // auto inwork = context->catalog.newContent(filepath, request->content_len, timestamp);
    // esp_err_t ret = ESP_OK;
    // for (size_t remaining = request->content_len; remaining > 0;)
    // {
    //     const int received = httpd_req_recv(request, buf.get(), std::min(remaining, rest::CHUNK_SIZE));
    //     if (received < 0)
    //     {
    //         ESP_LOGW(TAG, "PUT incomplete: %s [%d/%d]", request->uri,
    //                  (request->content_len - remaining), request->content_len);
    //         ret = ESP_FAIL;
    //         break;
    //     }
    //     if (false == inwork.write(buf.get(), received))
    //     {
    //         ESP_LOGE(TAG, "PUT write failed: %s [%d/%d]", request->uri,
    //                  (request->content_len - remaining), request->content_len);
    //         ret = ESP_FAIL;
    //         break;
    //     }
    //     remaining -= received;
    // }

    // if (ret == ESP_OK)
    // {
    //     // complete the file transaction
    //     inwork.done();
    //     // send an empty 200 response
    //     return httpd_resp_send(request, nullptr, 0);
    // }
    // else
    //     return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Upload failed");
    // FIXME
    return ILLEGAL_REQUEST(request);
}

esp_err_t DELETE_FILE(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto filepath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for DELETE FILE [/%s]", request->uri, filepath.c_str());

    if (! context->catalog.hasFile(filepath))
        return httpd_resp_send_404(request);

    if (context->catalog.isLocked(filepath))
    {
        // TODO determine if caller has admin credentials
        return httpd_resp_send_err(request, HTTPD_401_UNAUTHORIZED, "folder is locked by admin");
    }

    if (context->catalog.removeFile(filepath))
        return httpd_resp_sendstr(request, "OK");

    return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "folder is locked by admin");
}

esp_err_t GET_ICON(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto filepath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for PUT ICON [/%s]", request->uri, filepath.c_str());

    if (! context->catalog.hasFile(filepath))
        return httpd_resp_send_404(request);

    // create a chunk buffer
    std::unique_ptr<char[]> buf(new char[rest::CHUNK_SIZE]);
    if (! buf)
        return httpd_resp_send_custom_err(request, "408 - Too many requests", "try-again");

    auto fis = context->catalog.readIcon(filepath);
    return rest::sendOctetStream(request, fis);
}


esp_err_t PUT_ICON(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto filepath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for PUT ICON [/%s]", request->uri, filepath.c_str());

    if (! context->catalog.hasFile(filepath))
        return httpd_resp_send_404(request);

    if (context->catalog.isLocked(filepath))
    {
        // TODO determine if caller has admin credentials
        return httpd_resp_send_err(request, HTTPD_401_UNAUTHORIZED, "folder is locked by admin");
    }

    // FIXME implement
    return ESP_FAIL;
}

esp_err_t PUT_TITLE(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto filepath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for PUT ICON [/%s]", request->uri, filepath.c_str());

    if (! context->catalog.hasFile(filepath))
        return httpd_resp_send_404(request);

    if (context->catalog.isLocked(filepath))
    {
        // TODO determine if caller has admin credentials
        return httpd_resp_send_err(request, HTTPD_401_UNAUTHORIZED, "folder is locked by admin");
    }

    // FIXME implement
    return ESP_FAIL;
}
