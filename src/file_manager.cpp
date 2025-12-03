#include "file_manager.h"

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}



void FileManager::create_tag(file_tag tag)
{
    auto it = tags.find(tag);
    if(it != tags.end())
        return;
    tags[tag] = std::make_shared<file_tag>(tag);
    tag_with_paths[tags[tag]];
}
void FileManager::change_tag(const file_tag& old_tag, file_tag new_tag)
{
    auto it = tags.find(old_tag);
    if(it == tags.end())
        return;
    auto ptr = std::move(it->second);
    ptr->assign(std::move(new_tag));

    tags.erase(it);
    tags.emplace(*ptr, std::move(ptr));
}
void FileManager::delete_tag(const file_tag &tag)
{
    auto it = tags.find(tag);
    if(it == tags.end())
        return;
    for(auto path : tag_with_paths[tags[tag]])
    {
        path_with_tags[path].erase(tags[tag]);
    }
    tag_with_paths.erase(tags[tag]);
    tags.erase(tag);
}
std::vector<file_tag> FileManager::get_tags()
{
    std::vector<std::string> v;
    v.resize(tags.size());
    std::transform(tags.begin(), tags.end(), v.begin(),
               [](const auto& k) { return k.first; });
    return v;
};




void FileManager::create_path(file_path path)
{
    auto it = paths.find(path);
    if(it != paths.end())
        return;
    paths[path] = std::make_shared<file_path>(path);
    path_with_tags[paths[path]];
}
void FileManager::change_path(const file_path &old_path, file_path new_path)
{
    auto it = paths.find(old_path);
    if(it == paths.end())
        return;
    auto ptr = std::move(it->second);
    ptr->assign(std::move(new_path));

    paths.erase(it);
    paths.emplace(*ptr, std::move(ptr));
}
void FileManager::delete_path(const file_path &path)
{
    auto it = paths.find(path);
    if(it == paths.end())
        return;
    for(auto tag : path_with_tags[paths[path]])
    {
        tag_with_paths[tag].erase(paths[path]);
    }
    path_with_tags.erase(it->second);
    paths.erase(it);
}
std::vector<file_path> FileManager::get_paths()
{
    std::vector<std::filesystem::path> v;
    v.resize(paths.size());
    std::transform(paths.begin(), paths.end(), v.begin(),
               [](const auto& k) { return k.first; });
    return v;
}



// std::set<file_tag> FileManager::get_tags(sptr_file_path path)
// {
//     return tags;
// }

// std::set<sptr_file_path> FileManager::get_paths(file_tag tag)
// {
//     return paths;
// }


void FileManager::print_tags()
{
    std::cout << "------------tags----------\n";
    for(auto i : tags)
    {
        std::cout << i.first << " : " << i.second << std::endl;
    }
    std::cout << "--------------------------\n";
}
void FileManager::print_tag_with_paths()
{
    std::cout << "----------tag_with_paths----------\n";
    for(auto i : tag_with_paths)
    {
        std::cout << *(i.first) << " : ";
        for(auto j : i.second)
        {
            std::cout << j->string() << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << "----------------------------------\n";
}
void FileManager::print_paths()
{
    std::cout << "-----------paths-----------\n";
    for(auto i : paths)
    {
        std::cout << i.first.string() << " : " << i.second << std::endl;
    }
    std::cout << "---------------------------\n";
}
void FileManager::print_path_with_tags()
{
    std::cout << "--------path_with_tags-----------\n";
    for(auto i : path_with_tags)
    {
        std::cout << (i.first)->string() << " : ";
        for(auto j : i.second)
        {
            std::cout << *j << ", ";
        }
        std::cout << std::endl;
    }
    std::cout << "---------------------------------\n";
}


void FileManager::add_file_tag(file_path &path, file_tag tag)
{
    auto it_p = path_with_tags.find(paths[path]);
    if(it_p == path_with_tags.end())
    {
        create_path(path);
    }
    auto it_t = tag_with_paths.find(tags[tag]);
    if(it_t == tag_with_paths.end())
    {
        create_tag(tag);
    }

    it_p->second.insert(tags[tag]);
    it_t->second.insert(paths[path]);
}

void FileManager::delete_file_tag(file_path &path, file_tag tag)
{
    auto it_p = path_with_tags.find(paths[path]);
    if(it_p == path_with_tags.end())
    {
        return;
    }
    auto it_t = it_p->second.find(tags[tag]);
    if(it_t == it_p->second.end())
    {
        return;
    }
    it_p->second.erase(tags[tag]);
}