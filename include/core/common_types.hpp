#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include "json.hpp"
#include <cstddef>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

using FileTag = std::string;
using FilePath = std::filesystem::path;
using json = nlohmann::json;

/**
 * @brief Compact, index-based transfer format for a tag/path relation.
 *
 * `tags` is the tag registry (its position is the index used by
 * `path_to_tag_indices`); `path_to_tag_indices` maps each path to the indices of
 * the tags associated with it. Keeping only indices avoids repeating tag
 * strings on disk while staying fully reconstructable.
 */
struct FileTagData
{
    std::vector<FileTag> tags;
    std::map<FilePath, std::vector<std::size_t>> path_to_tag_indices;

    bool operator==(const FileTagData &) const = default;
};

#endif // COMMON_TYPES_H
