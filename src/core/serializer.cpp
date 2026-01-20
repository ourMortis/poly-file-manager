#include "serializer.hpp"

Serializer::Serializer(const FilePath &repo_path)
{
    if (!repo_path.is_absolute() || !std::filesystem::is_directory(repo_path))
    {
        throw std::invalid_argument("Path is not absolute or not a dirctory: " + repo_path.string());
    }
    repo_path_ = repo_path;
}

#ifdef _WIN32
bool Serializer::set_file_hidden(const std::filesystem::path &path) const
{
    return SetFileAttributesW(path.wstring().c_str(),
                              GetFileAttributesW(path.wstring().c_str()) | FILE_ATTRIBUTE_HIDDEN) != 0;
}
bool Serializer::remove_file_hidden(const std::filesystem::path &path) const
{
    return SetFileAttributesW(path.wstring().c_str(),
                              GetFileAttributesW(path.wstring().c_str()) & ~FILE_ATTRIBUTE_HIDDEN) != 0;
}
#endif

void Serializer::set_repo_path(FilePath path) noexcept { repo_path_ = path; }
FilePath Serializer::get_repo_path() const noexcept { return repo_path_; }

json Serializer::data_to_json(const FileTagData &data) const
{
    json root_j;
    root_j["index_to_tag"] = data.index_to_tag;
    root_j["path_to_index_map"] = data.path_to_index_map;
    return root_j;
}

json Serializer::file_to_json() const
{
    std::ifstream ifs(repo_path_ / config_file_path);
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
    FilePath config = repo_path_ / config_file_path;
    if (std::filesystem::is_regular_file(config))
    {
        remove_file_hidden(config);
    }
#endif

    json j = data_to_json(data);
    std::ofstream ofs(repo_path_ / config_file_path);
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << j;
    ofs.close();

#ifdef _WIN32
    if (std::filesystem::is_regular_file(config))
    {
        set_file_hidden(config);
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