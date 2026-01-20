#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <string>
#include <filesystem>
#include <fstream>
#include "common_types.hpp"
#ifdef _WIN32
#include <windows.h>
#endif

class Serializer
{
private:
    const std::string config_file_path = ".poly_file_manager";
    FilePath repo_path_;

#ifdef _WIN32
    bool set_file_hidden(const std::filesystem::path &path) const;
    bool remove_file_hidden(const std::filesystem::path &path) const;
#endif

public:
    Serializer(const FilePath &repo_path);
    ~Serializer() = default;

    void set_repo_path(FilePath path) noexcept;
    FilePath get_repo_path() const noexcept;

    json data_to_json(const FileTagData &data) const;

    json file_to_json() const;

    bool serialize_to_file(const FileTagData &data) const;

    FileTagData deserialize_from_file() const;
};

#endif // SERIALIZER_H
