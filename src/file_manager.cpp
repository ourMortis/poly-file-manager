#include "file_manager.h"

FileManager::FileManager(const FileTagData& data)
{
    for(const auto& tag : data.index_to_tag)
    {
        create_tag(tag);
    }
    for(const auto& path_index : data.path_to_index_map)
    {
        create_path(path_index.first);
        for(const auto& index : path_index.second)
        {
            if(index < data.index_to_tag.size())
            {
                assign_tag_to_path(path_index.first, data.index_to_tag[index]);
            }
        }
    }
}

// ==================== 标签管理 ====================
void FileManager::create_tag(const FileTag& tag)
{
    auto it = tag_registry_.find(tag);
    if (it != tag_registry_.end())
        return;
    
    tag_registry_[tag] = std::make_shared<FileTag>(tag);
    tag_to_paths_map_[tag_registry_[tag]]; // 创建空集合
}

void FileManager::rename_tag(const FileTag& old_tag, const FileTag& new_tag)
{
    auto it = tag_registry_.find(old_tag);
    if (it == tag_registry_.end())
        return;
    
    auto tag_ptr = std::move(it->second);
    *tag_ptr = new_tag;
    
    tag_registry_.erase(it);
    tag_registry_.emplace(*tag_ptr, std::move(tag_ptr));
}

void FileManager::remove_tag(const FileTag& tag)
{
    auto it = tag_registry_.find(tag);
    if (it == tag_registry_.end())
        return;
    
    auto tag_ptr = it->second;
    
    // 从所有关联的路径中移除该标签
    if (auto path_set_it = tag_to_paths_map_.find(tag_ptr); path_set_it != tag_to_paths_map_.end()) {
        for (auto path_ptr : path_set_it->second) {
            path_to_tags_map_[path_ptr].erase(tag_ptr);
        }
        tag_to_paths_map_.erase(tag_ptr);
    }
    
    tag_registry_.erase(it);
}

std::vector<FileTag> FileManager::get_all_tags() const
{
    std::vector<FileTag> tags;
    tags.reserve(tag_registry_.size());
    std::transform(tag_registry_.begin(), tag_registry_.end(), 
                   std::back_inserter(tags),
                   [](const auto& pair) { return pair.first; });
    return tags;
}

// ==================== 路径管理 ====================
void FileManager::create_path(const FilePath& path)
{
    auto it = path_registry_.find(path);
    if (it != path_registry_.end())
        return;
    
    path_registry_[path] = std::make_shared<FilePath>(path);
    path_to_tags_map_[path_registry_[path]]; // 创建空集合
}

void FileManager::rename_path(const FilePath& old_path, const FilePath& new_path)
{
    auto it = path_registry_.find(old_path);
    if (it == path_registry_.end())
        return;
    
    auto path_ptr = std::move(it->second);
    *path_ptr = new_path;
    
    path_registry_.erase(it);
    path_registry_.emplace(*path_ptr, std::move(path_ptr));
}

void FileManager::remove_path(const FilePath& path)
{
    auto it = path_registry_.find(path);
    if (it == path_registry_.end())
        return;
    
    auto path_ptr = it->second;
    
    // 从所有关联的标签中移除该路径
    if (auto tag_set_it = path_to_tags_map_.find(path_ptr); tag_set_it != path_to_tags_map_.end()) {
        for (auto tag_ptr : tag_set_it->second) {
            tag_to_paths_map_[tag_ptr].erase(path_ptr);
        }
        path_to_tags_map_.erase(path_ptr);
    }
    
    path_registry_.erase(it);
}

std::vector<FilePath> FileManager::get_all_paths() const
{
    std::vector<FilePath> paths;
    paths.reserve(path_registry_.size());
    std::transform(path_registry_.begin(), path_registry_.end(),
                   std::back_inserter(paths),
                   [](const auto& pair) { return pair.first; });
    return paths;
}

// ==================== 标签-路径关联管理 ====================
void FileManager::assign_tag_to_path(const FilePath& path, const FileTag& tag)
{
    // 确保路径和标签都存在
    auto path_ptr = get_or_create_path_ptr(path);
    auto tag_ptr = get_or_create_tag_ptr(tag);
    
    // 建立双向关联
    path_to_tags_map_[path_ptr].insert(tag_ptr);
    tag_to_paths_map_[tag_ptr].insert(path_ptr);
}

