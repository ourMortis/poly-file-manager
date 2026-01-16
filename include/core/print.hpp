#ifndef PRINT_H
#define PRINT_H

#include "common_types.hpp"
#include <set>
#include <iostream>

void print_tags(const std::map<FileTag, FileTagPtr>& tag_registry);
void print_tag_paths(const std::map<FileTagPtr, std::set<FilePathPtr>>& tag_to_paths_map);
void print_paths(const std::map<FilePath, FilePathPtr>& path_registry);
void print_path_tags(const std::map<FilePathPtr, std::set<FileTagPtr>>& path_to_tags_map);

#endif // PRINT_H