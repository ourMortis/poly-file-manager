#include "poly_file_manager.hpp"
#include "common_types.hpp"
#include "data_manager.hpp"
#include "file_manager.hpp"
#include "serializer.hpp"
#include <filesystem>
#include <stdexcept>
#include <vector>

bool PolyFileManager::repo_path_invalid(const FilePath& repo_path) const
{
    return !(repo_path.is_absolute() && std::filesystem::is_directory(repo_path));
}

PolyFileManager::PolyFileManager(const std::filesystem::path &repo_path, bool)
    : file_manager(repo_path), serializer(repo_path)
{
    data_manager = DataManager(serializer.deserialize_from_file());
}


PolyFileManager::PolyFileManager(const std::filesystem::path &repo_path)
    :PolyFileManager(repo_path, true)
{
    if (!repo_path_invalid(repo_path))
    {
        throw std::invalid_argument("PolyFileManager: invalid repo_path");
    }
}

std::filesystem::path PolyFileManager::get_repo_path() const
{
    return file_manager.get_repo_path();
}

void PolyFileManager::add_path(const FilePath &path) { data_manager.create_path(path); }

int PolyFileManager::rename_path(const FilePath &old_path, const FilePath &new_path)
{
    int rename_symlink_cnt = 0;
    for (const auto &tag : data_manager.get_tags_for_path(old_path))
    {
        if (!file_manager.remove_symlink_in_category(tag, old_path))
        {
            return 0;
        }
        if(!file_manager.create_symlink_in_category(tag, new_path))
        {
            file_manager.create_symlink_in_category(tag, old_path);
            return 0; 
        }
        rename_symlink_cnt++;
    }
    data_manager.rename_path(old_path, new_path);
    
    return rename_symlink_cnt;
}

int PolyFileManager::remove_path(const FilePath &path)
{
    int remove_symlink_cnt = 0;
    for (const auto &tag : data_manager.get_tags_for_path(path))
    {
        if (!file_manager.remove_symlink_in_category(tag, path))
        {
            return 0;
        }
        remove_symlink_cnt++;
    }
    data_manager.remove_path(path);
    return remove_symlink_cnt;
}

void PolyFileManager::add_tag(const FileTag &tag) { data_manager.create_tag(tag); }

bool PolyFileManager::rename_tag(const FileTag &old_tag, const FileTag &new_tag)
{
    if (file_manager.rename_category_dir(old_tag, new_tag))
    {
        data_manager.rename_tag(old_tag, new_tag);
        return true;
    }
    return false;
}

int PolyFileManager::remove_tag(const FileTag &tag)
{
    int remove_items_cnt = file_manager.remove_category_dir(tag);
    if (remove_items_cnt > 0)
    {
        data_manager.remove_tag(tag); 
    }
    return remove_items_cnt;
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

std::vector<FileTag> PolyFileManager::get_all_tags() const
{
    return data_manager.get_all_tags();
}
std::vector<FilePath> PolyFileManager::get_all_paths() const
{
    return data_manager.get_all_paths();
}

std::vector<FileTag> PolyFileManager::get_tags_for_file(const FilePath &path) const
{
    auto tag_set = data_manager.get_tags_for_path(path);
    return std::vector<FileTag>(tag_set.begin(),tag_set.end());
}
std::vector<FilePath> PolyFileManager::get_paths_with_tag(const FileTag &tag) const
{
    auto path_set = data_manager.get_paths_with_tag(tag);
    return std::vector<FilePath>(path_set.begin(), path_set.end());
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