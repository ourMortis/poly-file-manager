#include "serializer.hpp"

json Serializer::data_to_json(const FileTagData &data) const
{
    json root_j;
    root_j["index_to_tag"] = data.index_to_tag;
    root_j["path_to_index_map"] = data.path_to_index_map;
    return root_j;
}

json Serializer::file_to_json() const
{
    std::ifstream ifs(repo_path_ / data_file_name_);
    if (!ifs.is_open())
    {
        return {};
    }
    json j;
    ifs >> j;
    ifs.close();
    return j;
}

bool Serializer::serialize_to_file(const FileTagData &data) const
{
#ifdef _WIN32
    FilePath data_file_path = repo_path_ / data_file_name_;
    if (std::filesystem::is_regular_file(data_file_path))
    {
        if (!Tool::remove_file_hidden(data_file_path))
        {
            return false;    
        }
    }
#endif

    json j = data_to_json(data);
    std::ofstream ofs(repo_path_ / data_file_name_);
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << j;
    ofs.close();

#ifdef _WIN32
    if (std::filesystem::is_regular_file(data_file_path))
    {
        if (!Tool::set_file_hidden(data_file_path))
        {
            return false;
        }
    }
#endif
    return true;
}

FileTagData Serializer::deserialize_from_file() const
{
    FileTagData data;
    json j = file_to_json();
    if (j.contains("index_to_tag") && j.contains("path_to_index_map"))
    {
        data.index_to_tag = j["index_to_tag"];
        data.path_to_index_map = j["path_to_index_map"];
    }
    return data;
}