#include "file_manager.hpp"
#include <cmath>
#include <filesystem>

FileManager::FileManager(const FilePath &path)
{
    if (!path.is_absolute() || !std::filesystem::is_directory(path))
    {
        throw std::invalid_argument("Path is not absolute or not a dirctory: " + path.string());
    }
    repo_path_ = path;
}

FilePath FileManager::get_symlink_path(const std::string &category_name, const FilePath &target_path)
{
#ifdef _WIN32
    FilePath symlink_path = repo_path_ / category_name /
                            (target_path.filename().empty() ? target_path.parent_path().filename().string() + ".lnk"
                                                            : target_path.filename().string() + ".lnk");
#else
    FilePath symlink_path =
        repo_path_ / category_name /
        (target_path.filename().empty() ? target_path.parent_path().filename() : target_path.filename());
#endif
    return symlink_path;
}

void FileManager::create_category_dirs(const std::vector<std::string> &category_names)
{
    for (const auto &name : category_names)
    {
        if (!std::filesystem::create_directory(repo_path_ / name))
        {
            throw std::runtime_error("Failed to create directory '" + (repo_path_ / name).string());
        }
    }
}

int FileManager::remove_category_dir(const std::string &category_name)
{
    return std::filesystem::remove_all(repo_path_ / category_name);
}

bool FileManager::rename_category_dir(const std::string &old_name, const std::string &new_name)
{
    std::filesystem::rename(repo_path_ / old_name, repo_path_ / new_name);
    if (std::filesystem::is_directory(repo_path_ / new_name) && !std::filesystem::exists(repo_path_ / old_name))
    {
        return true;
    }
    return false;
}

int FileManager::create_symlink_in_category(const std::string &category_name,
                                                     const std::vector<FilePath> &paths)
{
    int cnt = 0;
    for (const auto &path : paths)
    {
        if (!create_symlink_in_category(category_name, path))
        {
            return cnt;
        }
        cnt++;
    }
    return cnt;
}

bool FileManager::create_symlink_in_category(const std::string &category_name, const FilePath &path)
{
#ifdef _WIN32
    ShortcutCreator creator;
    return creator.create(path, get_symlink_path(category_name, path));
#else
    std::filesystem::create_directory_symlink(path, get_symlink_path(category_name, path));
    return true;
#endif
}

int FileManager::remove_symlink_in_category(const std::string &category_name,
                                                     const std::vector<FilePath> &paths)
{
    int cnt = 0;
    for (auto path : paths)
    {
        if (!remove_symlink_in_category(category_name, path))
        {
            return cnt;
        }
        cnt++;
    }
    return cnt;
}

bool FileManager::remove_symlink_in_category(const std::string &category_name, const FilePath &path)
{
    return std::filesystem::remove(get_symlink_path(category_name, path));
}

FilePath FileManager::get_repo_path() const { return repo_path_; }

std::vector<FilePath> FileManager::get_symlinks_in_category(const std::string &category_name)
{
    std::vector<FilePath> paths;
    for (const auto &entry : std::filesystem::directory_iterator(repo_path_ / category_name))
    {
        paths.push_back(entry.path());
    }
    return paths;
}