#ifndef POLY_FILE_MANAGER_H
#define POLY_FILE_MANAGER_H

#include "serializer.h"
#include "file_manager.h"
#include "file_system_organizer.h"

#include <vector>


class PolyFileManager
{
  private:
    FileManager file_manager;

    FileSystemOrganizer file_system_orgnizer;

    Serializer serializer;

    bool repo_path_is_valid();

  public:
    PolyFileManager(const std::filesystem::path &repo_path);
    ~PolyFileManager() = default;

    std::filesystem::path get_repo_path();

    int add_file(const FilePath &path);
    int rename_file(const FilePath &old_path, const FilePath &new_path);
    int remove_file(const FilePath &path);

    int add_tag(const FileTag &tag);
    int rename_tag(const FileTag &old_tag, const FileTag &new_Tag);
    int remove_tag(const FileTag &tag);

    void assign_tag_to_file(const FilePath &path, const FileTag &tag);
    void remove_tag_from_file(const FilePath &path, const FileTag &tag);

    std::vector<FileTag> get_all_tags();
    std::vector<FilePath> get_all_paths();

    std::vector<FileTag> get_tags_for_file(const FilePath &path);
    std::vector<FilePath> get_paths_with_tag(const FileTag &tag);

    bool load(const std::filesystem::path &repo_path);

    bool save();
};

#endif