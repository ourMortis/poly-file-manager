#include "path_cmd.hpp"

namespace poly::cli
{

CommandResult PathCmd::execute()
{

    auto manager = create_manager();

    for (const auto &path : add_paths_)
    {
        manager.add_path(path);
    }

    for (const auto &path : remove_paths_)
    {
        manager.remove_path(path);
    }

    if (!modify_path_.first.empty())
    {
        if (!manager.rename_path(modify_path_.first, modify_path_.second))
        {
            return CommandResult::BusinessError;
        }
    }

    if (!manager.save())
    {
        return CommandResult::BusinessError;
    }
    return CommandResult::Success;
}

} // namespace poly::cli