#include "data_manager.hpp"
#include "common_types.hpp"

DataManager::DataManager(const FileTagData &data)
{
    for (const auto &tag : data.index_to_tag)
    {
        create_tag(tag);
    }
    for (const auto &[path, v] : data.path_to_index_map)
    {
        create_path(path);
        for (const auto &index : v)
        {
            if (index < data.index_to_tag.size())
            {
                assign_tag_to_path(path, data.index_to_tag[index]);
            }
        }
    }
}

void DataManager::create_tag(const FileTag &tag)
{
    auto it = tag_registry_.find(tag);
    if (it != tag_registry_.end())
        return;

    tag_registry_[tag] = std::make_shared<FileTag>(tag);
    tag_to_paths_map_[tag_registry_[tag]];
}

void DataManager::rename_tag(const FileTag &old_tag, const FileTag &new_tag)
{
    auto it = tag_registry_.find(old_tag);
    if (it == tag_registry_.end())
        return;

    auto tag_ptr = std::move(it->second);
    *tag_ptr = new_tag;

    tag_registry_.erase(it);
    tag_registry_.emplace(*tag_ptr, std::move(tag_ptr));
}

void DataManager::remove_tag(const FileTag &tag)
{
    auto it = tag_registry_.find(tag);
    if (it == tag_registry_.end())
        return;

    auto tag_ptr = it->second;

    if (auto path_set_it = tag_to_paths_map_.find(tag_ptr); path_set_it != tag_to_paths_map_.end())
    {
        for (auto path_ptr : path_set_it->second)
        {
            path_to_tags_map_[path_ptr].erase(tag_ptr);
        }
        tag_to_paths_map_.erase(tag_ptr);
    }

    tag_registry_.erase(it);
}

std::vector<FileTag> DataManager::get_all_tags() const
{
    std::vector<FileTag> tags;
    tags.reserve(tag_registry_.size());
    std::transform(tag_registry_.begin(), tag_registry_.end(), std::back_inserter(tags),
                   [](const auto &pair) { return pair.first; });
    return tags;
}

int DataManager::get_size_of_tags() const
{
    return tag_registry_.size();    
}

void DataManager::create_path(const FilePath &path)
{
    auto it = path_registry_.find(path);
    if (it != path_registry_.end())
        return;

    path_registry_[path] = std::make_shared<FilePath>(path);
    path_to_tags_map_[path_registry_[path]];
}

void DataManager::rename_path(const FilePath &old_path, const FilePath &new_path)
{
    auto it = path_registry_.find(old_path);
    if (it == path_registry_.end())
        return;

    auto path_ptr = std::move(it->second);
    *path_ptr = new_path;

    path_registry_.erase(it);
    path_registry_.emplace(*path_ptr, std::move(path_ptr));
}

void DataManager::remove_path(const FilePath &path)
{
    auto it = path_registry_.find(path);
    if (it == path_registry_.end())
        return;

    auto path_ptr = it->second;

    if (auto tag_set_it = path_to_tags_map_.find(path_ptr); tag_set_it != path_to_tags_map_.end())
    {
        for (auto tag_ptr : tag_set_it->second)
        {
            tag_to_paths_map_[tag_ptr].erase(path_ptr);
        }
        path_to_tags_map_.erase(path_ptr);
    }

    path_registry_.erase(it);
}

std::vector<FilePath> DataManager::get_all_paths() const
{
    std::vector<FilePath> paths;
    paths.reserve(path_registry_.size());
    std::transform(path_registry_.begin(), path_registry_.end(), std::back_inserter(paths),
                   [](const auto &pair) { return pair.first; });
    return paths;
}

int DataManager::get_size_of_paths() const
{
    return path_registry_.size();
}

void DataManager::assign_tag_to_path(const FilePath &path, const FileTag &tag)
{
    auto path_ptr = get_or_create_path_ptr(path);
    auto tag_ptr = get_or_create_tag_ptr(tag);

    path_to_tags_map_[path_ptr].insert(tag_ptr);
    tag_to_paths_map_[tag_ptr].insert(path_ptr);
}

