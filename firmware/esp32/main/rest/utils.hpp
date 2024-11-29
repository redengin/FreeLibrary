#pragma once

#include <esp_http_server.h>
#include <esp_err.h>
#include <fstream>

namespace rest {

/// HTTP timestamp format
static constexpr char ISO_8601_FORMAT[] = "%FT%T";

static constexpr std::size_t CHUNK_SIZE = 1048;
esp_err_t sendOctetStream(
    httpd_req_t* const request,
    std::ifstream& fis
);

};
