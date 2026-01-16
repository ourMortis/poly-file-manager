#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "common_types.hpp"
#include <filesystem>
#include <string>
#include <vector>

#ifdef _WIN32
#include "windows_shortcut_creator.hpp"
#endif

class FileManager
{
  private:
    
    FilePath repo_path_; // Absolute path of the repository

  public:
    FileManager(const FilePath &path);
    ~FileManager() = default;

    void create_category_dirs(const std::vector<std::string> &category_names);
    /// @returns The number of removed files, including the folder
    int remove_category_dir(const std::string &category_name);
    bool rename_category_dir(const std::string &old_name, const std::string &new_name);

    /// @returns The number of successfully created symbolic links
    int create_symlink_in_category(const std::string &category_name, const std::vector<FilePath> &paths);
    bool create_symlink_in_category(const std::string &category_name, const FilePath &path);
    /// @returns The number of successfully removed symbolic links
    int remove_symlink_in_category(const std::string &category_name, const std::vector<FilePath> &paths);
    bool remove_symlink_in_category(const std::string &category_name, const FilePath &path);

    FilePath get_repo_path() const;
    /// @returns The absolute path of the symbolic link, or the .lnk shortcut path on Windows
    FilePath get_symlink_path(const std::string &category_name, const FilePath &target_path);
    /// @returns All paths in the category folder
    std::vector<FilePath> get_symlinks_in_category(const std::string &category_name);
};

#endif // FILE_SYSTEM_ORGANIZER_H