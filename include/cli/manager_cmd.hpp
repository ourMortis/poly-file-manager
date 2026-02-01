#ifndef MANAGER_CMD_HPP
#define MANAGER_CMD_HPP

#include "cmd.hpp"
#include <vector>
#include <string>

namespace poly::cli
{

class ManagerCmd : public Cmd
{
  public:
    ManagerCmd(std::filesystem::path repo_path, std::vector<std::string> tags, std::vector<std::string> paths,
               std::vector<std::string> tag_or_path, bool remove_flag, bool create_flag, bool check_consistency_flag,
               bool Sync_repo_with_data_flag)
        : Cmd(std::move(repo_path)), tags_(std::move(tags)), paths_(std::move(paths)),
          tag_or_path_(std::move(tag_or_path)), remove_flag_(std::move(remove_flag)),
          create_flag_(std::move(create_flag)), check_consistency_flag_(std::move(check_consistency_flag)), 
          sync_repo_with_data_flag_(std::move(Sync_repo_with_data_flag))
    {
    }
    CommandError execute() override;

  private:
    std::vector<std::string> tags_;
    std::vector<std::string> paths_;
    std::vector<std::string> tag_or_path_;
    bool remove_flag_ = false;
    bool create_flag_ = false;
    bool check_consistency_flag_ = false;
    bool sync_repo_with_data_flag_ = false;
};

} // namespace poly::cli

#endif // MANAGER_CMD_HPP