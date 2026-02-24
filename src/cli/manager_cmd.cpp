#include "manager_cmd.hpp"
#include "cmd.hpp"
#include "poly_file_manager.hpp"

namespace poly::cli
{
CommandError ManagerCmd::execute()
{
    try
    {
        if ((create_flag_ && (!tag_or_path_.empty() || !tags_.empty() || !paths_.empty())) ||
            (!tags_.empty() && !paths_.empty()) || (tag_or_path_.size() > 1 && (!paths_.empty() || !tags_.empty())))
        {
            return {ErrorCode::InvalidArgs_ConflictOption, "[Error]ConflictOption\n"};
        }

        std::string message;

        if (create_flag_)
        {
            auto manager = create_manager();
            if (repo_path_.is_relative())
            {
                return {ErrorCode::InvalidArgs_InvalidPath,
                        "[Error]The repository path should not be a relative path\n"};
            }
            for (const auto &entry : std::filesystem::directory_iterator(repo_path_))
            {
                const auto &entry_path = entry.path();
                if (entry_path.filename() == "." || entry_path.filename() == "..")
                {
                    continue;
                }
                return {ErrorCode::InvalidArgs_InvalidPath, "[Error]The repository directory is not empty\n"};
            }
            if (!manager.save())
            {
                return {ErrorCode::Business_CreateRepositoryFailed, "[Error]Failed to create repository\n"};
            }
            message += "[Created]" + repo_path_.string() + '\n';
            if (!manager.save())
            {
                return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
            }
        }

        if (!PolyFileManager::is_repository(repo_path_))
        {
            return {ErrorCode::InvalidArgs_InvalidPath, "[Error]The path does not point to the repository\n"};
        }

        auto manager = create_manager();

        if (!tags_.empty() && tag_or_path_.size() == 1)
        {
            if (remove_flag_)
            {
                for (const auto &tag : tags_)
                {
                    if (!manager.remove_tag_from_file(tag_or_path_[0], tag))
                    {
                        if (!manager.save())
                        {
                            return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
                        }
                        return {ErrorCode::Business_RemoveAssociationFailed,
                                message + "[Error]Failed to remove tag (" + tag + ") from file: " + tag_or_path_[0] +
                                    "\n"};
                    }
                    message += "[Removed]" + tag_or_path_[0] + ": " + tag + '\n';
                }
            }
            else
            {
                for (const auto &tag : tags_)
                {
                    if (!manager.assign_tag_to_file(tag_or_path_[0], tag))
                    {
                        if (!manager.save())
                        {
                            return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
                        }
                        return {ErrorCode::Business_AssignTagFailed, message + "[Error]Failed to assign tag (" + tag +
                                                                         ") to file: " + tag_or_path_[0] + "\n"};
                    }
                    message += "[New]" + tag_or_path_[0] + ": " + tag + '\n';
                }
            }
        }
        else if (!paths_.empty() && tag_or_path_.size() == 1)
        {
            if (remove_flag_)
            {
                for (const auto &path : paths_)
                {
                    if (!manager.remove_tag_from_file(path, tag_or_path_[0]))
                    {
                        if (!manager.save())
                        {
                            return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
                        }
                        return {ErrorCode::Business_RemoveAssociationFailed,
                                "Failed to remove tag (" + tag_or_path_[0] + ") from file: " + path + "\n"};
                    }
                    message += "[Removed]" + path + ": " + tag_or_path_[0] + '\n';
                }
            }
            else
            {
                for (const auto &path : paths_)
                {
                    if (!manager.assign_tag_to_file(path, tag_or_path_[0]))
                    {
                        if (!manager.save())
                        {
                            return {ErrorCode::Business_SaveResultFailed, "[Error]Failed to save result\n"};
                        }
                        return {ErrorCode::Business_AssignTagFailed, message + "[Error]Failed to assign tag (" +
                                                                         tag_or_path_[0] + ") to file: " + path + "\n"};
                    }
                    message += "[New]" + path + ": " + tag_or_path_[0] + '\n';
                }
            }
        }

        else if (tag_or_path_.size() > 0)
        {
            auto result = manager.get_paths_with_tags(tag_or_path_);
            for (const auto &path : result)
            {
                message += path.string() + '\n';
            }
            return {ErrorCode::Success, message};
        }
        else if (!paths_.empty())
        {
            auto result = manager.get_tags_with_paths(paths_);
            int cnt = 0;
            for (const auto &path : result)
            {
                cnt++;
                message += path + ' ';
                if (cnt % 5 == 0)
                {
                    message += '\n';
                }
            }
            if (cnt % 5)
            {
                message += '\n';
            }
            return {ErrorCode::Success, message};
        }
        else if (check_consistency_flag_)
        {
            if (manager.is_data_consistent_with_repository())
            {
                message += "The content of repository is consistent with data\n";
            }
            else
            {
                message += "[Warning]Inconsistent content was found, use option \"-s\" to synchronize content\n";
            }
            return {ErrorCode::Success, message};
        }
        else if (sync_repo_with_data_flag_)
        {
            if (manager.sync_data_with_repository())
            {
                message += "Synchronization successful\n";
            }
            else
            {
                message += "[Error]Synchronization failed\n";
            }
            return {ErrorCode::Success, message};
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