void DataManager::remove_tag_from_path(const FilePath &path, const FileTag &tag)
{
    auto path_ptr = find_path_ptr(path);
    if (!path_ptr)
        return;

    auto tag_ptr = find_tag_ptr(tag);
    if (!tag_ptr)
        return;

    if (auto path_tags_it = path_to_tags_map_.find(path_ptr); path_tags_it != path_to_tags_map_.end())
    {
        path_tags_it->second.erase(tag_ptr);
    }

    if (auto tag_paths_it = tag_to_paths_map_.find(tag_ptr); tag_paths_it != tag_to_paths_map_.end())
    {
        tag_paths_it->second.erase(path_ptr);
    }
}

std::set<FileTag> DataManager::get_tags_for_path(const FilePath &path) const
{
    std::set<FileTag> tags;
    auto path_ptr = find_path_ptr(path);

    if (path_ptr)
    {
        if (auto it = path_to_tags_map_.find(path_ptr); it != path_to_tags_map_.end())
        {
            for (const auto &tag_ptr : it->second)
            {
                tags.insert(*tag_ptr);
            }
        }
    }

    return tags;
}

std::set<FilePath> DataManager::get_paths_with_tag(const FileTag &tag) const
{
    std::set<FilePath> paths;
    auto tag_ptr = find_tag_ptr(tag);

    if (tag_ptr)
    {
        if (auto it = tag_to_paths_map_.find(tag_ptr); it != tag_to_paths_map_.end())
        {
            for (const auto &path_ptr : it->second)
            {
                paths.insert(*path_ptr);
            }
        }
    }

    return paths;
}
int DataManager::get_size_of_tags_for_path(const FilePath &path) const
{
    auto sptr_it = path_registry_.find(path);
    if (sptr_it == path_registry_.end())
    {
        return 0;
    }
    auto it = path_to_tags_map_.find(sptr_it->second);

    return it->second.size();
}
int DataManager::get_size_of_paths_with_tag(const FileTag &tag) const
{
    auto sptr_it = tag_registry_.find(tag);
    if (sptr_it == tag_registry_.end())
    {
        return 0;
    }
    auto it = tag_to_paths_map_.find(sptr_it->second);

    return it->second.size();
}

FileTagData DataManager::get_file_tag_data() const
{
    FileTagData data;
    std::map<FileTag, int> hash;
    int i = 0;
    for (const auto &tag : tag_registry_ | std::views::keys)
    {
        data.index_to_tag.push_back(tag);
        hash[tag] = i++;
    }
    for (const auto &pathptr_tagptrs : path_to_tags_map_)
    {
        data.path_to_index_map[*(pathptr_tagptrs.first)];
        for (const auto &tag_ptr : pathptr_tagptrs.second)
        {
            data.path_to_index_map[*(pathptr_tagptrs.first)].push_back(hash[*tag_ptr]);
        }
    }
    return data;
}

FileTagPtr DataManager::get_or_create_tag_ptr(const FileTag &tag)
{
    auto it = tag_registry_.find(tag);
    if (it != tag_registry_.end())
    {
        return it->second;
    }

    create_tag(tag);
    return tag_registry_[tag];
}

FilePathPtr DataManager::get_or_create_path_ptr(const FilePath &path)
{
    auto it = path_registry_.find(path);
    if (it != path_registry_.end())
    {
        return it->second;
    }

    create_path(path);
    return path_registry_[path];
}

FileTagPtr DataManager::find_tag_ptr(const FileTag &tag) const
{
    auto it = tag_registry_.find(tag);
    return (it != tag_registry_.end()) ? it->second : nullptr;
}

FilePathPtr DataManager::find_path_ptr(const FilePath &path) const
{
    auto it = path_registry_.find(path);
    return (it != path_registry_.end()) ? it->second : nullptr;
}