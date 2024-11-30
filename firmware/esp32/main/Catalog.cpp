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

bool Catalog::hasFolder(const std::string& path) const
{
    if (isHidden(path))
        return false;

    std::error_code ec;
    return std::filesystem::is_directory(root/path, ec);
}

Catalog::FolderInfo Catalog::folderInfo(const std::string& _path) const
{
    auto path = root/_path;

    FolderInfo ret;
    // set the lock info
    ret.isLocked = isLocked(path);
    for(auto& entry : std::filesystem::directory_iterator(path))
    {
        if (isHidden(entry.path()))
            continue;
        if (entry.is_directory()) ret.subfolders.push_back(entry.path().filename());
        if (entry.is_regular_file()) 
        {
            // ret.contents.push_back(contentInfo);
            ret.contents.emplace_back(
                entry.path().filename(),
                entry.file_size(),
                entry.last_write_time().time_since_epoch().count(),
                title(entry.path()),
                hasIcon(entry.path())
            );
        }
    }

    return ret;
}

bool Catalog::hasContent(const std::string& filepath) const
{
    if (isHidden(filepath))
        return false;

    std::error_code ec;
    return std::filesystem::is_regular_file(root/filepath, ec);
}


std::ifstream Catalog::readContent(const std::string& filepath) const
{
    // FIXME
    // // validate filename
    // if(isValidContentFilepath(filepath))
    //     return std::ifstream(root/filepath, std::ios_base::in | std::ios_base::binary);
    // else
    //     // return a null stream
        return std::ifstream();
}


// PRIVATE
//==============================================================================

bool Catalog::isHidden(const std::filesystem::path& filepath)
{
    for(const auto& i : filepath)
    {
        // ignore hidden files/folders and relative paths
        if (*(i.c_str()) == HIDDEN_PREFIX)
            return false;

    }
    return true;
}

bool Catalog::isLocked(const std::filesystem::path& path) const
{
    return std::filesystem::exists(path/LOCKED_FILENAME);
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


std::string Catalog::title(const std::filesystem::path& filepath) const
{
    std::string title_filename = TITLE_PREFIX;
    title_filename.append(filepath.filename());

    auto titlePath = filepath.parent_path() / title_filename;
    if (std::filesystem::is_regular_file(titlePath))
    {
        std::ifstream ifs(titlePath);
        std::stringstream ss; 
        ss << ifs.rdbuf();
        return ss.str();
    }

    // no title file, return empty string
    return "";
}

bool Catalog::hasIcon(const std::filesystem::path& filepath) const
{
    std::string icon_filename = ICON_PREFIX;
    icon_filename.append(filepath.filename());

    auto iconPath = filepath.parent_path() / icon_filename;
    return std::filesystem::is_regular_file(iconPath);
}
