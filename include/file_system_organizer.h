#ifndef FILE_SYSTEM_ORGANIZER_H
#define FILE_SYSTEM_ORGANIZER_H

#include "common_types.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#ifdef _WIN32
#include "windows_shortcut_creator.h"
#endif

class FileSystemOrganizer
{
private:
    FilePath repo_path_; // 存储仓库的绝对路径

public:
    FileSystemOrganizer(const FilePath &path);
    ~FileSystemOrganizer() = default;

    void create_category_dirs(const std::vector<std::string> &category_names);
    void remove_category_dir(const std::string &category_name);
    void rename_category_dir(const std::string &old_name, const std::string &new_name);

    void create_symlink_in_category(const std::string &category_name, const std::vector<FilePath> &paths);
    void create_symlink_in_category(const std::string &category_name, const FilePath &path);
    void remove_symlink_in_category(const std::string &category_name, const std::vector<FilePath> &paths);
    void remove_symlink_in_category(const std::string &category_name, const FilePath &path);

    FilePath get_repo_path() const;
    FilePath get_symlink_path(const std::string &category_name, const FilePath &target_path);
    std::vector<FilePath> get_symlinks_in_category(const std::string &category_name);
};

#endif // FILE_SYSTEM_ORGANIZER_H