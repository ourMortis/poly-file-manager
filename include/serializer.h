#ifndef SERIALIZER_H
#define SERIALIZER_H

#include<string>
#include<filesystem>
#include<vector>
#include<map>
#include<fstream>
#include"json.hpp"

using FilePath = std::filesystem::path;
using FileTag = std::string;
using json = nlohmann::json;


struct FileTagData
{
    std::map<FileTag, std::vector<FilePath>> tag_to_paths_map;
    std::map<FilePath, std::vector<FileTag>> path_to_tags_map;
};



class Serializer
{
private:
    const std::string config_file_path = ".poly_file_manager";
    FilePath repo_path_;

public:
    Serializer() = default;
    ~Serializer() = default;

    void set_repo_path(FilePath path);
    FilePath get_repo_path() const;

    bool serialize_to_file(const FileTagData& data) const;

    FileTagData deserialize_from_file() const;

};

#endif // SERIALIZER_H
