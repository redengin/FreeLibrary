#pragma once

#include "../WebServer.hpp"
#include "../Catalog.hpp"

namespace rest {

    static constexpr std::string_view uri_wildcard = "/catalog/*";
    void registerCatalog(WebServer& webserver, Catalog& catalog);

};