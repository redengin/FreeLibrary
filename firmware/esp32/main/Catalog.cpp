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

bool Catalog::hasFolder(const std::string& folderpath) const
{
    if (isHidden(folderpath))
        return false;

    std::error_code ec;
    return std::filesystem::is_directory(root/folderpath, ec);
}

std::vector<std::filesystem::directory_entry> Catalog::entries(const std::string folderpath) const
{
    std::vector<std::filesystem::directory_entry> ret;

    for (auto& entry : std::filesystem::directory_iterator(root/folderpath))
    {
        // skip over hidden entries
        if (isHidden(entry.path()))
            continue;

        ret.push_back(entry);
    }

    return ret;
}

bool Catalog::isLocked(const std::string& folderpath) const
{
    if (isHidden(folderpath))
        return true;

    // TODO assert that parents are not locked

    std::error_code ec;
    return std::filesystem::is_regular_file(root/folderpath/LOCKED_FILENAME, ec);
}

std::optional<std::string> Catalog::getTitle(const std::string& filepath) const
{
    if (isHidden(filepath))
        return std::nullopt;

    auto parentpath = std::filesystem::path(filepath).parent_path();
    auto filename = std::filesystem::path(filepath).filename().string();
    auto titlefilename = filename.insert(0, TITLE_PREFIX);
    auto titlepath = root/parentpath/titlefilename;
    if (std::filesystem::is_regular_file(titlepath))
    {
        std::ifstream ifs(titlepath);
        std::stringstream ss; 
        ss << ifs.rdbuf();
        return ss.str();
    }

    // no title file
    return std::nullopt;
}

bool Catalog::hasIcon(const std::string& filepath) const
{
    if (isHidden(filepath))
        return false;

    // TODO
    return false;
}

bool Catalog::removeFolder(const std::string& filepath) const
{
    if (isHidden(filepath))
        return false;

    // TODO
    return false;
}


bool Catalog::hasFile(const std::string& filepath) const
{
    if (isHidden(filepath))
        return false;

    std::error_code ec;
    return std::filesystem::is_regular_file(root/filepath, ec);
}

time_t Catalog::timestamp(const std::string& filepath) const
{
    // FIXME
    return 0;
}

std::ifstream Catalog::readContent(const std::string& filepath) const
{
    if (isHidden(filepath))
        // return a null stream
        return std::ifstream();

    return std::ifstream(root/filepath, std::ios_base::in | std::ios_base::binary);
}

std::ifstream Catalog::readIcon(const std::string& filepath) const
{
    if (isHidden(filepath))
        // return a null stream
        return std::ifstream();

    // TODO return fis of icon
    return std::ifstream();
    // return std::ifstream(root/filepath, std::ios_base::in | std::ios_base::binary);
}

bool Catalog::removeFile(const std::string& filepath) const
{
    if (isHidden(filepath))
        return false;

    std::error_code ec;
    return std::filesystem::remove(root/filepath, ec);
}

// PRIVATE
//==============================================================================

bool Catalog::isHidden(const std::filesystem::path& filepath)
{
    for(const auto& i : filepath)
    {
        // ignore hidden files/folders and relative paths
        if (*(i.c_str()) == HIDDEN_PREFIX)
            return true;

    }
    return false;
}
