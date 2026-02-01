#include "windows_shortcut_creator.hpp"

const CLSID CLSID_ShellLink = {0x00021401, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
const IID IID_IShellLinkW = {0x000214F9, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
const IID IID_IPersistFile = {0x0000010b, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

bool ShortcutCreator::create(const std::filesystem::path &targetPath, const std::filesystem::path &shortcutPath)
{

    std::string str = targetPath.string();
    std::replace(str.begin(), str.end(), '/', '\\');
    std::filesystem::path win_target_path{std::move(str)};

    std::wstring targetWstr = win_target_path.wstring();
    std::wstring shortcutWstr = shortcutPath.wstring();

    HRESULT hr;
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        std::cerr << "Initialize COM library failed, error code: " << hr << '\n';
        return false;
    }

    bool bResult = false;
    IShellLinkW *pShellLink = nullptr;
    IPersistFile *pPersistFile = nullptr;

    // Instantiate IShellLink API
    hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void **)&pShellLink);
    if (SUCCEEDED(hr))
    {
        // set target path
        hr = pShellLink->SetPath(targetWstr.c_str());
        if (FAILED(hr))
        {
            std::cerr << "Set target path failed, error code: " << hr << '\n';
            goto Cleanup;
        }

        // Set the working directory as the target path (to enhance compatibility)
        hr = pShellLink->SetWorkingDirectory(targetWstr.c_str());
        if (FAILED(hr))
        {
            std::cerr << "Set working directory failed, error code: " << hr << '\n';
            goto Cleanup;
        }

        hr = pShellLink->SetShowCmd(SW_SHOWNORMAL);
        if (FAILED(hr))
        {
            std::cerr << "Set windows style failed, error code:" << hr << '\n';
            goto Cleanup;
        }

        // Get IPersistFile API
        hr = pShellLink->QueryInterface(IID_IPersistFile, (void **)&pPersistFile);
        if (SUCCEEDED(hr))
        {
            hr = pPersistFile->Save(shortcutWstr.c_str(), TRUE);
            if (SUCCEEDED(hr))
            {
                bResult = true;
                //std::cout << "Create shortcut success: " << shortcutPath << '\n';
            }
            else
            {
                std::cerr << "Create shortcut failed, error code: " << hr << '\n';
            }
        }
        else
        {
            std::cerr << "Get IPersistFile API failed, error code: " << hr << '\n';
        }
    }
    else
    {
        std::cerr << "Instantiate IShellLink failed, error code: " << hr << '\n';
    }

Cleanup:
    if (pPersistFile)
    {
        pPersistFile->Release();
    }
    if (pShellLink)
    {
        pShellLink->Release();
    }
    CoUninitialize();
    return bResult;
}