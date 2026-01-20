#ifndef CMD_HPP
#define CMD_HPP

#include "poly_file_manager.hpp"
#include <filesystem>

namespace poly::cli
{

enum class CommandResult
{
    Success = 0,
    InvalidArgs = 1,
    BusinessError = 2
};

class Cmd
{
  public:
    explicit Cmd(std::filesystem::path repo_path) : repo_path_(std::move(repo_path)) {}
    virtual ~Cmd() = default;

    virtual CommandResult execute() = 0;

  protected:
    std::filesystem::path repo_path_;
    PolyFileManager create_manager() const { return PolyFileManager(repo_path_); }
};
} // namespace poly::cli
#endif // CMD_HPP