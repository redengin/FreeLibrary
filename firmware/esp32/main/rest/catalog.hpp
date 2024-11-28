#pragma once

#include "../WebServer.hpp"
#include "../Catalog.hpp"

namespace rest {

    void registerCatalog(WebServer& webserver, Catalog& catalog);

};