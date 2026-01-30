#include "tag_cmd.hpp"
#include "cmd.hpp"

namespace poly::cli
{

CommandError TagCmd::execute()
{
    try
    {
        if (!PolyFileManager::is_repository(repo_path_))
        {
            return {ErrorCode::InvalidArgs_InvalidPath, "[Error]The path does not point to the repository\n"};
        }
        auto manager = create_manager();
        std::string message;
        for (const auto &tag : add_tags_)
        {
            if (!manager.add_tag(tag))
            {
                if (!manager.save())
                {
                    return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
                }
                return {ErrorCode::Business_AddTagFailed, message + "[Error]" + tag + "(The tag may already exist)\n"};
            }
            message += "[new]" + tag + '\n';
        }

        for (const auto &tag : remove_tags_)
        {
            if (!manager.remove_tag(tag))
            {
                if (!manager.save())
                {
                    return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
                }
                return {ErrorCode::Business_RemoveTagFailed,
                        message + "[Error]Failed to remove tag: " + tag + '\n'};
            }
            message += "[Removed]" + tag + '\n';
        }

        if (!modify_tag_.first.empty())
        {
            if (!manager.rename_tag(modify_tag_.first, modify_tag_.second))
            {
                if (!manager.save())
                {
                    return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
                }
                return {ErrorCode::Business_RenameTagFailed, message + "[Error]Failed to rename tag\n"};
            }
            message += "[Renamed]" + modify_tag_.first + " -> " + modify_tag_.second + '\n';
        }

        if (!manager.save())
        {
            return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
        }
        return {ErrorCode::Success, message};
    }
    catch (const std::system_error &e)
    {
        return {ErrorCode::System_FileSystemError, "File system error: " + std::string(e.what())};
    }
    catch (const std::exception &e)
    {
        return {ErrorCode::System_Unknown, "Unexpected error: " + std::string(e.what())};
    }
}

} // namespace poly::cli