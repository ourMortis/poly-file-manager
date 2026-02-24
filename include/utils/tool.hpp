#ifndef TOOL_H
#define TOOL_H

#ifdef _WIN32
#include <windows.h>
#endif

#include "common_types.hpp"
#include <iostream>
#include <shlobj.h>

class Tool
{
  public:
    static bool set_file_hidden(const FilePath &path)
    {
        return SetFileAttributesW(path.wstring().c_str(),
                                  GetFileAttributesW(path.wstring().c_str()) | FILE_ATTRIBUTE_HIDDEN) != 0;
    }
    static bool remove_file_hidden(const FilePath &path)
    {
        return SetFileAttributesW(path.wstring().c_str(),
                                  GetFileAttributesW(path.wstring().c_str()) & ~FILE_ATTRIBUTE_HIDDEN) != 0;
    }

    /// @returns The absolute path of the symbolic link, or the ".lnk" shortcut path on Windows
    static FilePath get_symlink_path(const FilePath &repo_path, const std::string &category_name,
                                     const FilePath &target_path)
    {
#ifdef _WIN32
        return repo_path / category_name /
               (target_path.filename().empty() ? target_path.parent_path().filename().string() + ".lnk"
                                               : target_path.filename().string() + ".lnk");
#else
        return repo_path_ / category_name /
               (target_path.filename().empty() ? target_path.parent_path().filename() : target_path.filename());
#endif
    }
#ifdef _WIN32
    /**
     * @brief 创建快捷方式
     * @param targetPath 目标文件/文件夹的路径（std::filesystem::path类型）
     * @param shortcutPath 快捷方式的保存路径（包含.lnk后缀，std::filesystem::path类型）
     * @return bool 创建成功返回true，失败返回false
     */
    static bool create_win_shortcut(const FilePath &targetPath, const FilePath &shortcutPath);
#endif
};






#endif // TOOL_H