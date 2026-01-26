#include "path_cmd.hpp"
#include "cmd.hpp"

namespace poly::cli
{

CommandError PathCmd::execute()
{
    try
    {
        auto manager = create_manager();

        for (const auto &path : add_paths_)
        {
            manager.add_path(path);
        }

        for (const auto &path : remove_paths_)
        {
            if (manager.remove_path(path) == 0)
            {
                return {ErrorCode::Business_RemovePathFailed, "Failed to remove path"};
            }
        }

        if (!modify_path_.first.empty())
        {
            if (!manager.rename_path(modify_path_.first, modify_path_.second))
            {
                return {ErrorCode::Business_RenamePathFailed, "Failed to rename path"};
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