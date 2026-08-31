#include "poly_file_manager.hpp"

#include <algorithm>
#include <iterator>
#include <map>
#include <ranges>
#include <stdexcept>

PolyFileManager::PolyFileManager(const FilePath &repo_path)
    : file_manager(repo_path), serializer(repo_path, data_file_name), data_manager(serializer.load())
{
    if (!is_valid_repository(repo_path))
    {
        throw std::invalid_argument("PolyFileManager: not a valid repository");
    }
}

bool PolyFileManager::is_valid_repository(const FilePath &repo_path)
{
    return repo_path.is_absolute() && std::filesystem::is_directory(repo_path) &&
           std::filesystem::is_regular_file(repo_path / data_file_name);
}

FilePath PolyFileManager::repo_path() const
{
    return file_manager.repo_path();
}

bool PolyFileManager::create_repository(const FilePath &repo_path)
{
    if (!repo_path.is_absolute() || !std::filesystem::is_directory(repo_path))
    {
        return false;
    }
    return Serializer(repo_path, data_file_name).save(FileTagData{});
}

bool PolyFileManager::is_repository(const FilePath &repo_path)
{
    return std::filesystem::is_regular_file(repo_path / data_file_name);
}

bool PolyFileManager::destroy_repository(const FilePath &repo_path)
{
    if (!is_repository(repo_path))
    {
        return false;
    }
    std::error_code ec;
    for (auto it = std::filesystem::directory_iterator(repo_path, ec); it != std::filesystem::directory_iterator(); ++it)
    {
        if (ec)
        {
            return false;
        }
        if (it->is_directory(ec))
        {
            std::filesystem::remove_all(it->path(), ec);
        }
    }
    std::filesystem::remove(repo_path / data_file_name, ec);
    return !ec;
}

bool PolyFileManager::add_path(const FilePath &path)
{
    return path.is_absolute() && data_manager.add_path(path);
}

bool PolyFileManager::replace_path(const FilePath &old_path, const FilePath &new_path)
{
    if (!data_manager.has_path(old_path) || data_manager.has_path(new_path))
    {
        return false;
    }

    std::vector<FileTag> migrated;
    for (const auto &tag : data_manager.tags_of(old_path))
    {
        if (!file_manager.remove_link(tag, old_path) || !file_manager.create_link(tag, new_path))
        {
            restore_links(migrated, old_path, new_path);
            file_manager.create_link(tag, old_path); // re-create this tag's old link
            return false;
        }
        migrated.push_back(tag);
    }
    return data_manager.replace_path(old_path, new_path);
}

bool PolyFileManager::remove_path(const FilePath &path)
{
    if (!data_manager.has_path(path))
    {
        return false;
    }

    std::vector<FileTag> removed;
    for (const auto &tag : data_manager.tags_of(path))
    {
        if (!file_manager.remove_link(tag, path))
        {
            break;
        }
        removed.push_back(tag);
    }

    if (removed.size() != data_manager.tags_of(path).size())
    {
        for (const auto &tag : removed)
        {
            file_manager.create_link(tag, path);
        }
        return false;
    }
    return data_manager.remove_path(path);
}

bool PolyFileManager::add_tag(const FileTag &tag)
{
    if (data_manager.has_tag(tag) || !file_manager.create_category(tag))
    {
        return false;
    }
    if (!data_manager.add_tag(tag))
    {
        file_manager.remove_category(tag); // roll back the orphan directory
        return false;
    }
    return true;
}

bool PolyFileManager::rename_tag(const FileTag &old_tag, const FileTag &new_tag)
{
    if (!data_manager.has_tag(old_tag) || data_manager.has_tag(new_tag) ||
        !file_manager.rename_category(old_tag, new_tag))
    {
        return false;
    }
    return data_manager.rename_tag(old_tag, new_tag);
}

bool PolyFileManager::remove_tag(const FileTag &tag)
{
    if (!data_manager.has_tag(tag))
    {
        return false;
    }
    // The category dir holds one link per associated path plus the dir itself.
    if (file_manager.remove_category(tag) == data_manager.path_count_of(tag) + 1)
    {
        return data_manager.remove_tag(tag);
    }
    return false;
}

bool PolyFileManager::assign_tag(const FilePath &path, const FileTag &tag)
{
    if (!file_manager.create_link(tag, path))
    {
        return false;
    }
    if (!data_manager.add_association(tag, path))
    {
        file_manager.remove_link(tag, path); // roll back the created link
        return false;
    }
    return true;
}

bool PolyFileManager::unassign_tag(const FilePath &path, const FileTag &tag)
{
    if (!file_manager.remove_link(tag, path))
    {
        return false;
    }
    if (!data_manager.remove_association(tag, path))
    {
        file_manager.create_link(tag, path); // roll back the removed link
        return false;
    }
    return true;
}

