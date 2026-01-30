#ifndef CMD_HPP
#define CMD_HPP

#include "poly_file_manager.hpp"
#include <filesystem>

namespace poly::cli
{

enum class ErrorCode
{
    Success = 0,

    InvalidArgs_EmptyArgument = 101,
    InvalidArgs_InvalidPath,
    InvalidArgs_InvalidTag,
    InvalidArgs_ConflictOption,

    Business_AddTagFailed = 201,
    Business_AddPathFailed,
    Business_RenameTagFailed,
    Business_RenamePathFailed,
    Business_RemoveTagFailed,
    Business_RemovePathFailed,
    Business_SaveResultFailed,
    Business_ReadFileFailed,
    Business_AssignTagFailed,
    Business_RemoveAssociationFailed,
    Business_CreateRepositoryFailed,

    System_FileSystemError = 301,
    System_OutOfMemory,
    System_Unknown
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