#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "common_types.hpp"
#include "tool.hpp"
#include <cstdio>
#include <filesystem>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

class Serializer
{
  private:
    FilePath repo_path_;
    FilePath data_file_name_;

  public:
    Serializer() = default;
    Serializer(const FilePath &repo_path, const FilePath &data_file_name)
        : repo_path_(repo_path), data_file_name_(data_file_name)
    {
    }

    void set_repo_path(FilePath path) noexcept { repo_path_ = path; }
    void set_data_file_name(FilePath path) noexcept { data_file_name_ = path; }

    FilePath get_repo_path() const noexcept { return repo_path_; }
    FilePath get_data_file_name() const noexcept { return data_file_name_; }

    json data_to_json(const FileTagData &data) const;

    json file_to_json() const;

    bool serialize_to_file(const FileTagData &data) const;

    FileTagData deserialize_from_file() const;
};

#endif // SERIALIZER_H
