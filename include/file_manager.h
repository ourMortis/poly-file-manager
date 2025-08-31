#pragma once;

#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <set>
#include <memory>

using file_path = std::shared_ptr<std::filesystem::path>;
using file_tag = std::shared_ptr<std::string>;

class FileManager
{
public:
    FileManager();
    ~FileManager();
    void create_tag(const std::string &tag);
    void change_tag(const std::string &old_tag, const std::string &new_tag);
    void delete_tag(const std::string &tag);

    void create_path(const std::filesystem::path &path);
    void change_path(const std::filesystem::path &old_path, const std::filesystem::path &new_path);
    void delete_path(const std::filesystem::path &path);
    
    

private: 
    std::set<file_tag>tags;
    std::set<file_path> paths;
    std::unordered_map<file_path, std::set<file_tag>> path_with_tags;
    std::unordered_map<file_tag, std::set<file_path>> tag_with_paths;
};