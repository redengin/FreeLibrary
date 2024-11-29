#pragma once

#include <filesystem>
#include <fstream>

class Catalog {
public:
    static constexpr char TAG[] = "Catalog"; ///< ESP logging tag

    // magic filename prefixes
    static constexpr char INWORK_PREFIX[] = ".inwork-"; ///< tempororary used during receive

    Catalog(std::filesystem::path root);

    bool exists(const std::string& filepath) const;

    std::time_t timestamp(const std::string& filepath) const;

    std::ifstream readContent(const std::string& filepath) const;


    // bool isLocked(const std::filesystem::path path) const;

    // static bool isValidContentFilepath(const std::string filepath);

private:
    const std::filesystem::path root;

};