#pragma once

#include <filesystem>
#include <vector>
#include <optional>
#include <fstream>

class Catalog {
public:
    static constexpr char TAG[] = "Catalog"; ///< ESP logging tag

    Catalog(std::filesystem::path root);

// Folder support
//------------------------------------------------------------------------------
    /// @returns true if folder exists
    bool hasFolder(const std::string& folderpath) const;

    std::vector<std::filesystem::directory_entry> entries(const std::string folderpath) const;

    /// @returns true if folder is admin only
    bool isLocked(const std::string& folderpath) const;

    std::optional<std::string> getTitle(const std::string& filepath) const;

    bool hasIcon(const std::string& filepath) const;

    /// @brief folder must be empty to allow removal
    bool removeFolder(const std::string& filepath) const;
//------------------------------------------------------------------------------

// File support
//------------------------------------------------------------------------------
    /// @returns true if content exists
    bool hasFile(const std::string& filepath) const;

    std::filesystem::file_time_type timestamp(const std::string& filepath) const;

    std::ifstream readContent(const std::string& filepath) const;

    std::ifstream readIcon(const std::string& filepath) const;

    bool removeFile(const std::string& filepath) const;
//------------------------------------------------------------------------------

// Upload Support
//------------------------------------------------------------------------------
    class InWorkContent {
    public:
        bool write(const char buffer[], const size_t sz);
        /// @brief swaps original file (if exists) with new file
        void done();

        /// @brief  cleans up inwork content
        ~InWorkContent();

    // FIXME provide access to newContent
    // protected:
        InWorkContent(
            const std::filesystem::path& filepath,
            const std::filesystem::file_time_type timestamp
        );

    private:
        const std::filesystem::path& filepath;
        const std::filesystem::path inwork_filepath;
        const std::filesystem::file_time_type timestamp;
        std::ofstream ofs;
    };
    /// start a new content transfer
    InWorkContent newContent(const std::string& filepath, const std::filesystem::file_time_type timestamp = {});
//------------------------------------------------------------------------------

private:
    const std::filesystem::path root;

    /// hidden file/folder prefix
    static constexpr char HIDDEN_PREFIX = '.';
    /// file specifying the folder is locked
    static constexpr char LOCKED_FILENAME[] = ".locked";
    /// file for the icon (name follows prefix)
    static constexpr char ICON_PREFIX[] = ".icon-";
    /// file for the title (name follows prefix)
    static constexpr char TITLE_PREFIX[] = ".title-";
    ///< tempororary filename used during receive
    static constexpr char INWORK_PREFIX[] = ".inwork-";

    static bool isHidden(const std::filesystem::path& path);
};