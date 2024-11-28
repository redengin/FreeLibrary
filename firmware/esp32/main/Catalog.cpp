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


