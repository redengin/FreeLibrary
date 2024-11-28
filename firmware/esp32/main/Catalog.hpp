#pragma once

#include <filesystem>

class Catalog {
public:
    static constexpr char TAG[] = "Catalog"; ///< ESP logging tag

    // magic filename prefixes
    static constexpr char INWORK_PREFIX[] = ".inwork-"; ///< tempororary used during receive

    Catalog(std::filesystem::path root);


private:
    const std::filesystem::path root;

};