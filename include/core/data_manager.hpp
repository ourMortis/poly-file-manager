#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include "bidirectional_map.hpp"
#include "common_types.hpp"

#include <cstddef>
#include <set>

/**
 * @brief Domain-facing wrapper around BiMap for the tag/path relation.
 *
 * The underling BiMap is a generic two-domain container; this class gives it the
 * vocabulary of the file manager: tags on one side, paths on the other, plus a
 * many-to-many relation between them. It also converts to/from the compact
 * FileTagData transfer format.
 */
class DataManager
{
  public:
    DataManager() = default;
    explicit DataManager(const FileTagData &data);

    /// @return The relation as a compact, index-based FileTagData.
    [[nodiscard]] FileTagData to_file_tag_data() const;

    // ----- Tags (left domain) -------------------------------------------------

    bool add_tag(const FileTag &tag);
    bool rename_tag(const FileTag &old_tag, const FileTag &new_tag);
    bool remove_tag(const FileTag &tag) noexcept;

    [[nodiscard]] bool has_tag(const FileTag &tag) const noexcept;
    [[nodiscard]] std::size_t tag_count() const noexcept;
    [[nodiscard]] std::set<FileTag> tags() const;

    // ----- Paths (right domain) ------------------------------------------------

    bool add_path(const FilePath &path);
    bool replace_path(const FilePath &old_path, const FilePath &new_path);
    bool remove_path(const FilePath &path) noexcept;

    [[nodiscard]] bool has_path(const FilePath &path) const noexcept;
    [[nodiscard]] std::size_t path_count() const noexcept;
    [[nodiscard]] std::set<FilePath> paths() const;

    // ----- Relations -----------------------------------------------------------

    bool add_association(const FileTag &tag, const FilePath &path);
    bool remove_association(const FileTag &tag, const FilePath &path);
    [[nodiscard]] bool has_association(const FileTag &tag, const FilePath &path) const noexcept;

    /// @return Number of paths associated with the given tag.
    [[nodiscard]] std::size_t path_count_of(const FileTag &tag) const noexcept;
    /// @return Number of tags associated with the given path.
    [[nodiscard]] std::size_t tag_count_of(const FilePath &path) const noexcept;

    /// @return All paths associated with the given tag.
    [[nodiscard]] std::set<FilePath> paths_of(const FileTag &tag) const;
    /// @return All tags associated with the given path.
    [[nodiscard]] std::set<FileTag> tags_of(const FilePath &path) const;

  private:
    BiMap<FileTag, FilePath> tag_path_;
};

#endif // DATA_MANAGER_H
