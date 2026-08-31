#include "cli.hpp"

#include "CLI11.hpp"
#include "poly_file_manager.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace
{

// Walk upward from the current directory (like git) to find a repository.
fs::path detect_repository()
{
    constexpr int max_depth = 5;
    auto dir = fs::current_path();
    for (int depth = 0; depth < max_depth; ++depth)
    {
        if (PolyFileManager::is_repository(dir))
        {
            return dir;
        }
        const auto parent = dir.parent_path();
        if (parent == dir)
        {
            break;
        }
        dir = parent;
    }
    return {};
}

fs::path resolve_repo_path(const std::string &given)
{
    if (!given.empty())
    {
        fs::path p = given;
        if (p.is_relative())
        {
            p = fs::current_path() / p;
        }
        return fs::absolute(p).lexically_normal();
    }
    return detect_repository();
}

} // namespace

int Cli::run(int argc, char **argv)
{
    struct Options
    {
        std::string repo;
        std::string tag;              // single value (for assign / unassign)
        std::string old_name, new_name;
        std::vector<std::string> tags;  // variadic (add/rm, or files query)
        std::vector<std::string> paths; // variadic (add/rm, or tags query)
        bool list = false;
        bool yes = false;
        bool to_repo = true;
        bool from_repo = false;
    } opts;

    int exit_code = 0;

    CLI::App app{"pfm - a tag-based file organizer"};
    app.require_subcommand(1);
    app.add_option("--repo", opts.repo, "repository path (default: auto-detected from cwd)");

    // Shared helper: construct a manager bound to the resolved repo.
    auto with_manager = [&](auto &&body) {
        const auto repo = resolve_repo_path(opts.repo);
        if (repo.empty())
        {
            std::cerr << "error: not inside a repository (or pass --repo)\n";
            exit_code = 1;
            return;
        }
        try
        {
            PolyFileManager mgr(repo);
            body(mgr);
        }
        catch (const std::exception &e)
        {
            std::cerr << "error: " << e.what() << '\n';
            exit_code = 1;
        }
    };

    auto add_repo_option = [&](CLI::App &sub) {
        sub.add_option("--repo", opts.repo, "repository path (default: auto-detected from cwd)");
    };

    // ----- init / destroy ------------------------------------------------------

    auto *init = app.add_subcommand("init", "create a repository");
    add_repo_option(*init);
    init->callback([&] {
        auto repo = opts.repo.empty() ? fs::current_path() : fs::path(opts.repo);
        if (repo.is_relative())
        {
            repo = fs::current_path() / repo;
        }
        repo = fs::absolute(repo).lexically_normal();
        if (PolyFileManager::is_repository(repo))
        {
            std::cerr << "error: already a repository at " << repo.string() << '\n';
            exit_code = 1;
            return;
        }
        std::error_code ec;
        fs::create_directories(repo, ec);
        if (ec || !PolyFileManager::create_repository(repo))
        {
            std::cerr << "error: failed to create repository\n";
            exit_code = 1;
            return;
        }
        std::cout << "initialized repository at " << repo.string() << '\n';
    });

    auto *destroy = app.add_subcommand("destroy", "delete a repository");
    add_repo_option(*destroy);
    destroy->add_flag("-y,--yes", opts.yes, "confirm deletion");
    destroy->callback([&] {
        const auto repo = resolve_repo_path(opts.repo);
        if (repo.empty())
        {
            std::cerr << "error: not inside a repository (or pass --repo)\n";
            exit_code = 1;
            return;
        }
        if (!opts.yes)
        {
            std::cerr << "error: pass --yes to confirm deletion of " << repo.string() << '\n';
            exit_code = 1;
            return;
        }
        std::error_code ec;
        for (auto it = fs::directory_iterator(repo, ec); it != fs::directory_iterator(); ++it)
        {
            if (ec)
            {
                std::cerr << "error: " << ec.message() << '\n';
                exit_code = 1;
                return;
            }
            if (it->is_directory(ec))
            {
                fs::remove_all(it->path(), ec);
            }
        }
        fs::remove(repo / ".poly_file_manager", ec);
        std::cout << "destroyed repository at " << repo.string() << '\n';
    });

    // ----- tag manage ----------------------------------------------------------

    auto *tag = app.add_subcommand("tag", "manage tags");
    tag->require_subcommand(1);

    auto *tag_add = tag->add_subcommand("add", "add one or more tags");
    add_repo_option(*tag_add);
    tag_add->add_option("name", opts.tags)->required();
    tag_add->callback([&] { with_manager([&](auto &mgr) {
        for (const auto &t : opts.tags)
        {
            if (!mgr.add_tag(t))
            {
                std::cerr << "error: tag '" << t << "' already exists\n";
                exit_code = 1;
                return;
            }
        }
        if (!mgr.save())
        {
            std::cerr << "error: failed to save\n";
            exit_code = 1;
            return;
        }
        std::cout << "added " << opts.tags.size() << " tag(s)\n";
    }); });

    auto *tag_rm = tag->add_subcommand("rm", "remove one or more tags");
    add_repo_option(*tag_rm);
    tag_rm->add_option("name", opts.tags)->required();
    tag_rm->callback([&] { with_manager([&](auto &mgr) {
        for (const auto &t : opts.tags)
        {
            if (!mgr.remove_tag(t))
            {
                std::cerr << "error: failed to remove tag '" << t << "'\n";
                exit_code = 1;
                return;
            }
        }
        if (!mgr.save())
        {
            std::cerr << "error: failed to save\n";
            exit_code = 1;
            return;
        }
        std::cout << "removed " << opts.tags.size() << " tag(s)\n";
    }); });

    auto *tag_mv = tag->add_subcommand("mv", "rename a tag");
    add_repo_option(*tag_mv);
    tag_mv->add_option("old", opts.old_name)->required();
    tag_mv->add_option("new", opts.new_name)->required();
    tag_mv->callback([&] { with_manager([&](auto &mgr) {
        if (!mgr.rename_tag(opts.old_name, opts.new_name) || !mgr.save())
        {
            std::cerr << "error: failed to rename tag\n";
            exit_code = 1;
            return;
        }
        std::cout << "renamed tag '" << opts.old_name << "' -> '" << opts.new_name << "'\n";
    }); });

    auto *tag_ls = tag->add_subcommand("ls", "list all tags");
    add_repo_option(*tag_ls);
    tag_ls->callback([&] { with_manager([&](auto &mgr) {
        for (const auto &t : mgr.tags())
        {
            std::cout << t << '\n';
        }
    }); });

    // ----- path manage ---------------------------------------------------------

    auto *path = app.add_subcommand("path", "manage file paths");
    path->require_subcommand(1);

    auto *path_add = path->add_subcommand("add", "add one or more paths");
    add_repo_option(*path_add);
    path_add->add_option("path", opts.paths)->required();
    path_add->callback([&] { with_manager([&](auto &mgr) {
        for (const auto &p : opts.paths)
        {
            if (!mgr.add_path(fs::path(p)))
            {
                std::cerr << "error: failed to add path '" << p << "'\n";
                exit_code = 1;
                return;
            }
        }
        if (!mgr.save())
        {
            std::cerr << "error: failed to save\n";
            exit_code = 1;
            return;
        }
        std::cout << "added " << opts.paths.size() << " path(s)\n";
    }); });

    auto *path_rm = path->add_subcommand("rm", "remove one or more paths");
    add_repo_option(*path_rm);
    path_rm->add_option("path", opts.paths)->required();
    path_rm->callback([&] { with_manager([&](auto &mgr) {
        for (const auto &p : opts.paths)
        {
            if (!mgr.remove_path(fs::path(p)))
            {
                std::cerr << "error: failed to remove path '" << p << "'\n";
                exit_code = 1;
                return;
            }
        }
        if (!mgr.save())
        {
            std::cerr << "error: failed to save\n";
            exit_code = 1;
            return;
        }
        std::cout << "removed " << opts.paths.size() << " path(s)\n";
    }); });

    auto *path_mv = path->add_subcommand("mv", "replace a registered file path");
    add_repo_option(*path_mv);
    path_mv->add_option("old", opts.old_name)->required();
    path_mv->add_option("new", opts.new_name)->required();
    path_mv->callback([&] { with_manager([&](auto &mgr) {
        if (!fs::exists(fs::path(opts.new_name)))
        {
            std::cerr << "error: target path does not exist\n";
            exit_code = 1;
            return;
        }
        if (!mgr.replace_path(fs::path(opts.old_name), fs::path(opts.new_name)) || !mgr.save())
        {
            std::cerr << "error: failed to replace file path\n";
            exit_code = 1;
            return;
        }
        std::cout << "replaced path '" << opts.old_name << "' -> '" << opts.new_name << "'\n";
    }); });

    auto *path_ls = path->add_subcommand("ls", "list all paths");
    add_repo_option(*path_ls);
    path_ls->callback([&] { with_manager([&](auto &mgr) {
        for (const auto &p : mgr.paths())
        {
            std::cout << p.string() << '\n';
        }
    }); });

    // ----- associations --------------------------------------------------------

    auto *assign = app.add_subcommand("assign", "assign a tag to one or more paths");
    add_repo_option(*assign);
    assign->add_option("tag", opts.tag)->required();
    assign->add_option("path", opts.paths);
    assign->callback([&] { with_manager([&](auto &mgr) {
        for (const auto &p : opts.paths)
        {
            // convenience: register the tag/path first if they are missing
            mgr.add_tag(opts.tag);
            mgr.add_path(fs::path(p));
            if (!mgr.assign_tag(fs::path(p), opts.tag))
            {
                std::cerr << "error: failed to assign '" << opts.tag << "' to '" << p << "'\n";
                exit_code = 1;
                return;
            }
        }
        if (!mgr.save())
        {
            std::cerr << "error: failed to save\n";
            exit_code = 1;
            return;
        }
        std::cout << "assigned tag '" << opts.tag << "' to " << opts.paths.size() << " path(s)\n";
    }); });

    auto *unassign = app.add_subcommand("unassign", "remove a tag from one or more paths");
    add_repo_option(*unassign);
    unassign->add_option("tag", opts.tag)->required();
    unassign->add_option("path", opts.paths);
    unassign->callback([&] { with_manager([&](auto &mgr) {
        for (const auto &p : opts.paths)
        {
            if (!mgr.unassign_tag(fs::path(p), opts.tag))
            {
                std::cerr << "error: failed to unassign '" << opts.tag << "' from '" << p << "'\n";
                exit_code = 1;
                return;
            }
        }
        if (!mgr.save())
        {
            std::cerr << "error: failed to save\n";
            exit_code = 1;
            return;
        }
        std::cout << "unassigned tag '" << opts.tag << "' from " << opts.paths.size() << " path(s)\n";
    }); });

    // ----- queries -------------------------------------------------------------

    auto *files = app.add_subcommand("files", "find files carrying all given tags");
    add_repo_option(*files);
    files->add_option("tag", opts.tags)->required();
    files->callback([&] { with_manager([&](auto &mgr) {
        for (const auto &p : mgr.paths_with_tags(opts.tags))
        {
            std::cout << p.string() << '\n';
        }
    }); });

    auto *tags_cmd = app.add_subcommand("tags", "show tags carried by the given paths");
    add_repo_option(*tags_cmd);
    tags_cmd->add_option("path", opts.paths)->required();
    tags_cmd->callback([&] { with_manager([&](auto &mgr) {
        std::vector<fs::path> ps;
        for (const auto &p : opts.paths)
        {
            ps.emplace_back(p);
        }
        for (const auto &t : mgr.tags_with_paths(ps))
        {
            std::cout << t << '\n';
        }
    }); });

    // ----- consistency ---------------------------------------------------------

    auto *check = app.add_subcommand("check", "check whether data and repository are in sync");
    add_repo_option(*check);
    check->callback([&] { with_manager([&](auto &mgr) {
        if (mgr.is_consistent())
        {
            std::cout << "consistent\n";
        }
        else
        {
            std::cout << "inconsistent\n";
            exit_code = 1;
        }
    }); });

    auto *sync = app.add_subcommand("sync", "make data and repository consistent");
    add_repo_option(*sync);
    sync->add_flag("--to-repo", opts.to_repo, "rebuild repository to match data (default)");
    sync->add_flag("--from-repo", opts.from_repo, "rebuild data to match repository");
    sync->get_option("--to-repo")->excludes("--from-repo");
    sync->get_option("--from-repo")->excludes("--to-repo");
    sync->callback([&] { with_manager([&](auto &mgr) {
        const bool ok = opts.from_repo ? (mgr.rebuild_from_repository() && mgr.save()) : mgr.sync();
        if (!ok)
        {
            std::cerr << "error: sync failed\n";
            exit_code = 1;
            return;
        }
        std::cout << (opts.from_repo ? "rebuilt data from repository\n" : "rebuilt repository from data\n");
    }); });

    try
    {
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError &e)
    {
        return app.exit(e);
    }
    catch (const std::exception &e)
    {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
    return exit_code;
}
