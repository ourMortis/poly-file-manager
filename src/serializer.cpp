#include "serializer.h"

void Serializer::set_repo_path(FilePath path)
{
    repo_path_ = path;
}
FilePath Serializer::get_repo_path() const
{
    return repo_path_;
}

bool Serializer::serialize_to_file(const FileTagData &data) const
{
    json j1;
    for (const auto &pair : data.tag_to_paths_map)
    {
        const FileTag &tag = pair.first;
        json path_arr;
        for (const auto &path : pair.second)
        {
            path_arr.push_back(path.string());
        }
        j1[tag] = path_arr;
    }

    json j2;
    for (const auto &pair : data.path_to_tags_map)
    {
        const std::string path_str = pair.first.string();
        j2[path_str] = pair.second;
    }

    json root_j;
    root_j["tag_to_paths"] = j1; // 给j1命名为tag_to_paths
    root_j["path_to_tags"] = j2; // 给j2命名为path_to_tags

    std::ofstream ofs(repo_path_ / config_file_path);
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << std::setw(4) << root_j;
    ofs.close();
    return true;
}

FileTagData Serializer::deserialize_from_file() const
{
    FileTagData data;
    std::ifstream ifs(repo_path_ / config_file_path);
    if (!ifs.is_open())
    {
        return data;
    }
    json root_j;
    ifs >> root_j;
    ifs.close();

    if (root_j.contains("tag_to_paths") && root_j["tag_to_paths"].is_object())
    {
        json j1 = root_j["tag_to_paths"];
        for (const auto &item : j1.items())
        {
            FileTag tag = item.key();
            json path_arr = item.value();
            std::vector<FilePath> paths;
            for (const auto &path_str_json : path_arr)
            {
                std::string path_str = path_str_json.get<std::string>();
                paths.push_back(FilePath(path_str));
            }
            data.tag_to_paths_map[tag] = paths;
        }
    }
    if (root_j.contains("path_to_tags") && root_j["path_to_tags"].is_object())
    {
        json j2 = root_j["path_to_tags"];
        for (const auto &item : j2.items())
        {
            FilePath path(item.key());
            json tag_arr = item.value();
            std::vector<FileTag> tags;
            for (const auto &tag_str_json : tag_arr)
            {
                tags.push_back(tag_str_json.get<std::string>());
            }
            data.path_to_tags_map[path] = tags;
        }
    }

    return data;
}