#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "common_types.hpp"
#include "tool.hpp"
#include <filesystem>
#include <set>
#include <string>

#ifdef _WIN32
#include "windows_shortcut_creator.hpp"
#endif

class FileManager
{
  private:
    FilePath repo_path_;

  public:
    FileManager() = default;
    FileManager(const FilePath &path) : repo_path_(path) {}
    FilePath get_repo_path() const noexcept { return repo_path_; }

    void set_repo_path(const FilePath &path) noexcept { repo_path_ = path; }

    bool create_category_dir(const std::string &category_name) const
    {
        return std::filesystem::create_directory(repo_path_ / category_name);
    }

    /// @returns The number of removed files, including the folder
    uintmax_t remove_category_dir(const std::string &category_name) const
    {
        return std::filesystem::remove_all(repo_path_ / category_name);
    }

    bool rename_category_dir(const std::string &old_name, const std::string &new_name) const;

    bool create_symlink_in_category(const std::string &category_name, const FilePath &path) const;

    bool remove_symlink_in_category(const std::string &category_name, const FilePath &path) const
    {
        return std::filesystem::remove(Tool::get_symlink_path(repo_path_ ,category_name, path));
    }


    /// @returns All filenames in the category folder
    std::set<std::string> get_symlink_names_in_category(const std::string &category_name) const;

    std::set<FileTag> get_category_dir_names_in_repo() const;
};

#endif // FILE_SYSTEM_ORGANIZER_H