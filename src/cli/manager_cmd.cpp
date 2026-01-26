#include "manager_cmd.hpp"
#include "cmd.hpp"

namespace poly::cli
{
CommandError ManagerCmd::execute()
{
    try
    {
        if (!create_flag_ && (tag_or_path_.empty() || (tags_.empty() && paths_.empty())))
        {
            return {ErrorCode::InvalidArgs_MissingParameter,
                    "Missing parameter! Use: tag -p path... or path -t tag..."};
        }

        auto manager = create_manager();

        if (create_flag_)
        {
            if (!manager.save())
            {
                return {ErrorCode::Business_CreateRepositoryFailed, "Failed to create repository"};
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
                        return {ErrorCode::Business_AssignTagFailed, "Failed to remove tag from file"};
                    }
                }
            }
            else
            {
                for (const auto &tag : tags_)
                {
                    if (!manager.assign_tag_to_file(tag_or_path_, tag))
                    {
                        return {ErrorCode::Business_AssignTagFailed, "Failed to assign tag to file"};
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
                        return {ErrorCode::Business_AssignTagFailed, "Failed to remove tag from file"};
                    }
                }
            }
            else
            {
                for (const auto &path : paths_)
                {
                    if (!manager.assign_tag_to_file(path, tag_or_path_))
                    {
                        return {ErrorCode::Business_AssignTagFailed, "Failed to assign tag to file"};
                    }
                }
            }
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