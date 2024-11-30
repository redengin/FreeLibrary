#pragma once

#include <filesystem>
#include <vector>
#include <optional>
#include <fstream>

class Catalog {
public:
    static constexpr char TAG[] = "Catalog"; ///< ESP logging tag

    Catalog(std::filesystem::path root);

    /// @returns true if folder exists
    bool hasFolder(const std::string& path) const;

    typedef struct {
        std::string name;
        std::size_t size;
        std::time_t timestamp;
        std::optional<std::string> title;
        bool hasIcon;
    } ContentInfo;
    typedef struct {
        bool isLocked;
        std::vector<std::string> subfolders;
        std::vector<ContentInfo> contents;
    } FolderInfo;
    FolderInfo folderInfo(const std::string& path) const;

    /// @returns true if content exists
    bool hasContent(const std::string& filepath) const;

    std::ifstream readContent(const std::string& filepath) const;

private:
    const std::filesystem::path root;

    /// hidden file/folder prefix
    static constexpr char HIDDEN_PREFIX = '.';
    ///< tempororary filename used during receive
    static constexpr char INWORK_PREFIX[] = ".inwork-";
    /// file specifying the folder is locked
    static constexpr char LOCKED_FILENAME[] = ".locked";
    /// file for the icon (name follows prefix)
    static constexpr char ICON_PREFIX[] = ".icon-";
    /// file for the title (name follows prefix)
    static constexpr char TITLE_PREFIX[] = ".title-";

    static bool isHidden(const std::filesystem::path& path);
    bool isLocked(const std::filesystem::path& path) const;
    std::time_t timestamp(const std::string& filepath) const;
    std::string title(const std::filesystem::path& filepath) const;
    bool hasIcon(const std::filesystem::path& filepath) const;
};