#include "utils.hpp"

#include <esp_log.h>
#include <memory>

esp_err_t rest::sendOctetStream(httpd_req_t* const request, std::ifstream& fis)
{
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(rest::CHUNK_SIZE);
    if (! buffer)
        return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Too many requests (try again)");

    // set the response mime type
    httpd_resp_set_type(request, "application/octet-stream");

    // send the data
    do {
        const size_t sz = fis.readsome(buffer.get(), rest::CHUNK_SIZE);
        if (ESP_OK != httpd_resp_send_chunk(request, buffer.get(), sz))
            // file transfer interrupted
            return ESP_FAIL;

        // send is complete once we've sent a 0 length chunk
        if (sz == 0)
            return ESP_OK;

    } while(true);
}
