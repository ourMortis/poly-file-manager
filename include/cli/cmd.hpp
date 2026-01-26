#ifndef CMD_HPP
#define CMD_HPP

#include "poly_file_manager.hpp"
#include <filesystem>

namespace poly::cli
{

enum class ErrorCode
{
    Success = 0,

    InvalidArgs_EmptyRequired = 101,
    InvalidArgs_InvalidPath = 102,
    InvalidArgs_ConflictOption = 103,
    InvalidArgs_MissingParameter = 104,

    Business_TagNotFound = 201,
    Business_PathNotFound = 202,
    Business_RenameTagFailed = 203,
    Business_RenamePathFailed = 204,
    Business_SaveFileFailed = 205,
    Business_ReadFileFailed = 206,
    Business_AssignTagFailed = 207,
    Business_CreateRepositoryFailed = 208,
    Business_RemoveTagFailed = 209,
    Business_RemovePathFailed = 210,
    Business_CreateTagFailed = 211,
    Business_CreatePathFailed = 212,

    System_FileSystemError = 301,
    System_PermissionDenied = 302,
    System_Unknown = 999
};

struct CommandError
{
    ErrorCode code;
    std::string message;
};

class Cmd
{
  public:
    explicit Cmd(std::filesystem::path repo_path) : repo_path_(std::move(repo_path)) {}
    virtual ~Cmd() = default;

    virtual CommandError execute() = 0;

  protected:
    std::filesystem::path repo_path_;
    PolyFileManager create_manager() const { return PolyFileManager(repo_path_); }
};
} // namespace poly::cli
#endif // CMD_HPP