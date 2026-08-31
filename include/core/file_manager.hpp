#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "common_types.hpp"
#include "link_manager.hpp"
#include <cstdint>
#include <filesystem>
#include <set>
#include <string>

/**
 * @brief Manages the repository directory structure: one category folder per
 * tag and, inside each folder, one link (Windows .lnk shortcut or POSIX
 * symlink) per associated file path.
 *
 * All link creation/removal is delegated to LinkManager; this class only adds
 * the repository-scoped folder layout and the category/filename conventions.
 */
class FileManager
{
  public:
    explicit FileManager(const FilePath &repo_path);
    explicit FileManager(FilePath &&repo_path);

    FileManager(const FileManager &) = default;
    FileManager &operator=(const FileManager &) = default;
    FileManager(FileManager &&) noexcept = default;
    FileManager &operator=(FileManager &&) noexcept = default;

    [[nodiscard]] FilePath repo_path() const;
    void set_repo_path(const FilePath &repo_path);

    // ----- Category (tag directory) management --------------------------------

    [[nodiscard]] bool category_exists(const std::string &category) const;
    bool create_category(const std::string &category);
    bool rename_category(const std::string &old_category, const std::string &new_category) const;
    std::uintmax_t remove_category(const std::string &category) const;
    [[nodiscard]] std::set<FileTag> categories() const;

    // ----- Link (shortcut / symlink) management -------------------------------

    bool create_link(const std::string &category, const FilePath &target_path);
    bool remove_link(const std::string &category, const FilePath &target_path) const;
    [[nodiscard]] bool link_exists(const std::string &category, const FilePath &target_path) const;

    /// @return The target file names referenced inside a category, i.e. the
    ///         link names with the ".lnk" suffix removed on Windows.
    [[nodiscard]] std::set<std::string> link_names(const std::string &category) const;

    /// @return The full paths of every link inside a category directory
    ///         (.lnk files on Windows, symlinks elsewhere).
    [[nodiscard]] std::set<FilePath> link_paths(const std::string &category) const;

    // ----- Path helpers --------------------------------------------------------

    [[nodiscard]] FilePath category_path(const std::string &category) const;
    [[nodiscard]] FilePath link_path(const std::string &category, const FilePath &target_path) const;

  private:
    FilePath repo_path_;
};

#endif // FILE_MANAGER_H
