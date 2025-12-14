#include "file_system_organizer.h"
#ifdef _WIN32
#include "windows_shortcut_creator.h"
#endif

FilePath FileSystemOrganizer::get_symlink_path(const std::string &category_name, const FilePath &target_path)
{
#ifdef _WIN32
    FilePath symlink_path = repo_path_ / category_name /
                             (target_path.filename().empty() ? target_path.parent_path().filename().string() + ".lnk" : target_path.filename().string() + ".lnk");
#else
    FilePath symlink_path = repo_path_ / category_name /
                            (target_path.filename().empty() ? target_path.parent_path().filename() : target_path.filename());
#endif
    return symlink_path;
}

FileSystemOrganizer::FileSystemOrganizer(const FilePath& path)
{
    if(!path.is_absolute() || !std::filesystem::is_directory(path))
    {
        throw std::invalid_argument("Path is not absolute or not a dirctory: " + path.string());
    }
    repo_path_ = path;
}

void FileSystemOrganizer::create_category_dirs(const std::vector<std::string> &category_names)
{
    for(const auto& name : category_names)
    {
        if(!std::filesystem::create_directory(repo_path_ / name))
        {
            throw std::runtime_error("Failed to create directory '" + (repo_path_ / name).string());
        }
    }
}

void FileSystemOrganizer::remove_category_dir(const std::string &category_name)
{
    std::filesystem::remove_all(repo_path_ / category_name);
}

void FileSystemOrganizer::rename_category_dir(const std::string &old_name, const std::string &new_name)
{
    std::filesystem::rename(repo_path_ / old_name, repo_path_ / new_name);
}

void FileSystemOrganizer::create_symlink_in_category(const std::string &category_name, const std::vector<FilePath> &paths)
{
    for(const auto& path : paths)
    {
        create_symlink_in_category(category_name, path);
    }
}

void FileSystemOrganizer::create_symlink_in_category(const std::string &category_name, const FilePath &path)
{
#ifdef _WIN32
    ShortcutCreator creator;
    if(!creator.create(path, get_symlink_path(category_name, path)))
    {
        throw std::runtime_error("Failed to create shortcut: " + get_symlink_path(category_name, path).string());
    }
#else   
    std::filesystem::create_directory_symlink(path, get_symlink_path(category_name, path));
#endif
}

void FileSystemOrganizer::remove_symlink_in_category(const std::string &category_name, const std::vector<FilePath> &paths)
{
    for(auto path : paths)
    {
        remove_symlink_in_category(category_name, path);
    }
}

void FileSystemOrganizer::remove_symlink_in_category(const std::string &category_name, const FilePath &path)
{
    std::filesystem::remove(get_symlink_path(category_name, path));
}

FilePath FileSystemOrganizer::get_repo_path() const
{
    return repo_path_;
}

std::vector<FilePath> FileSystemOrganizer::get_symlinks_in_category(const std::string &category_name)
{
    std::vector<FilePath> paths;
    for(const auto& entry : std::filesystem::directory_iterator(repo_path_ / category_name))
    {
        paths.push_back(entry.path());
    }
    return paths;
}