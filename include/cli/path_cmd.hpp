#ifndef PATH_CMD_HPP
#define PATH_CMD_HPP

#include "cmd.hpp"
#include <vector>
#include <string>

namespace poly::cli
{

class PathCmd : public Cmd
{
  public:
    PathCmd(std::filesystem::path repo_path, std::vector<std::string> add_paths, std::vector<std::string> remove_paths,
            std::pair<std::string, std::string> modify_path)
        : Cmd(std::move(repo_path)), add_paths_(std::move(add_paths)), remove_paths_(std::move(remove_paths)),
          modify_path_(std::move(modify_path))
    {
    }
    CommandError execute() override;

  private:
    std::vector<std::string> add_paths_;
    std::vector<std::string> remove_paths_;
    std::pair<std::string, std::string> modify_path_;
};

} // namespace poly::cli
#endif // PATH_CMD_HPP