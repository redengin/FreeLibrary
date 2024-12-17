#pragma once

#include <esp_http_server.h>
#include <esp_err.h>
#include <filesystem>
#include <fstream>

namespace rest {

static constexpr char TAG[] = "rest::utils"; ///< ESP logging tag

/// replaces HTTP tokens inplace
void httpDecode(std::string& encoded);

/// timestamp format
static constexpr char ISO_8601_Z_FORMAT[] = "%FT%TZ";
std::string timestamp(const std::filesystem::file_time_type& timestamp);
std::filesystem::file_time_type timestamp(const char* const s, const std::size_t sz);

// common HTTP request handlers
esp_err_t ILLEGAL_REQUEST(httpd_req_t* request);
esp_err_t TOO_MANY_REQUESTS(httpd_req_t* request);

static constexpr std::size_t CHUNK_SIZE = 1450;
/// @brief send chunked HTTP stream
esp_err_t sendOctetStream(
    httpd_req_t* const request,
    std::ifstream& fis
);
/// @brief receive chunked HTTP stream
/// @post upon success, caller provides the response
bool receiveOctetStream(
    httpd_req_t* const request,
    std::ofstream& fos
);

};
