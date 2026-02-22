#ifndef DATA_MANAGER_NEW_H
#define DATA_MANAGER_NEW_H

#include "bidirectional_map.hpp"
#include "common_types.hpp"
#include <set>

class DataManagerNew
{
  private:
    BiMap<FileTag, FilePath> tag_path_map_;

  public:
    DataManagerNew() = default;
    DataManagerNew(const FileTagData &data);
    FileTagData get_file_tag_data() const;

    bool create_tag(const FileTag &tag) { return tag_path_map_.insert_T1(tag); }

    bool replace_tag(const FileTag &old_tag, const FileTag &new_tag)
    {
        return tag_path_map_.replace_T1(old_tag, new_tag);
    }

    bool remove_tag(const FileTag &tag) noexcept { return tag_path_map_.erase_T1(tag); }

    std::set<FileTag> get_all_tags() const { return tag_path_map_.get_all_T1(); }

    int count_tag() const noexcept { return tag_path_map_.count_T1(); }

    bool create_path(const FilePath &path) { return tag_path_map_.insert_T2(path); }

    bool replace_path(const FilePath &old_path, const FilePath &new_path)
    {
        return tag_path_map_.replace_T2(old_path, new_path);
    }

    bool remove_path(const FilePath &path) noexcept { return tag_path_map_.erase_T2(path); }

    std::set<FilePath> get_all_paths() const { return tag_path_map_.get_all_T2(); }

    int count_path() const noexcept { return tag_path_map_.count_T2(); }

    bool assign_tag_to_path(const FilePath &path, const FileTag &tag) { return tag_path_map_.associate(tag, path); }

    bool remove_tag_from_path(const FilePath &path, const FileTag &tag) { return tag_path_map_.unassociate(tag, path); }

    std::set<FileTag> get_tags_for_path(const FilePath &path) const { return tag_path_map_.get_associated_T2(path); }

    std::set<FilePath> get_paths_with_tag(const FileTag &tag) const { return tag_path_map_.get_associated_T1(tag); }

    int count_associated_with_path(const FilePath &path) const noexcept
    {
        return tag_path_map_.count_associated_T2(path);
    }

    int count_associated_with_tag(const FileTag &tag) const noexcept { return tag_path_map_.count_associated_T1(tag); }

    bool contains_tag(const FileTag &tag) const noexcept { return tag_path_map_.contains_T1(tag); }

    bool contains_path(const FilePath &path) const noexcept { return tag_path_map_.contains_T2(path); }

    bool contains_association(const FilePath &path, const FileTag &tag) const noexcept
    {
        return tag_path_map_.contains_association(tag, path);
    }
};

#endif // DATA_MANAGER_NEW_H