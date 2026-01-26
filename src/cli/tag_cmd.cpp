#include "tag_cmd.hpp"
#include "cmd.hpp"

namespace poly::cli
{

CommandError TagCmd::execute()
{
    try
    {
        auto manager = create_manager();

        for (const auto &tag : add_tags_)
        {
            manager.add_tag(tag);
        }

        for (const auto &tag : remove_tags_)
        {
            if (manager.remove_tag(tag) == 0)
            {
                return {ErrorCode::Business_RemoveTagFailed, "Failed to remove tag"};
            }
        }

        if (!modify_tag_.first.empty())
        {
            if (!manager.rename_tag(modify_tag_.first, modify_tag_.second))
            {

                return {ErrorCode::Business_RenameTagFailed, "Failed to rename tag"};
            }
        }

        if (!manager.save())
        {

            return {ErrorCode::Business_SaveFileFailed, "Failed to save result"};
        }
        return {ErrorCode::Success, ""};
    }
    catch (const std::system_error &e)
    {
        return {ErrorCode::System_FileSystemError, "File system exception: " + std::string(e.what())};
    }
    catch (const std::exception &e)
    {
        return {ErrorCode::System_Unknown, "Unexpected error: " + std::string(e.what())};
    }
}

} // namespace poly::cli