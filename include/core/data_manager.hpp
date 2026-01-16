#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "common_types.hpp"
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <vector>

class DataManager
{
  private:
    std::map<FileTag, FileTagPtr> tag_registry_;
    std::map<FilePath, FilePathPtr> path_registry_;

    std::map<FileTagPtr, std::set<FilePathPtr>> tag_to_paths_map_;
    std::map<FilePathPtr, std::set<FileTagPtr>> path_to_tags_map_;

    FileTagPtr get_or_create_tag_ptr(const FileTag &tag);
    FilePathPtr get_or_create_path_ptr(const FilePath &path);
    FileTagPtr find_tag_ptr(const FileTag &tag) const;
    FilePathPtr find_path_ptr(const FilePath &path) const;
  public:
    DataManager() = default;
    DataManager(const FileTagData &data);
    ~DataManager() = default;

    void create_tag(const FileTag &tag);
    void rename_tag(const FileTag &old_tag, const FileTag &new_tag);
    void remove_tag(const FileTag &tag);
    std::vector<FileTag> get_all_tags() const;
    int get_size_of_tags() const;

    void create_path(const FilePath &path);
    void rename_path(const FilePath &old_path, const FilePath &new_path);
    void remove_path(const FilePath &path);
    std::vector<FilePath> get_all_paths() const;
    int get_size_of_paths() const;

    void assign_tag_to_path(const FilePath &path, const FileTag &tag);
    void remove_tag_from_path(const FilePath &path, const FileTag &tag);

    std::set<FileTag> get_tags_for_path(const FilePath &path) const;
    std::set<FilePath> get_paths_with_tag(const FileTag &tag) const;
    int get_size_of_tags_for_path(const FilePath &path) const;
    int get_size_of_paths_with_tag(const FileTag &tag) const;

    FileTagData get_file_tag_data() const;
};

#endif // DATA_MANAGER_H