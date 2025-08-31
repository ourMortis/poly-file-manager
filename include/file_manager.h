#pragma once;

#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <set>

class FileManager
{
public:
    FileManager();

private:
    std::set<std::string> tags;
    std::set<std::filesystem::path> paths;
    std::unordered_map<std::filesystem::path, std::set<std::string>> path_with_tags;
    std::unordered_map<std::string, std::set<std::filesystem::path>> tag_with_paths;
};