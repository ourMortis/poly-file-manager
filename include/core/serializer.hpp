#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "common_types.hpp"

#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

/**
 * @brief Persists a FileTagData relation to a JSON data file.
 *
 * Responsibilities are split into two clearly separated groups:
 *   - stateless conversion between the domain object (FileTagData) and json;
 *   - stateful file persistence (read/write the data file), including the
 *     Windows hidden-attribute handling for the data file.
 */
class Serializer
{
  public:
    explicit Serializer(const FilePath &repo_path, const FilePath &data_file_name);

    [[nodiscard]] const FilePath &repo_path() const noexcept;
    [[nodiscard]] const FilePath &data_file_name() const noexcept;
    [[nodiscard]] FilePath data_file_path() const;

    // ----- Pure conversion between FileTagData and json (stateless) ----------

    [[nodiscard]] static json to_json(const FileTagData &data);
    [[nodiscard]] static FileTagData from_json(const json &root);

    // ----- File persistence ---------------------------------------------------

    bool save(const FileTagData &data) const;
    [[nodiscard]] FileTagData load() const;

  private:
    FilePath repo_path_;
    FilePath data_file_name_;

#ifdef _WIN32
    static bool set_hidden(const FilePath &path);
    static bool clear_hidden(const FilePath &path);
#endif
};

#endif // SERIALIZER_H
