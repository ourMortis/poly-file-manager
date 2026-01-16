#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include "json.hpp"
#include <filesystem>
#include <map>
#include <string>
#include <vector>


using FileTag = std::string;
using FilePath = std::filesystem::path;
using FileTagPtr = std::shared_ptr<FileTag>;
using FilePathPtr = std::shared_ptr<FilePath>;
using json = nlohmann::json;

struct FileTagData
{
    std::vector<FileTag> index_to_tag;
    std::map<FilePath, std::vector<int>> path_to_index_map;
};

#endif // COMMEN_TYPES_H