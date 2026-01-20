#ifndef TAG_CMD_HPP
#define TAG_CMD_HPP

#include "cmd.hpp"
#include <vector>
#include <string>

namespace poly::cli
{

class TagCmd : public Cmd
{
  public:
    TagCmd(std::filesystem::path repo_path, std::vector<std::string> add_tags, std::vector<std::string> remove_tags,
           std::pair<std::string, std::string> modify_tag)
        : Cmd(std::move(repo_path)), add_tags_(std::move(add_tags)), remove_tags_(std::move(remove_tags)),
          modify_tag_(std::move(modify_tag))
    {
    }

    CommandResult execute() override;

  private:
    std::vector<std::string> add_tags_;
    std::vector<std::string> remove_tags_;
    std::pair<std::string, std::string> modify_tag_;
};

} // namespace poly::cli

#endif // TAG_CMD_HPP