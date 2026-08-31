#include "data_manager.hpp"

#include <map>

DataManager::DataManager(const FileTagData &data)
{
    for (const auto &tag : data.tags)
    {
        add_tag(tag);
    }
    for (const auto &[path, indices] : data.path_to_tag_indices)
    {
        add_path(path);
        for (const auto index : indices)
        {
            if (index < data.tags.size())
            {
                add_association(data.tags[index], path);
            }
        }
    }
}

FileTagData DataManager::to_file_tag_data() const
{
    FileTagData data;
    std::map<FileTag, std::size_t> tag_index;
    std::size_t index = 0;

    for (const auto &tag : tags())
    {
        data.tags.push_back(tag);
        tag_index[tag] = index++;
    }

    for (const auto &path : paths())
    {
        auto &indices = data.path_to_tag_indices[path];
        const auto path_tags = tags_of(path);
        indices.reserve(path_tags.size());
        for (const auto &tag : path_tags)
        {
            indices.push_back(tag_index[tag]);
        }
    }
    return data;
}

bool DataManager::add_tag(const FileTag &tag)
{
    return tag_path_.insert_left(tag);
}

bool DataManager::rename_tag(const FileTag &old_tag, const FileTag &new_tag)
{
    return tag_path_.replace_left(old_tag, new_tag);
}

bool DataManager::remove_tag(const FileTag &tag) noexcept
{
    return tag_path_.erase_left(tag);
}

bool DataManager::has_tag(const FileTag &tag) const noexcept
{
    return tag_path_.contains_left(tag);
}

std::size_t DataManager::tag_count() const noexcept
{
    return tag_path_.count_left();
}

std::set<FileTag> DataManager::tags() const
{
    return tag_path_.get_all_left();
}

bool DataManager::add_path(const FilePath &path)
{
    return tag_path_.insert_right(path);
}

bool DataManager::replace_path(const FilePath &old_path, const FilePath &new_path)
{
    return tag_path_.replace_right(old_path, new_path);
}

bool DataManager::remove_path(const FilePath &path) noexcept
{
    return tag_path_.erase_right(path);
}

bool DataManager::has_path(const FilePath &path) const noexcept
{
    return tag_path_.contains_right(path);
}

std::size_t DataManager::path_count() const noexcept
{
    return tag_path_.count_right();
}

std::set<FilePath> DataManager::paths() const
{
    return tag_path_.get_all_right();
}

bool DataManager::add_association(const FileTag &tag, const FilePath &path)
{
    return tag_path_.associate(tag, path);
}

bool DataManager::remove_association(const FileTag &tag, const FilePath &path)
{
    return tag_path_.unassociate(tag, path);
}

bool DataManager::has_association(const FileTag &tag, const FilePath &path) const noexcept
{
    return tag_path_.contains_association(tag, path);
}

std::size_t DataManager::path_count_of(const FileTag &tag) const noexcept
{
    return tag_path_.count_associated_right(tag);
}

std::size_t DataManager::tag_count_of(const FilePath &path) const noexcept
{
    return tag_path_.count_associated_left(path);
}

std::set<FilePath> DataManager::paths_of(const FileTag &tag) const
{
    return tag_path_.get_associated_right(tag);
}

std::set<FileTag> DataManager::tags_of(const FilePath &path) const
{
    return tag_path_.get_associated_left(path);
}
