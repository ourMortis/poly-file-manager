#ifndef FILE_SYSTEM_ORGANIZER_H
#define FILE_SYSTEM_ORGANIZER_H

#include<iostream>
#include<vector>
#include<string>
#include<filesystem>

using FilePath = std::filesystem::path;

class FileSystemOrganizer
{
private:
    FilePath repo_path_; // 存储仓库的绝对路径
    
    

public:
    FileSystemOrganizer();
    ~FileSystemOrganizer();

    void create_category_dirs(const std::vector<std::string> &category_names);
    void remove_category_dir(const std::string &category_name);
    void rename_category_dir(const std::string &old_name, const std::string &new_name);

    void create_symlink_in_category(const std::string &category_name, const std::vector<FilePath> &paths);
    void create_symlink_in_category(const std::string &category_name, const FilePath &path);
    void remove_symlink_in_category(const std::string &category_name, const std::vector<FilePath> &paths);
    void remove_symlink_in_category(const std::string &category_name, const FilePath &path);

};

#endif // FILE_SYSTEM_ORGANIZER_H