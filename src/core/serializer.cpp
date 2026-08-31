#include "serializer.hpp"

#include <fstream>

Serializer::Serializer(const FilePath &repo_path, const FilePath &data_file_name)
    : repo_path_(repo_path), data_file_name_(data_file_name)
{
}

const FilePath &Serializer::repo_path() const noexcept
{
    return repo_path_;
}

const FilePath &Serializer::data_file_name() const noexcept
{
    return data_file_name_;
}

FilePath Serializer::data_file_path() const
{
    return repo_path_ / data_file_name_;
}

json Serializer::to_json(const FileTagData &data)
{
    json root;
    root["tags"] = data.tags;
    json path_map = json::object();
    for (const auto &[path, indices] : data.path_to_tag_indices)
    {
        path_map[path.generic_string()] = indices;
    }
    root["path_to_tag_indices"] = std::move(path_map);
    return root;
}

FileTagData Serializer::from_json(const json &root)
{
    FileTagData data;
    if (!root.is_object() || !root.contains("tags") || !root.contains("path_to_tag_indices") ||
        !root["tags"].is_array() || !root["path_to_tag_indices"].is_object())
    {
        return data;
    }
    root["tags"].get_to(data.tags);
    for (auto it = root["path_to_tag_indices"].begin(); it != root["path_to_tag_indices"].end(); ++it)
    {
        data.path_to_tag_indices[FilePath(it.key())] = it.value().get<std::vector<std::size_t>>();
    }
    return data;
}

bool Serializer::save(const FileTagData &data) const
{
    const FilePath file = data_file_path();
    std::error_code ec;

#ifdef _WIN32
    if (std::filesystem::is_regular_file(file, ec) && !clear_hidden(file))
    {
        return false;
    }
#endif

    std::ofstream ofs(file, std::ios::out | std::ios::trunc);
    if (!ofs.is_open())
    {
        return false;
    }
    ofs << to_json(data);
    if (!ofs)
    {
        return false;
    }
    ofs.close();

#ifdef _WIN32
    ec.clear();
    if (std::filesystem::is_regular_file(file, ec) && !set_hidden(file))
    {
        return false;
    }
#endif
    return true;
}

FileTagData Serializer::load() const
{
    const FilePath file = data_file_path();
    std::error_code ec;
    if (!std::filesystem::is_regular_file(file, ec))
    {
        return {};
    }

    std::ifstream ifs(file);
    if (!ifs.is_open())
    {
        return {};
    }

    json root;
    try
    {
        ifs >> root;
    }
    catch (const json::parse_error &)
    {
        return {};
    }
    return from_json(root);
}

#ifdef _WIN32
bool Serializer::set_hidden(const FilePath &path)
{
    const auto attrs = GetFileAttributesW(path.wstring().c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && SetFileAttributesW(path.wstring().c_str(),
                                                                  attrs | FILE_ATTRIBUTE_HIDDEN) != 0;
}

bool Serializer::clear_hidden(const FilePath &path)
{
    const auto attrs = GetFileAttributesW(path.wstring().c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && SetFileAttributesW(path.wstring().c_str(),
                                                                  attrs & ~FILE_ATTRIBUTE_HIDDEN) != 0;
}
#endif
