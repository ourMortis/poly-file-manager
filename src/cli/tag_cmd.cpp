#include "tag_cmd.hpp"

namespace poly::cli
{

CommandResult TagCmd::execute()
{
    auto manager = create_manager();

    for (const auto &tag : add_tags_)
    {
        manager.add_tag(tag);
    }

    for (const auto &tag : remove_tags_)
    {
        manager.remove_tag(tag);
    }

    if (!modify_tag_.first.empty())
    {
        if (!manager.rename_tag(modify_tag_.first, modify_tag_.second))
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