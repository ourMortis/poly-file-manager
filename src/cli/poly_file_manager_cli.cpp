#include "CLI11.hpp"
#include "poly_file_manager.hpp"
#include <filesystem>
#include <string>
#include <vector>


void app_run(std::vector<std::string> &tags, std::vector<std::string> &paths, std::string &tag_or_path,
             bool &remove_flag, std::string &repo_path)
{
    if (repo_path.empty())
    {
        repo_path = std::filesystem::current_path().string();
    }

    if (tags.empty() && paths.empty())
    {
        return;
    }
    if (!tags.empty() && !paths.empty())
    {
        return;
    }
    if (tag_or_path.empty())
    {
        return;
    }

    PolyFileManager manager(repo_path);
    if (!tags.empty())
    {
        if (remove_flag)
        {
            for (const auto &tag : tags)
            {
                manager.remove_tag_from_file(tag_or_path, tag);
            }
        }
        else
        {
            for (const auto &tag : tags)
            {
                manager.assign_tag_to_file(tag_or_path, tag);
            }
        }
    }
    else
    {
        if (remove_flag)
        {
            for (const auto &path : paths)
            {
                manager.remove_tag_from_file(path, tag_or_path);
            }
        }
        else
        {
            for (const auto &path : paths)
            {
                manager.assign_tag_to_file(path, tag_or_path);
            }
        }
    }
    manager.save();
}

void create_run(std::string &repo_path)
{
    if (repo_path.empty())
    {
        repo_path = std::filesystem::current_path().string();
    }
    PolyFileManager manager(repo_path);
    manager.save();
}

void tag_run(std::vector<std::string> &add_tag, std::vector<std::string> &remove_tag,
             std::pair<std::string, std::string> &modify_tag, std::string &repo_path)
{
    if (repo_path.empty())
    {
        repo_path = std::filesystem::current_path().string();
    }
    PolyFileManager manager(repo_path);
    for (const auto &tag : add_tag)
    {
        manager.add_tag(tag);
    }
    for (const auto &tag : remove_tag)
    {
        manager.remove_tag(tag);
    }
    if (!modify_tag.first.empty())
    {
        manager.rename_tag(modify_tag.first, modify_tag.second);
    }
    manager.save();
}

void path_run(std::vector<std::string> &add_path, std::vector<std::string> &remove_path,
              std::pair<std::string, std::string> &modify_path, std::string &repo_path)
{
    if (repo_path.empty())
    {
        repo_path = std::filesystem::current_path().string();
    }
    PolyFileManager manager(repo_path);
    for (const auto &path : add_path)
    {
        manager.add_tag(path);
    }
    for (const auto &path : remove_path)
    {
        manager.remove_tag(path);
    }
    if (!modify_path.first.empty())
    {
        manager.rename_tag(modify_path.first, modify_path.second);
    }
    manager.save();
}

int main(int argc, char **argv)
{
    CLI::App app("PolyFileManager, Categorize files by tags.");
    app.require_subcommand(0, 1);
    auto *create_cmd = app.add_subcommand("create");
    auto *tag_cmd = app.add_subcommand("tag");
    auto *path_cmd = app.add_subcommand("path");

    std::string repo_path;
    app.add_option("--repo", repo_path, "repository_path");

    std::vector<std::string> add_tag, remove_tag;
    std::pair<std::string, std::string> modify_tag;
    tag_cmd->add_option("-a,--add", add_tag)->expected(1, 99);
    tag_cmd->add_option("-r,--remove", remove_tag)->expected(1, 99);
    tag_cmd->add_option("-m,--modify", modify_tag)->expected(2);

    std::vector<std::string> add_path, remove_path;
    std::pair<std::string, std::string> modify_path;
    path_cmd->add_option("-a,--add", add_path)->expected(1, 99);
    path_cmd->add_option("-r,--remove", remove_path)->expected(1, 99);
    path_cmd->add_option("-m,--modify", modify_path)->expected(2);

    std::vector<std::string> tags, paths;
    std::string tag_or_path;
    bool remove_flag = false;
    app.add_flag("-r", remove_flag);
    app.add_option("-t,--tag", tags)->expected(1, 99);
    app.add_option("-p,--path", paths)->expected(1, 99);
    app.add_option("path or tag", tag_or_path);

    app.parse(argc, argv);

    if (create_cmd->parsed())
    {
        std::cout << "created" << std::endl;
        create_run(repo_path);
    }
    else if (tag_cmd->parsed())
    {
        std::cout << "tag" << std::endl;
        tag_run(add_tag, remove_tag, modify_tag, repo_path);
    }
    else if (path_cmd->parsed())
    {
        std::cout << "path" << std::endl;
        path_run(add_path, remove_path, modify_path, repo_path);
    }
    else
    {
        app_run(tags, paths, tag_or_path, remove_flag, repo_path);
    }

    std::cout << repo_path << std::endl;
}