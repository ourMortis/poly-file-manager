#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "common_types.hpp"
#include <filesystem>
#include <string>
#include <vector>
#include <set>

#ifdef _WIN32
#include "windows_shortcut_creator.hpp"
#endif

class FileManager
{
  private:
    
    FilePath repo_path_; // Absolute path of the repository

  public:
    FileManager(const FilePath &path);

    //bool create_repo(const FilePath &repo_path);

    bool create_category_dirs(const std::vector<std::string> &category_names) const;
    /// @returns The number of removed files, including the folder
    int remove_category_dir(const std::string &category_name) const;
    bool rename_category_dir(const std::string &old_name, const std::string &new_name) const;

    /// @returns The number of successfully created symbolic links
    int create_symlink_in_category(const std::string &category_name, const std::vector<FilePath> &paths) const;
    bool create_symlink_in_category(const std::string &category_name, const FilePath &path) const;
    /// @returns The number of successfully removed symbolic links
    int remove_symlink_in_category(const std::string &category_name, const std::vector<FilePath> &paths) const;
    bool remove_symlink_in_category(const std::string &category_name, const FilePath &path) const;

    FilePath get_repo_path() const noexcept;
    /// @returns The absolute path of the symbolic link, or the .lnk shortcut path on Windows
    FilePath get_symlink_path(const std::string &category_name, const FilePath &target_path) const noexcept;
    /// @returns All paths in the category folder
    std::set<std::string> get_symlink_names_in_category(const std::string &category_name) const;
    std::set<FileTag> get_category_dir_names_in_repo() const;
};

#endif // FILE_SYSTEM_ORGANIZER_H