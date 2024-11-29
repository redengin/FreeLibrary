#include "Catalog.hpp"

#include <esp_log.h>

Catalog::Catalog(const std::filesystem::path _root)
    : root(_root)
{
    // cleanup incomplete temporary files
    for(const auto& entry : std::filesystem::recursive_directory_iterator(root))
    {
        const auto filename = entry.path().filename();
        if (filename.string().starts_with(INWORK_PREFIX))
        {
            ESP_LOGI(TAG, "removing INWORK %s", entry.path().c_str());
            std::error_code ec;
            std::filesystem::remove(entry, ec);
            if (ec)
            {
                ESP_LOGW(TAG, "failed to remove %s [%s]"
                     ,entry.path().c_str()
                     ,ec.message().c_str()
                );
            }
        }
    }
}

bool Catalog::exists(const std::string& filepath) const
{
    const std::filesystem::path path = root/filepath;

    // hide internal files
    if (*(path.filename().c_str()) == '.')
        return false;

    return std::filesystem::exists(path);
}

std::time_t Catalog::timestamp(const std::string& filepath) const
{
    const std::filesystem::path path{root/filepath};

    std::error_code ec;
    auto timestamp = std::filesystem::last_write_time(path, ec);
    if (ec)
        ESP_LOGW(TAG, "last_write_time() failed for \"%s\" [%s]", path.c_str(), ec.message().c_str());

    return timestamp.time_since_epoch().count();
}

std::ifstream Catalog::readContent(const std::string& filepath) const
{
    // // validate filename
    // if(isValidContentFilepath(filepath))
    //     return std::ifstream(root/filepath, std::ios_base::in | std::ios_base::binary);
    // else
    //     // return a null stream
        return std::ifstream();
}


