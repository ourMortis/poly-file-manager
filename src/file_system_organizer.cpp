#include "file_system_organizer.h"



FileSystemOrganizer::FileSystemOrganizer(const FilePath& path)
{
    if(!path.is_absolute() || !std::filesystem::is_directory(path))
    {
        throw std::invalid_argument("Path is not absolute or not a dirctory: " + path.string());
    }
    repo_path_ = path;
}

FileSystemOrganizer::~FileSystemOrganizer()
{

}

void FileSystemOrganizer::create_category_dirs(const std::vector<std::string> &category_names)
{
    for(const auto& name : category_names)
    {
        FilePath path = repo_path_ / name;
        std::filesystem::create_directory(path);

        if(!std::filesystem::is_directory(path))
        {
            throw std::runtime_error("create_directory(s) failed: " + path.string());
        }
    }
}

void FileSystemOrganizer::remove_category_dir(const std::string &category_name)
{

}

void FileSystemOrganizer::rename_category_dir(const std::string &old_name, const std::string &new_name)
{

}

void FileSystemOrganizer::create_symlink_in_category(const std::string &category_name, const std::vector<FilePath> &paths)
{

}

void FileSystemOrganizer::create_symlink_in_category(const std::string &category_name, const FilePath &path)
{

}

void FileSystemOrganizer::remove_symlink_in_category(const std::string &category_name, const std::vector<FilePath> &paths)
{

}

void FileSystemOrganizer::remove_symlink_in_category(const std::string &category_name, const FilePath &path)
{

}

std::vector<FilePath> FileSystemOrganizer::get_symlinks_in_category(const std::string &category_name)
{
    return {};
}