#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "common_types.h"
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <vector>

class FileManager
{
  private:
    // 主注册表
    std::map<FileTag, FileTagPtr> tag_registry_;    // 存储所有标签
    std::map<FilePath, FilePathPtr> path_registry_; // 存储所有路径

    // 关联映射表
    std::map<FileTagPtr, std::set<FilePathPtr>> tag_to_paths_map_; // 标签到路径的映射
    std::map<FilePathPtr, std::set<FileTagPtr>> path_to_tags_map_; // 路径到标签的映射

  public:
    FileManager() = default;
    FileManager(const FileTagData &data);
    ~FileManager() = default;

    // 标签管理
    void create_tag(const FileTag &tag);
    void rename_tag(const FileTag &old_tag, const FileTag &new_tag);
    void remove_tag(const FileTag &tag);
    std::vector<FileTag> get_all_tags() const;

    // 路径管理
    void create_path(const FilePath &path);
    void rename_path(const FilePath &old_path, const FilePath &new_path);
    void remove_path(const FilePath &path);
    std::vector<FilePath> get_all_paths() const;

    // 标签-路径关联管理
    void assign_tag_to_path(const FilePath &path, const FileTag &tag);
    void remove_tag_from_path(const FilePath &path, const FileTag &tag);

    // 查询操作
    std::set<FileTag> get_tags_for_path(const FilePath &path) const;
    std::set<FilePath> get_paths_with_tag(const FileTag &tag) const;

    FileTagData get_file_tag_data() const;

    // 调试输出
    void dump_tags() const;
    void dump_tag_assignments() const;
    void dump_paths() const;
    void dump_path_tags() const;

  private:
    // 内部辅助函数
    FileTagPtr get_or_create_tag_ptr(const FileTag &tag);
    FilePathPtr get_or_create_path_ptr(const FilePath &path);
    FileTagPtr find_tag_ptr(const FileTag &tag) const;
    FilePathPtr find_path_ptr(const FilePath &path) const;
};

#endif // FILE_MANAGER_H