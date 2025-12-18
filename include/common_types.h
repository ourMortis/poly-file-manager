#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <string>
#include <filesystem>
#include <vector>
#include <map>

using FileTag = std::string;
using FilePath = std::filesystem::path;
using FileTagPtr = std::shared_ptr<FileTag>;
using FilePathPtr = std::shared_ptr<FilePath>;

#ifdef SERIALIZER_H
using json = nlohmann::json;
#endif

struct FileTagData
{
    std::vector<FileTag> index_to_tag;
    std::map<FilePath, std::vector<int>> path_to_index_map;
};

#endif // COMMEN_TYPES_H