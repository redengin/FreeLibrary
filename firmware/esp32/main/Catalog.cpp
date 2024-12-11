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
    ESP_LOGI(TAG, "looking for title in %s", titlepath.c_str());

    std::error_code ec;
    if (std::filesystem::is_regular_file(titlepath, ec))
    {
        std::ifstream ifs(titlepath);
        std::stringstream ss; 
        ss << ifs.rdbuf();
        ESP_LOGI(TAG, "found title [%s]", ss.str().c_str());
        return ss.str();
    }

    // no title file
    return std::nullopt;
}

bool Catalog::hasIcon(const std::string& filepath) const
{
    if (isHidden(filepath))
        return false;

    auto parentpath = std::filesystem::path(filepath).parent_path();
    auto filename = std::filesystem::path(filepath).filename().string();
    auto iconfilename = filename.insert(0, ICON_PREFIX);
    auto iconpath = root/parentpath/iconfilename;
    ESP_LOGI(TAG, "looking for icon in %s", iconpath.c_str());

    std::error_code ec;
    return std::filesystem::is_regular_file(iconpath, ec);
}

bool Catalog::removeFolder(const std::string& filepath) const
{
    if (isHidden(filepath))
        return false;

    std::error_code ec;
    return std::filesystem::remove(root/filepath, ec);
}

bool Catalog::hasFile(const std::string& filepath) const
{
    if (isHidden(filepath))
        return false;

    std::error_code ec;
    return std::filesystem::is_regular_file(root/filepath, ec);
}

std::filesystem::file_time_type Catalog::timestamp(const std::string& filepath) const
{
    std::error_code ec;
    return std::filesystem::last_write_time(root/filepath, ec);
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

    auto parentpath = std::filesystem::path(filepath).parent_path();
    auto filename = std::filesystem::path(filepath).filename().string();
    auto iconfilename = filename.insert(0, ICON_PREFIX);
    auto iconpath = root/parentpath/iconfilename;
    ESP_LOGI(TAG, "looking for icon in %s", iconpath.c_str());
    return std::ifstream(iconpath, std::ios_base::in | std::ios_base::binary);
}

bool Catalog::removeFile(const std::string& filepath) const
{
    if (isHidden(filepath))
        return false;

    std::error_code ec;
    return std::filesystem::remove(root/filepath, ec);
}


// InWork
//==============================================================================
Catalog::InWorkContent::InWorkContent(const std::filesystem::path& filepath, const std::filesystem::file_time_type timestamp)
    : filepath(filepath)
     ,timestamp(timestamp)
{
    auto parentpath = filepath.parent_path();
    auto filename = filepath.filename().string();
    auto inwork_filename = filename.insert(0, INWORK_PREFIX);
    auto inwork_filepath = parentpath/inwork_filename;
    ofs = std::ofstream(inwork_filepath, std::ios_base::in | std::ios_base::binary);
}

bool Catalog::InWorkContent::write(const char buffer[], const size_t sz)
{
    ofs.write(buffer, sz);
    return ofs.good();
}

void Catalog::InWorkContent::done()
{
    ofs.close();
    std::error_code ec;
    // remove the old file
    if (std::filesystem::exists(filepath, ec))
        std::filesystem::remove(filepath, ec);
    // swap in the new file
    std::filesystem::rename(inwork_filepath, filepath, ec);

    // set the timestamp
    std::filesystem::last_write_time(filepath, timestamp, ec);
}

Catalog::InWorkContent::~InWorkContent()
{
    if (ofs.is_open())
    {
        ofs.close();
        std::error_code ec;
        std::filesystem::remove(inwork_filepath, ec);
    }
}

Catalog::InWorkContent Catalog::newContent(const std::string& filepath, const std::filesystem::file_time_type timestamp)
{
    // TODO decide if we should make space for new content
    return InWorkContent(root/filepath, timestamp);
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
