#ifndef POLY_data_manager_H
#define POLY_data_manager_H

#include "common_types.hpp"
#include "serializer.hpp"
#include "data_manager.hpp"
#include "file_manager.hpp"
#include <vector>

class PolyFileManager
{
  private:
    DataManager data_manager;

    FileManager file_manager;

    Serializer serializer;

    /// @returns False if the path is an absolute path and a directory
    bool repo_path_invalid(const FilePath& repo_path) const;

    PolyFileManager(const std::filesystem::path &repo_path, bool);

  public:
    explicit PolyFileManager(const std::filesystem::path &repo_path);
    ~PolyFileManager() = default;

    std::filesystem::path get_repo_path() const;

    void add_path(const FilePath &path);
    /// @returns The number of renamed symbolic links or shortcuts on Windows
    int rename_path(const FilePath &old_path, const FilePath &new_path);
    /// @returns The number of removed symbolic links or shortcuts on Windows
    int remove_path(const FilePath &path);

    void add_tag(const FileTag &tag);
    bool rename_tag(const FileTag &old_tag, const FileTag &new_Tag);
    /// @returns The number of removed files including the directory
    int remove_tag(const FileTag &tag);

    bool assign_tag_to_file(const FilePath &path, const FileTag &tag);
    bool remove_tag_from_file(const FilePath &path, const FileTag &tag);

    std::vector<FileTag> get_all_tags() const;
    std::vector<FilePath> get_all_paths() const;

    std::vector<FileTag> get_tags_for_file(const FilePath &path) const;
    std::vector<FilePath> get_paths_with_tag(const FileTag &tag) const;

    void load(const std::filesystem::path &repo_path);

    bool save() const;
};

#endif // POLY_FILE_MANAGER_H