#ifndef POLY_FILE_MANAGER_H
#define POLY_FILE_MANAGER_H

#include "common_types.hpp"
#include "data_manager.hpp"
#include "file_manager.hpp"
#include "serializer.hpp"

#include <filesystem>
#include <set>
#include <string>
#include <vector>

/**
 * @brief Facade tying together the repository (FileManager), the relation data
 * (DataManager) and the persistence (Serializer).
 *
 * Every mutating operation keeps the on-disk category/link layout and the
 * in-memory tag/path relation in lock-step, rolling back either side when a
 * step fails. Queries are delegated to DataManager and, for multi-key lookups,
 * use a smallest-first intersection to cut the work down quickly.
 */
class PolyFileManager
{
  public:
    explicit PolyFileManager(const FilePath &repo_path);

    [[nodiscard]] FilePath repo_path() const;

    static bool create_repository(const FilePath &repo_path);
    [[nodiscard]] static bool is_repository(const FilePath &repo_path);
    static bool destroy_repository(const FilePath &repo_path);

    // ----- Paths ---------------------------------------------------------------

    bool add_path(const FilePath &path);
    bool replace_path(const FilePath &old_path, const FilePath &new_path);
    bool remove_path(const FilePath &path);

    // ----- Tags ----------------------------------------------------------------

    bool add_tag(const FileTag &tag);
    bool rename_tag(const FileTag &old_tag, const FileTag &new_tag);
    bool remove_tag(const FileTag &tag);

    // ----- Associations ---------------------------------------------------------

    bool assign_tag(const FilePath &path, const FileTag &tag);
    bool unassign_tag(const FilePath &path, const FileTag &tag);

    // ----- Queries --------------------------------------------------------------

    [[nodiscard]] std::set<FileTag> tags() const;
    [[nodiscard]] std::set<FilePath> paths() const;
    [[nodiscard]] std::set<FileTag> tags_of_file(const FilePath &path) const;
    [[nodiscard]] std::set<FilePath> paths_of_tag(const FileTag &tag) const;
    [[nodiscard]] std::set<FilePath> paths_with_tags(const std::vector<FileTag> &tags) const;
    [[nodiscard]] std::set<FileTag> tags_with_paths(const std::vector<FilePath> &paths) const;

    // ----- Persistence / consistency -------------------------------------------

    bool save() const;
    [[nodiscard]] FileTagData current_data() const;
    bool restore_data(const FileTagData &data);
    [[nodiscard]] bool is_consistent() const;
    bool sync();
    bool rebuild_from_repository();

  private:
    static constexpr const char *data_file_name = ".poly_file_manager";

    FileManager file_manager;
    Serializer serializer;
    DataManager data_manager;

    static bool is_valid_repository(const FilePath &repo_path);
    void restore_links(const std::vector<FileTag> &tags, const FilePath &old_path, const FilePath &new_path);
};

#endif // POLY_FILE_MANAGER_H
