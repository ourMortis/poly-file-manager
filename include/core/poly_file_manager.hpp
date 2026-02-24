#ifndef POLY_data_manager_H
#define POLY_data_manager_H

#include "common_types.hpp"
#include "data_manager_new.hpp"
#include "file_manager.hpp"
#include "serializer.hpp"
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <vector>

class PolyFileManager
{
  private:
    FileManager file_manager;
    Serializer serializer;
    DataManager data_manager;
    static constexpr const char *data_file_name = ".poly_file_manager";

    /// @returns False if the path is an absolute path and a directory
    bool check_repo_path(const FilePath &repo_path) const
    {
        return repo_path.is_absolute() && std::filesystem::is_directory(repo_path);
    }

    PolyFileManager(const std::filesystem::path &repo_path, bool);

  public:
    explicit PolyFileManager(const std::filesystem::path &repo_path);

    std::filesystem::path get_repo_path() const noexcept { return file_manager.get_repo_path(); }

    static bool create_repository(const FilePath &repo_path);
    static bool is_repository(const FilePath &repo_path)
    {
        return std::filesystem::is_regular_file(repo_path / ".poly_file_manager");
    }

    bool add_path(const FilePath &path);
    bool rename_path(const FilePath &old_path, const FilePath &new_path);
    bool remove_path(const FilePath &path);

    bool add_tag(const FileTag &tag);
    bool rename_tag(const FileTag &old_tag, const FileTag &new_Tag);
    bool remove_tag(const FileTag &tag);

    bool assign_tag_to_file(const FilePath &path, const FileTag &tag);
    bool remove_tag_from_file(const FilePath &path, const FileTag &tag);

    std::set<FileTag> get_all_tags() const { return data_manager.get_all_tags(); }
    std::set<FilePath> get_all_paths() const { return data_manager.get_all_paths(); }

    std::vector<FileTag> get_tags_for_file(const FilePath &path) const;
    std::vector<FilePath> get_paths_with_tag(const FileTag &tag) const;

    std::set<FilePath> get_paths_with_tags(const std::vector<FileTag> &tags) const;
    std::set<FileTag> get_tags_with_paths(const std::vector<FileTag> &paths) const;

    void load(const std::filesystem::path &repo_path)
    {
        FileTagData data = serializer.deserialize_from_file();
        data_manager = DataManager(data);
    }

    bool save() const
    {
        FileTagData data = data_manager.get_file_tag_data();
        return serializer.serialize_to_file(data);
    }

    bool is_data_consistent_with_repository() const;

    bool sync_data_with_repository();
};

#endif // POLY_FILE_MANAGER_H