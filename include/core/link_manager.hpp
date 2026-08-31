#ifndef LINK_MANAGER_H
#define LINK_MANAGER_H

#include <filesystem>
#include <optional>
#include <string>

/**
 * @brief Unified interface for creating and managing filesystem links.
 *
 * On Windows this produces a .lnk shortcut through the Shell Link COM API.
 * On other platforms it falls back to a POSIX symbolic link. The module hides
 * that platform difference behind one interface so upper layers only need to
 * know how to create, remove and test a link.
 */
class LinkManager
{
  public:
    /// Create a link at link_path pointing to target_path. An existing link at
    /// link_path is replaced. Returns true on success.
    static bool create(const std::filesystem::path &target_path, const std::filesystem::path &link_path);

    /// Remove the link at link_path. Returns true if a link was actually removed.
    static bool remove(const std::filesystem::path &link_path);

    /// Whether a link exists at link_path.
    static bool exists(const std::filesystem::path &link_path);

    /// Resolve the target that a link points to. Returns nullopt if the link
    /// cannot be resolved (e.g. missing or unreadable).
    [[nodiscard]] static std::optional<std::filesystem::path> resolve(const std::filesystem::path &link_path);

    /// Whether creating a link is supported for the given target on this platform.
    static bool is_supported(const std::filesystem::path &target_path);

    /// Compute the link path for a target inside a category directory, e.g.
    /// `<repo>/<category>/<target>.lnk` on Windows and `<repo>/<category>/<target>`
    /// on POSIX platforms.
    static std::filesystem::path link_path(const std::filesystem::path &repo,
                                           const std::string &category,
                                           const std::filesystem::path &target_path);
};

#endif // LINK_MANAGER_H