std::set<FileTag> PolyFileManager::tags() const
{
    return data_manager.tags();
}

std::set<FilePath> PolyFileManager::paths() const
{
    return data_manager.paths();
}

std::set<FileTag> PolyFileManager::tags_of_file(const FilePath &path) const
{
    return data_manager.tags_of(path);
}

std::set<FilePath> PolyFileManager::paths_of_tag(const FileTag &tag) const
{
    return data_manager.paths_of(tag);
}

std::set<FilePath> PolyFileManager::paths_with_tags(const std::vector<FileTag> &tags) const
{
    if (tags.empty())
    {
        return {};
    }

    // Intersect the smallest set first so the running result shrinks quickly.
    auto ordered = tags;
    std::ranges::sort(ordered, [this](const FileTag &a, const FileTag &b) {
        return data_manager.path_count_of(a) < data_manager.path_count_of(b);
    });

    std::set<FilePath> result = data_manager.paths_of(ordered.front());
    for (auto it = std::next(ordered.begin()); it != ordered.end() && !result.empty(); ++it)
    {
        const auto current = data_manager.paths_of(*it);
        std::set<FilePath> next;
        std::ranges::set_intersection(result, current, std::inserter(next, next.end()));
        result = std::move(next);
    }
    return result;
}

std::set<FileTag> PolyFileManager::tags_with_paths(const std::vector<FilePath> &paths) const
{
    if (paths.empty())
    {
        return {};
    }

    // Intersect the smallest set first so the running result shrinks quickly.
    auto ordered = paths;
    std::ranges::sort(ordered, [this](const FilePath &a, const FilePath &b) {
        return data_manager.tag_count_of(a) < data_manager.tag_count_of(b);
    });

    std::set<FileTag> result = data_manager.tags_of(ordered.front());
    for (auto it = std::next(ordered.begin()); it != ordered.end() && !result.empty(); ++it)
    {
        const auto current = data_manager.tags_of(*it);
        std::set<FileTag> next;
        std::ranges::set_intersection(result, current, std::inserter(next, next.end()));
        result = std::move(next);
    }
    return result;
}

bool PolyFileManager::save() const
{
    return serializer.save(data_manager.to_file_tag_data());
}

FileTagData PolyFileManager::current_data() const
{
    return data_manager.to_file_tag_data();
}

bool PolyFileManager::restore_data(const FileTagData &data)
{
    data_manager = DataManager(data);
    return sync();
}

bool PolyFileManager::is_consistent() const
{
    const auto tag_set = data_manager.tags();
    const auto category_set = file_manager.categories();
    if (tag_set.size() != category_set.size())
    {
        return false;
    }
    for (const auto &tag : tag_set)
    {
        if (!category_set.contains(tag))
        {
            return false;
        }
    }
    for (const auto &tag : tag_set)
    {
        const auto target_paths = data_manager.paths_of(tag);
        const auto link_names = file_manager.link_names(tag);
        if (target_paths.size() != link_names.size())
        {
            return false;
        }
        for (const auto &path : target_paths)
        {
            if (!link_names.contains(path.filename().string()))
            {
                return false;
            }
        }
    }
    return true;
}

bool PolyFileManager::sync()
{
    const auto repo = file_manager.repo_path();
    std::error_code ec;
    for (const auto &entry : std::filesystem::directory_iterator(repo, ec))
    {
        if (ec)
        {
            return false;
        }
        if (entry.is_directory(ec))
        {
            if (std::filesystem::remove_all(entry.path(), ec) < 1)
            {
                return false;
            }
        }
    }

    const auto all_tags = data_manager.tags();
    for (const auto &tag : all_tags)
    {
        if (!file_manager.create_category(tag))
        {
            return false;
        }
    }
    for (const auto &tag : all_tags)
    {
        for (const auto &path : data_manager.paths_of(tag))
        {
            if (!file_manager.create_link(tag, path))
            {
                return false;
            }
        }
    }
    return true;
}

bool PolyFileManager::rebuild_from_repository()
{
    FileTagData rebuilt;
    std::map<FileTag, std::size_t> tag_index;
    std::size_t index = 0;

    for (const auto &category : file_manager.categories())
    {
        rebuilt.tags.push_back(category);
        tag_index[category] = index++;
    }

    for (const auto &category : file_manager.categories())
    {
        for (const auto &link : file_manager.link_paths(category))
        {
            const auto target = LinkManager::resolve(link);
            if (!target)
            {
                return false;
            }
            rebuilt.path_to_tag_indices[*target].push_back(tag_index[category]);
        }
    }

    data_manager = DataManager(rebuilt);
    return true;
}

void PolyFileManager::restore_links(const std::vector<FileTag> &tags, const FilePath &old_path, const FilePath &new_path)
{
    for (const auto &tag : tags)
    {
        file_manager.remove_link(tag, new_path);
        file_manager.create_link(tag, old_path);
    }
}
