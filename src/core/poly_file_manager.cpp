#include "poly_file_manager.hpp"
#include "data_manager_new.hpp"

PolyFileManager::PolyFileManager(const std::filesystem::path &repo_path, bool)
    : file_manager(repo_path), serializer(repo_path, data_file_name), data_manager(serializer.deserialize_from_file())
{
    data_manager = DataManager(serializer.deserialize_from_file());
}

PolyFileManager::PolyFileManager(const std::filesystem::path &repo_path) : PolyFileManager(repo_path, false)
{
    if (!check_repo_path(repo_path))
    {
        throw std::invalid_argument("PolyFileManager: invalid repository path");
    }
    if (!std::filesystem::is_regular_file(repo_path / data_file_name))
    {
        throw std::invalid_argument("PolyFileManager: not a repository, create repository first");
    }
}

bool PolyFileManager::create_repository(const FilePath &repo_path)
{
    Serializer s = Serializer(repo_path, data_file_name);
    return s.serialize_to_file({});
}

bool PolyFileManager::add_path(const FilePath &path)
{
    if (path.is_relative() || data_manager.contains_path(path))
    {
        return false;
    }
    return data_manager.create_path(path);
}

bool PolyFileManager::rename_path(const FilePath &old_path, const FilePath &new_path)
{
    if (!data_manager.contains_path(old_path) || data_manager.contains_path(new_path))
    {
        return false;
    }

    std::set<FileTag> removed, created;

    for (const auto &tag : data_manager.get_tags_for_path(old_path))
    {
        if (file_manager.remove_symlink_in_category(tag, old_path))
        {
            removed.insert(tag);
        }
        else
        {
            break;
        }
        if (file_manager.create_symlink_in_category(tag, new_path))
        {
            created.insert(tag);
        }
        else
        {
            break;
        }
    }
    if (removed.size() == data_manager.count_associated_with_path(old_path) && created.size() == removed.size())
    {
        data_manager.replace_path(old_path, new_path);
        return true;
    }

    for (const auto &tag : removed)
    {
        data_manager.remove_tag_from_path(old_path, tag);
    }
    for (const auto &tag : created)
    {
        data_manager.assign_tag_to_path(new_path, tag);
    }
    return false;
}

bool PolyFileManager::remove_path(const FilePath &path)
{
    if (!data_manager.contains_path(path))
    {
        return false;
    }
    int num_of_tags = data_manager.count_associated_with_path(path);
    if (num_of_tags == 0)
    {
        data_manager.remove_path(path);
        return true;
    }
    const auto &tags = data_manager.get_tags_for_path(path);
    std::set<FileTag> removed;
    for (const auto &tag : tags)
    {
        if (file_manager.remove_symlink_in_category(tag, path))
        {
            removed.insert(tag);
        }
        else
        {
            break;
        }
    }
    if (removed.size() == data_manager.count_associated_with_path(path))
    {
        data_manager.remove_path(path);
        return true;
    }

    for (const auto &tag : removed)
    {
        data_manager.remove_tag_from_path(path, tag);
    }
    return false;
}

bool PolyFileManager::add_tag(const FileTag &tag)
{
    if (data_manager.contains_tag(tag) || !file_manager.create_category_dir(tag))
    {
        return false;
    }
    data_manager.create_tag(tag);
    return true;
}

bool PolyFileManager::rename_tag(const FileTag &old_tag, const FileTag &new_tag)
{
    if (!data_manager.contains_tag(old_tag) || data_manager.contains_tag(new_tag))
    {
        return false;
    }
    if (file_manager.rename_category_dir(old_tag, new_tag))
    {
        return data_manager.replace_tag(old_tag, new_tag);
    }
    return false;
}

bool PolyFileManager::remove_tag(const FileTag &tag)
{
    if (!data_manager.contains_tag(tag))
    {
        return false;
    }
    if (file_manager.remove_category_dir(tag) == data_manager.count_associated_with_tag(tag) + 1)
    {
        data_manager.remove_tag(tag);
        return true;
    }
    return false;
}

bool PolyFileManager::assign_tag_to_file(const FilePath &path, const FileTag &tag)
{
    if (file_manager.create_symlink_in_category(tag, path))
    {
        data_manager.assign_tag_to_path(path, tag);
        return true;
    }
    return false;
}

bool PolyFileManager::remove_tag_from_file(const FilePath &path, const FileTag &tag)
{

    if (file_manager.remove_symlink_in_category(tag, path))
    {
        data_manager.remove_tag_from_path(path, tag);
        return true;
    }
    return false;
}

std::vector<FileTag> PolyFileManager::get_tags_for_file(const FilePath &path) const
{
    auto tag_set = data_manager.get_tags_for_path(path);
    return std::vector<FileTag>(tag_set.begin(), tag_set.end());
}
std::vector<FilePath> PolyFileManager::get_paths_with_tag(const FileTag &tag) const
{
    auto path_set = data_manager.get_paths_with_tag(tag);
    return std::vector<FilePath>(path_set.begin(), path_set.end());
}

std::set<FilePath> PolyFileManager::get_paths_with_tags(const std::vector<FileTag> &tags) const
{
    auto result = data_manager.get_paths_with_tag(tags[0]);
    for (int i = 1; i < tags.size(); i++)
    {
        auto pre = std::move(result);
        auto cur = data_manager.get_paths_with_tag(tags[i]);
        std::set_intersection(cur.begin(), cur.end(), pre.begin(), pre.end(), std::inserter(result, result.begin()));
    }
    return result;
}
std::set<FileTag> PolyFileManager::get_tags_with_paths(const std::vector<FileTag> &paths) const
{
    std::set<FileTag> pre, result = data_manager.get_tags_for_path(paths[0]);
    for (int i = 1; i < paths.size(); i++)
    {
        pre = std::move(result);
        auto cur = data_manager.get_tags_for_path(paths[i]);
        std::set_intersection(cur.begin(), cur.end(), pre.begin(), pre.end(), std::inserter(result, result.begin()));
    }
    return result;
}

bool PolyFileManager::is_data_consistent_with_repository() const
{
    auto tag_vector = data_manager.get_all_tags();
    auto category_names_set = file_manager.get_category_dir_names_in_repo();
    if (tag_vector.size() != category_names_set.size())
    {
        return false;
    }
    for (const auto &name : tag_vector)
    {
        if (category_names_set.find(name) == category_names_set.end())
        {
            return false;
        }
    }
    for (const auto &tag : tag_vector)
    {
        auto paths = data_manager.get_paths_with_tag(tag);
        auto symlink_names = file_manager.get_symlink_names_in_category(tag);
        if (paths.size() != symlink_names.size())
        {
            return false;
        }
        for (const auto &path : paths)
        {
            if (symlink_names.find(path.filename().string()) == symlink_names.end())
            {
                return false;
            }
        }
    }
    return true;
}

bool PolyFileManager::sync_data_with_repository()
{
    for (const auto &entry : std::filesystem::directory_iterator(get_repo_path()))
    {
        if (std::filesystem::is_directory(entry.path()))
        {
            if (std::filesystem::remove_all(entry.path()) < 1)
            {
                return false;
            }
        }
    }
    auto tags = data_manager.get_all_tags();
    for (const auto &t : tags)
        if (!file_manager.create_category_dir(t))
        {
            return false;
        }
    for (const auto &tag : tags)
    {
        auto paths = data_manager.get_paths_with_tag(tag);
        for (const auto &path : paths)
        {
            if (!file_manager.create_symlink_in_category(tag, path))
            {
                return false;
            }
        }
    }
    return true;
}