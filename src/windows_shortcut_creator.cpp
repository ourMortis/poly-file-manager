#include "windows_shortcut_creator.hpp"

const CLSID CLSID_ShellLink = {0x00021401, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
const IID IID_IShellLinkW = {0x000214F9, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
const IID IID_IPersistFile = {0x0000010b, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

// 实现create方法
bool ShortcutCreator::create(const std::filesystem::path &targetPath, const std::filesystem::path &shortcutPath)
{

    std::string str = targetPath.string();
    std::replace(str.begin(), str.end(), '/', '\\');
    std::filesystem::path win_target_path{std::move(str)};

    std::wstring targetWstr = win_target_path.wstring();
    std::wstring shortcutWstr = shortcutPath.wstring();

    HRESULT hr;
    // 初始化COM库（COINIT_APARTMENTTHREADED是Shell API推荐的线程模型）
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        std::cerr << "COM库初始化失败，错误码：" << hr << std::endl;
        return false;
    }

    bool bResult = false;
    IShellLinkW *pShellLink = nullptr;
    IPersistFile *pPersistFile = nullptr;

    // 创建IShellLink接口实例
    hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void **)&pShellLink);
    if (SUCCEEDED(hr))
    {
        // 设置目标路径（核心，必须）
        hr = pShellLink->SetPath(targetWstr.c_str());
        if (FAILED(hr))
        {
            std::cerr << "设置目标路径失败，错误码：" << hr << std::endl;
            goto Cleanup;
        }

        // 设置工作目录为目标路径（增强兼容性）
        hr = pShellLink->SetWorkingDirectory(targetWstr.c_str());
        if (FAILED(hr))
        {
            std::cerr << "设置工作目录失败，错误码：" << hr << std::endl;
            goto Cleanup;
        }

        // 设置窗口样式（默认普通窗口，对文件夹无效）
        hr = pShellLink->SetShowCmd(SW_SHOWNORMAL);
        if (FAILED(hr))
        {
            std::cerr << "设置窗口样式失败，错误码：" << hr << std::endl;
            goto Cleanup;
        }

        // 获取IPersistFile接口，用于保存快捷方式
        hr = pShellLink->QueryInterface(IID_IPersistFile, (void **)&pPersistFile);
        if (SUCCEEDED(hr))
        {
            // 保存快捷方式文件
            hr = pPersistFile->Save(shortcutWstr.c_str(), TRUE);
            if (SUCCEEDED(hr))
            {
                bResult = true;
                std::cout << "快捷方式创建成功 -> " << shortcutPath << std::endl;
            }
            else
            {
                std::cerr << "保存快捷方式失败，错误码：" << hr << std::endl;
            }
        }
        else
        {
            std::cerr << "获取IPersistFile接口失败，错误码：" << hr << std::endl;
        }
    }
    else
    {
        std::cerr << "创建IShellLink实例失败，错误码：" << hr << std::endl;
    }

Cleanup:
    // 释放接口资源
    if (pPersistFile)
    {
        pPersistFile->Release();
    }
    if (pShellLink)
    {
        pShellLink->Release();
    }

    // 释放COM库
    CoUninitialize();

    return bResult;
}