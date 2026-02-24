#include "file_manager.hpp"
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <winnt.h>

bool FileManager::rename_category_dir(const std::string &old_name, const std::string &new_name) const
{
    if (std::filesystem::exists(repo_path_ / new_name) || !std::filesystem::is_directory(repo_path_ / old_name))
    {
        return false;
    }
    std::filesystem::rename(repo_path_ / old_name, repo_path_ / new_name);
    if (std::filesystem::is_directory(repo_path_ / new_name) && !std::filesystem::exists(repo_path_ / old_name))
    {
        return true;
    }
    return false;
}

bool FileManager::create_symlink_in_category(const std::string &category_name, const FilePath &path) const
{
    if (!std::filesystem::is_directory(repo_path_ / category_name))
    {
        return false;
    }
#ifdef _WIN32
    Tool::create_win_shortcut(path, Tool::get_symlink_path(repo_path_, category_name, path));
#else
    std::filesystem::create_symlink(path, get_symlink_path(category_name, path));
    return true;
#endif
}

std::set<std::string> FileManager::get_symlink_names_in_category(const std::string &category_name) const
{
    std::set<std::string> names;
    for (const auto &entry : std::filesystem::directory_iterator(repo_path_ / category_name))
    {
        std::string name = entry.path().filename().string();
#ifdef WIN32
        names.insert(name.substr(0, name.size() - 4));
#else
        names.insert(name);
#endif
    }
    return names;
}

std::set<FileTag> FileManager::get_category_dir_names_in_repo() const
{
    std::set<FileTag> tags;
    for (const auto &entry : std::filesystem::directory_iterator(repo_path_))
    {
        if (std::filesystem::is_directory(entry.path()))
        {
            tags.insert(entry.path().filename().string());
        }
    }
    return tags;
}