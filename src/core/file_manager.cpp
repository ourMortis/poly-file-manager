#include "file_manager.hpp"

#include <filesystem>

FileManager::FileManager(const FilePath &repo_path) : repo_path_(repo_path) {}

FileManager::FileManager(FilePath &&repo_path) : repo_path_(std::move(repo_path)) {}

FilePath FileManager::repo_path() const
{
    return repo_path_;
}

void FileManager::set_repo_path(const FilePath &repo_path)
{
    repo_path_ = repo_path;
}

bool FileManager::category_exists(const std::string &category) const
{
    return std::filesystem::is_directory(category_path(category));
}

bool FileManager::create_category(const std::string &category)
{
    std::error_code ec;
    return std::filesystem::create_directory(category_path(category), ec);
}

bool FileManager::rename_category(const std::string &old_category, const std::string &new_category) const
{
    const FilePath old_path = category_path(old_category);
    const FilePath new_path = category_path(new_category);
    if (std::filesystem::exists(new_path) || !std::filesystem::is_directory(old_path))
    {
        return false;
    }
    std::error_code ec;
    std::filesystem::rename(old_path, new_path, ec);
    return !ec && std::filesystem::is_directory(new_path) && !std::filesystem::exists(old_path);
}

std::uintmax_t FileManager::remove_category(const std::string &category) const
{
    std::error_code ec;
    return std::filesystem::remove_all(category_path(category), ec);
}

std::set<FileTag> FileManager::categories() const
{
    std::set<FileTag> result;
    std::error_code ec;
    for (const auto &entry : std::filesystem::directory_iterator(repo_path_, ec))
    {
        if (ec)
        {
            break;
        }
        if (entry.is_directory(ec))
        {
            result.insert(entry.path().filename().string());
        }
    }
    return result;
}

bool FileManager::create_link(const std::string &category, const FilePath &target_path)
{
    if (!std::filesystem::is_directory(category_path(category)))
    {
        return false;
    }
    return LinkManager::create(target_path, LinkManager::link_path(repo_path_, category, target_path));
}

bool FileManager::remove_link(const std::string &category, const FilePath &target_path) const
{
    return LinkManager::remove(LinkManager::link_path(repo_path_, category, target_path));
}

bool FileManager::link_exists(const std::string &category, const FilePath &target_path) const
{
    return LinkManager::exists(LinkManager::link_path(repo_path_, category, target_path));
}

std::set<std::string> FileManager::link_names(const std::string &category) const
{
    std::set<std::string> names;
    std::error_code ec;
    for (const auto &entry : std::filesystem::directory_iterator(category_path(category), ec))
    {
        if (ec)
        {
            break;
        }
        std::string name = entry.path().filename().string();
#ifdef _WIN32
        if (name.ends_with(".lnk"))
        {
            name.resize(name.size() - 4);
        }
#endif
        names.insert(std::move(name));
    }
    return names;
}

std::set<FilePath> FileManager::link_paths(const std::string &category) const
{
    std::set<FilePath> paths;
    std::error_code ec;
    for (const auto &entry : std::filesystem::directory_iterator(category_path(category), ec))
    {
        if (ec)
        {
            break;
        }
#ifdef _WIN32
        if (entry.is_regular_file(ec) && entry.path().extension() == ".lnk")
#else
        if (entry.is_symlink(ec))
#endif
        {
            paths.insert(entry.path());
        }
    }
    return paths;
}

FilePath FileManager::category_path(const std::string &category) const
{
    return repo_path_ / category;
}

FilePath FileManager::link_path(const std::string &category, const FilePath &target_path) const
{
    return LinkManager::link_path(repo_path_, category, target_path);
}
