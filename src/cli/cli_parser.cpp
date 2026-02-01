#include "cli_parser.hpp"
#include "CLI11.hpp"
#include "manager_cmd.hpp"
#include "path_cmd.hpp"
#include "tag_cmd.hpp"
#include <algorithm>
#include <filesystem>
#include <utility>

namespace poly::cli
{

CliParser::CliParser(int argc, char **argv)
    : app_("PolyFileManager, Categorize files by tags."), argc_(argc), argv_(argv)
{
    init_cli();
}

void CliParser::init_cli() noexcept
{
    app_.require_subcommand(0, 1);
    app_.add_option("--repo", repo_path_, "repository_path")->check(CLI::ExistingDirectory);
    app_.add_option("path_or_tag", tag_or_path_)->expected(0, 99);
    app_.add_flag("-r,--remove", remove_flag_, "Remove tag/path association");
    app_.add_option("-t,--tag", tags_)->expected(1, 99);
    app_.add_option("-p,--path", paths_)->expected(1, 99)->check(CLI::ExistingPath)->excludes("-t");
    app_.add_flag("-c,--create", create_flag_)->excludes("-t")->excludes("-p")->excludes("path_or_tag");
    app_.add_flag("--check", check_consistency_flag_, "Check the consistensy between data and repository")
        ->excludes("-c")
        ->excludes("-t")
        ->excludes("-p")
        ->excludes("path_or_tag")
        ->excludes("-r");
    app_.add_flag("-s,--sync", sync_repo_with_data_flag_, "Synchronize actual content of repository to data")
        ->excludes("-c")
        ->excludes("-t")
        ->excludes("-p")
        ->excludes("path_or_tag")
        ->excludes("-r")
        ->excludes("--check");
    
    auto *tag_cmd = app_.add_subcommand("tag", "Manage tags");
    tag_cmd->add_option("-a,--add", tag_add_)->expected(1, 99);
    tag_cmd->add_option("-r,--remove", tag_remove_)->expected(1, 99);
    tag_cmd->add_option("-m,--modify", tag_modify_)->expected(1);
    tag_cmd->add_flag("-l,--list", list_flag_)->excludes("-a")->excludes("-r")->excludes("-m");

    auto *path_cmd = app_.add_subcommand("path", "Manage paths");
    path_cmd->add_option("-a,--add", path_add_)->expected(1, 99)->check(CLI::ExistingPath);
    path_cmd->add_option("-r,--remove", path_remove_)->expected(1, 99)->check(CLI::ExistingPath);
    path_cmd->add_option("-m,--modify", path_modify_)->expected(1);
    path_cmd->add_flag("-l,--list", list_flag_)->excludes("-a")->excludes("-r")->excludes("-m");
}

std::unique_ptr<Cmd> CliParser::parse()
{
    try
    {
        app_.parse(argc_, argv_);

        std::filesystem::path repo_path(repo_path_);
        if (repo_path_.empty())
        {
            repo_path = std::filesystem::current_path();
        }
        else if (repo_path.is_relative())
        {
            repo_path = std::filesystem::current_path() / repo_path;
        }

        if (app_.get_subcommand("tag")->parsed())
        {
            return std::make_unique<TagCmd>(std::move(repo_path), std::move(tag_add_), std::move(tag_remove_),
                                            std::move(tag_modify_), std::move(list_flag_));
        }
        else if (app_.get_subcommand("path")->parsed())
        {
            return std::make_unique<PathCmd>(std::move(repo_path), std::move(path_add_), std::move(path_remove_),
                                             std::move(path_modify_), std::move(list_flag_));
        }
        else
        {
            return std::make_unique<ManagerCmd>(std::move(repo_path), std::move(tags_), std::move(paths_),
                                                std::move(tag_or_path_), std::move(remove_flag_),
                                                std::move(create_flag_), std::move(check_consistency_flag_),
                                                std::move(sync_repo_with_data_flag_));
        }
    }
    catch (const CLI::ParseError &e)
    {
        throw;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        throw CLI::ParseError("File system error: " + std::string(e.what()), CLI::ExitCodes::FileError);
    }
    catch (const std::exception &e)
    {
        throw CLI::ParseError("Unexpected error during parsing: " + std::string(e.what()),
                              CLI::ExitCodes::HorribleError);
    }
}

} // namespace poly::cli