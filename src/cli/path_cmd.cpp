#include "path_cmd.hpp"
#include "cmd.hpp"
#include <filesystem>
#include <string>

namespace poly::cli
{

CommandError PathCmd::execute()
{
    try
    {
        if (!PolyFileManager::is_repository(repo_path_))
        {
            return {ErrorCode::InvalidArgs_InvalidPath, "[Error]The path does not point to the repository\n"};
        }
        auto manager = create_manager();
        std::string message;
        for (const auto &path : add_paths_)
        {
            if (!manager.add_path(path))
            {
                if (!manager.save())
                {
                    return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
                }
                return {ErrorCode::Business_AddPathFailed,
                        message + "[Error]" + path + "(The path may already exist or is not an absolute path)\n"};
            }
            message += "[new]" + path + '\n';
        }

        for (const auto &path : remove_paths_)
        {
            if (!manager.remove_path(path))
            {
                if (!manager.save())
                {
                    return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
                }
                return {ErrorCode::Business_RemovePathFailed,
                        message + "[Error]Failed to remove path: " + path + " \n"};
            }
            message += "[Removed]" + path + "\n";
        }

        if (!modify_path_.first.empty())
        {
            if (!std::filesystem::exists(modify_path_.second))
            {
                return {ErrorCode::Business_RenamePathFailed, message + "[Error]Failed to rename path(" + modify_path_.second + " dose not exist)\n"};
            }
            if (!manager.rename_path(modify_path_.first, modify_path_.second))
            {
                if (!manager.save())
                {
                    return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
                }
                return {ErrorCode::Business_RenamePathFailed, message + "[Error]Failed to rename path\n"};
            }
            message += "[Renamed]" + modify_path_.first + " -> " + modify_path_.second + '\n';
        }

        if (!manager.save())
        {
            return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
        }
        return {ErrorCode::Success, message};
    }
    catch (const std::system_error &e)
    {
        return {ErrorCode::System_FileSystemError, "[Error]File system error: " + std::string(e.what())};
    }
    catch (const std::exception &e)
    {
        return {ErrorCode::System_Unknown, "[Error]Unexpected error: " + std::string(e.what())};
    }
}

} // namespace poly::cli