#include "print.hpp"

void print_tags(const std::map<FileTag, FileTagPtr>& tag_registry)
{
    std::cout << "=========== Tags Registry ===========\n";
    for (const auto &[tag, ptr] : tag_registry)
    {
        std::cout << "Tag: \"" << tag << "\" [Ptr: " << ptr << "]\n";
    }
    std::cout << "=====================================\n";
}

void print_tag_paths(const std::map<FileTagPtr, std::set<FilePathPtr>>& tag_to_paths_map)
{
    std::cout << "======= Tag to Path Assignments =======\n";
    for (const auto &[tag_ptr, path_set] : tag_to_paths_map)
    {
        std::cout << "Tag: \"" << *tag_ptr << "\" -> Paths: ";
        for (const auto &path_ptr : path_set)
        {
            std::cout << path_ptr->string() << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << "=======================================\n";
}

void print_paths(const std::map<FilePath, FilePathPtr>& path_registry)
{
    std::cout << "========== Paths Registry ==========\n";
    for (const auto &[path, ptr] : path_registry)
    {
        std::cout << "Path: \"" << path.string() << "\" [Ptr: " << ptr << "]\n";
    }
    std::cout << "====================================\n";
}

void print_path_tags(const std::map<FilePathPtr, std::set<FileTagPtr>>& path_to_tags_map)
{
    std::cout << "======= Path to Tag Assignments =======\n";
    for (const auto &[path_ptr, tag_set] : path_to_tags_map)
    {
        std::cout << "Path: \"" << path_ptr->string() << "\" -> Tags: ";
        for (const auto &tag_ptr : tag_set)
        {
            std::cout << *tag_ptr << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << "=======================================\n";
}