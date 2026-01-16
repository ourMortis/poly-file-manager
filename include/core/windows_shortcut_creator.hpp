#ifndef WINDOWS_SHORTCUT_CREATOR_H
#define WINDOWS_SHORTCUT_CREATOR_H
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <shlobj.h>
#include <string>
#include <windows.h>


extern const CLSID CLSID_ShellLink;
extern const IID IID_IShellLinkW;
extern const IID IID_IPersistFile;

class ShortcutCreator
{
  public:
    ShortcutCreator() = default;
    ~ShortcutCreator() = default;
    ShortcutCreator(const ShortcutCreator &) = delete;
    ShortcutCreator &operator=(const ShortcutCreator &) = delete;

    /**
     * @brief 创建快捷方式
     * @param targetPath 目标文件/文件夹的路径（std::filesystem::path类型）
     * @param shortcutPath 快捷方式的保存路径（包含.lnk后缀，std::filesystem::path类型）
     * @return bool 创建成功返回true，失败返回false
     */
    bool create(const std::filesystem::path &targetPath, const std::filesystem::path &shortcutPath);
};

#endif // WINDOWS_SHORTCUT_CREATOR