void FileManager::remove_tag_from_path(const FilePath& path, const FileTag& tag)
{
    auto path_ptr = find_path_ptr(path);
    if (!path_ptr)
        return;
    
    auto tag_ptr = find_tag_ptr(tag);
    if (!tag_ptr)
        return;
    
    // 移除双向关联
    if (auto path_tags_it = path_to_tags_map_.find(path_ptr); 
        path_tags_it != path_to_tags_map_.end()) {
        path_tags_it->second.erase(tag_ptr);
    }
    
    if (auto tag_paths_it = tag_to_paths_map_.find(tag_ptr);
        tag_paths_it != tag_to_paths_map_.end()) {
        tag_paths_it->second.erase(path_ptr);
    }
}

// ==================== 查询操作 ====================
std::set<FileTag> FileManager::get_tags_for_path(const FilePath& path) const
{
    std::set<FileTag> tags;
    auto path_ptr = find_path_ptr(path);
    
    if (path_ptr) {
        if (auto it = path_to_tags_map_.find(path_ptr); it != path_to_tags_map_.end()) {
            for (const auto& tag_ptr : it->second) {
                tags.insert(*tag_ptr);
            }
        }
    }
    
    return tags;
}

std::set<FilePath> FileManager::get_paths_with_tag(const FileTag& tag) const
{
    std::set<FilePath> paths;
    auto tag_ptr = find_tag_ptr(tag);
    
    if (tag_ptr) {
        if (auto it = tag_to_paths_map_.find(tag_ptr); it != tag_to_paths_map_.end()) {
            for (const auto& path_ptr : it->second) {
                paths.insert(*path_ptr);
            }
        }
    }
    
    return paths;
}

FileTagData FileManager::get_file_tag_data() const
{
    FileTagData data;
    std::map<FileTag, int> hash;
    int i = 0;
    for(const auto& tag : tag_registry_ | std::views::keys)
    {
        data.index_to_tag.push_back(tag);
        hash[tag] = i++;
    }
    for(const auto& pathptr_tagptrs : path_to_tags_map_)
    {
        data.path_to_index_map[*(pathptr_tagptrs.first)];
        for(const auto& tag_ptr : pathptr_tagptrs.second)
        {
            data.path_to_index_map[*(pathptr_tagptrs.first)].push_back(hash[*tag_ptr]);
        }
    }
    return data;
}

// ==================== 调试输出 ====================
void FileManager::dump_tags() const
{
    std::cout << "=========== Tags Registry ===========\n";
    for (const auto& [tag, ptr] : tag_registry_) {
        std::cout << "Tag: \"" << tag << "\" [Ptr: " << ptr << "]\n";
    }
    std::cout << "=====================================\n";
}

void FileManager::dump_tag_assignments() const
{
    std::cout << "======= Tag to Path Assignments =======\n";
    for (const auto& [tag_ptr, path_set] : tag_to_paths_map_) {
        std::cout << "Tag: \"" << *tag_ptr << "\" -> Paths: ";
        for (const auto& path_ptr : path_set) {
            std::cout << path_ptr->string() << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << "=======================================\n";
}

void FileManager::dump_paths() const
{
    std::cout << "========== Paths Registry ==========\n";
    for (const auto& [path, ptr] : path_registry_) {
        std::cout << "Path: \"" << path.string() << "\" [Ptr: " << ptr << "]\n";
    }
    std::cout << "====================================\n";
}

void FileManager::dump_path_tags() const
{
    std::cout << "======= Path to Tag Assignments =======\n";
    for (const auto& [path_ptr, tag_set] : path_to_tags_map_) {
        std::cout << "Path: \"" << path_ptr->string() << "\" -> Tags: ";
        for (const auto& tag_ptr : tag_set) {
            std::cout << *tag_ptr << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << "=======================================\n";
}

// ==================== 内部辅助函数 ====================
FileTagPtr FileManager::get_or_create_tag_ptr(const FileTag& tag)
{
    auto it = tag_registry_.find(tag);
    if (it != tag_registry_.end()) {
        return it->second;
    }
    
    // 标签不存在，创建它
    create_tag(tag);
    return tag_registry_[tag];
}

FilePathPtr FileManager::get_or_create_path_ptr(const FilePath& path)
{
    auto it = path_registry_.find(path);
    if (it != path_registry_.end()) {
        return it->second;
    }
    
    // 路径不存在，创建它
    create_path(path);
    return path_registry_[path];
}

FileTagPtr FileManager::find_tag_ptr(const FileTag& tag) const
{
    auto it = tag_registry_.find(tag);
    return (it != tag_registry_.end()) ? it->second : nullptr;
}

FilePathPtr FileManager::find_path_ptr(const FilePath& path) const
{
    auto it = path_registry_.find(path);
    return (it != path_registry_.end()) ? it->second : nullptr;
}