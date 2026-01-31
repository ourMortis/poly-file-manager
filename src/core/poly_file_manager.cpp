#include "poly_file_manager.hpp"
#include "common_types.hpp"
#include "data_manager.hpp"
#include "file_manager.hpp"
#include "serializer.hpp"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fltwinerror.h>
#include <stdexcept>
#include <vector>
#include <winnt.h>

bool PolyFileManager::repo_path_invalid(const FilePath &repo_path) const
{
    return !(repo_path.is_absolute() && std::filesystem::is_directory(repo_path));
}

PolyFileManager::PolyFileManager(const std::filesystem::path &repo_path, bool)
    : file_manager(repo_path), serializer(repo_path)
{
    data_manager = DataManager(serializer.deserialize_from_file());
}

PolyFileManager::PolyFileManager(const std::filesystem::path &repo_path) : PolyFileManager(repo_path, true)
{
    if (repo_path_invalid(repo_path))
    {
        throw std::invalid_argument("PolyFileManager: invalid repo_path");
    }
}

std::filesystem::path PolyFileManager::get_repo_path() const noexcept { return file_manager.get_repo_path(); }

bool PolyFileManager::add_path(const FilePath &path)
{
    if (path.is_relative() || data_manager.contains_path(path))
    {
        return false;
    }
    data_manager.create_path(path);
    return true;
}

bool PolyFileManager::rename_path(const FilePath &old_path, const FilePath &new_path)
{
    if (!data_manager.contains_path(old_path) || data_manager.contains_path(new_path))
    {
        return false;
    }
    int num_of_tags = data_manager.get_size_of_tags_for_path(old_path);
    int rename_symlink_cnt = 0;
    for (const auto &tag : data_manager.get_tags_for_path(old_path))
    {
        if (!file_manager.remove_symlink_in_category(tag, old_path))
        {
            return false;
        }
        if (!file_manager.create_symlink_in_category(tag, new_path))
        {
            file_manager.create_symlink_in_category(tag, old_path);
            return false;
        }
        rename_symlink_cnt++;
    }
    if (rename_symlink_cnt < num_of_tags)
    {
        return false;
    }
    data_manager.rename_path(old_path, new_path);
    return true;
}

bool PolyFileManager::remove_path(const FilePath &path)
{
    if (!data_manager.contains_path(path))
    {
        return false;
    }
    int num_of_tags = data_manager.get_size_of_tags_for_path(path);
    if (num_of_tags == 0)
    {
        data_manager.remove_path(path);
        return true;
    }
    const auto &tags = data_manager.get_tags_for_path(path);
    int remove_symlink_cnt = 0;
    for (const auto &tag : tags)
    {
        if (!file_manager.remove_symlink_in_category(tag, path))
        {
            break;
        }
        remove_symlink_cnt++;
    }
    if (remove_symlink_cnt < num_of_tags)
    {
        return false;
    }
    data_manager.remove_path(path);
    return true;
}

bool PolyFileManager::add_tag(const FileTag &tag)
{
    if (data_manager.contains_tag(tag))
    {
        return false;
    }
    if (!file_manager.create_category_dirs({tag}))
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
        data_manager.rename_tag(old_tag, new_tag);
        return true;
    }
    return false;
}

bool PolyFileManager::remove_tag(const FileTag &tag)
{
    if (!data_manager.contains_tag(tag))
    {
        return false;    
    }
    if (file_manager.remove_category_dir(tag) < 1)
    {
        return false;
    }
    data_manager.remove_tag(tag);
    return true;
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

std::vector<FileTag> PolyFileManager::get_all_tags() const { return data_manager.get_all_tags(); }
std::vector<FilePath> PolyFileManager::get_all_paths() const { return data_manager.get_all_paths(); }

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
    std::set<FilePath> pre, result = data_manager.get_paths_with_tag(tags[0]);
    
    for (int i = 1;i < tags.size();i++)
    {
        pre = std::move(result);
        auto cur = data_manager.get_paths_with_tag(tags[i]);
        std::set_intersection(cur.begin(), cur.end(), pre.begin(), pre.end(), std::inserter(result, result.begin()));
    }

    return result;
}
std::set<FileTag> PolyFileManager::get_tags_with_paths(const std::vector<FileTag> &paths) const
{
    std::set<FileTag> pre, result = data_manager.get_tags_for_path(paths[0]);
    
    for (int i = 1;i < paths.size();i++)
    {
        pre = std::move(result);
        auto cur = data_manager.get_tags_for_path(paths[i]);
        std::set_intersection(cur.begin(), cur.end(), pre.begin(), pre.end(), std::inserter(result, result.begin()));
    }

    return result;
}

void PolyFileManager::load(const std::filesystem::path &repo_path)
{
    FileTagData data = serializer.deserialize_from_file();
    data_manager = DataManager(data);
}

bool PolyFileManager::save() const
{
    FileTagData data = data_manager.get_file_tag_data();
    return serializer.serialize_to_file(data);
}

bool PolyFileManager::is_repository(const std::filesystem::path &repo_path)
{
    return std::filesystem::is_regular_file(repo_path / ".poly_file_manager");
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
    if (!file_manager.create_category_dirs(tags))
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