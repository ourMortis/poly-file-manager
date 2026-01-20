#include "manager_cmd.hpp"
#include "cmd.hpp"

namespace poly::cli
{
CommandResult ManagerCmd::execute()
{
    if (!create_flag_ && (tag_or_path_.empty() || (tags_.empty() && paths_.empty())))
    {
        return CommandResult::InvalidArgs;
    }

    auto manager = create_manager();

    if (create_flag_)
    {
        if (!manager.save())
        {
            return CommandResult::BusinessError;
        }
    }
    else if (!tags_.empty())
    {
        if (remove_flag_)
        {
            for (const auto &tag : tags_)
            {
                if (!manager.remove_tag_from_file(tag_or_path_, tag))
                {
                    return CommandResult::BusinessError;
                }
            }
        }
        else
        {
            for (const auto &tag : tags_)
            {
                if (!manager.assign_tag_to_file(tag_or_path_, tag))
                {
                    return CommandResult::BusinessError;
                }
            }
        }
    }
    else
    {
        if (remove_flag_)
        {
            for (const auto &path : paths_)
            {
                if (!manager.remove_tag_from_file(path, tag_or_path_))
                {
                    return CommandResult::BusinessError;
                }
            }
        }
        else
        {
            for (const auto &path : paths_)
            {
                if (!manager.assign_tag_to_file(path, tag_or_path_))
                {
                    return CommandResult::BusinessError;
                }
            }
        }
    }
    return CommandResult::Success;
}
} // namespace poly::cli