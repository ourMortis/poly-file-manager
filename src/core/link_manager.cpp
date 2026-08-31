#include "link_manager.hpp"

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <algorithm>
#include <iostream>
#include <string>
#endif

#ifndef _WIN32
#include <chrono>
#endif

#ifdef _WIN32
// MinGW does not ship these GUID data symbols in its import libraries, so they
// must be provided explicitly (the byte patterns match the Shell Link IIDs).
const CLSID CLSID_ShellLink = {0x00021401, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
const IID IID_IShellLinkW = {0x000214F9, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
const IID IID_IPersistFile = {0x0000010B, 0x0000, 0x0000, {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};
#endif

namespace
{
#ifdef _WIN32
bool create_win_shortcut(const std::filesystem::path &target_path, const std::filesystem::path &shortcut_path)
{
    std::string str = target_path.string();
    std::replace(str.begin(), str.end(), '/', '\\');
    const std::filesystem::path win_target_path{std::move(str)};

    const std::wstring target_wstr = win_target_path.wstring();
    const std::wstring shortcut_wstr = shortcut_path.wstring();

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        std::cerr << "Initialize COM library failed, error code: " << hr << '\n';
        return false;
    }

    bool ok = false;
    IShellLinkW *shell_link = nullptr;
    IPersistFile *persist_file = nullptr;

    hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void **)&shell_link);
    if (SUCCEEDED(hr))
    {
        hr = shell_link->SetPath(target_wstr.c_str());
        if (FAILED(hr))
        {
            std::cerr << "Set target path failed, error code: " << hr << '\n';
            goto Cleanup;
        }

        hr = shell_link->SetWorkingDirectory(target_wstr.c_str());
        if (FAILED(hr))
        {
            std::cerr << "Set working directory failed, error code: " << hr << '\n';
            goto Cleanup;
        }

        hr = shell_link->SetShowCmd(SW_SHOWNORMAL);
        if (FAILED(hr))
        {
            std::cerr << "Set window style failed, error code: " << hr << '\n';
            goto Cleanup;
        }

        hr = shell_link->QueryInterface(IID_IPersistFile, (void **)&persist_file);
        if (SUCCEEDED(hr))
        {
            hr = persist_file->Save(shortcut_wstr.c_str(), TRUE);
            ok = SUCCEEDED(hr);
            if (!ok)
            {
                std::cerr << "Save shortcut failed, error code: " << hr << '\n';
            }
        }
        else
        {
            std::cerr << "QueryInterface IPersistFile failed, error code: " << hr << '\n';
        }
    }
    else
    {
        std::cerr << "CoCreateInstance IShellLink failed, error code: " << hr << '\n';
    }

Cleanup:
    if (persist_file)
    {
        persist_file->Release();
    }
    if (shell_link)
    {
        shell_link->Release();
    }
    CoUninitialize();
    return ok;
}
#endif
} // namespace

bool LinkManager::create(const std::filesystem::path &target_path, const std::filesystem::path &link_path)
{
#ifdef _WIN32
    return create_win_shortcut(target_path, link_path);
#else
    // Remove a stale link so create_symlink does not fail when it already exists.
    std::error_code ec;
    std::filesystem::remove(link_path, ec);
    return std::filesystem::create_symlink(target_path, link_path, ec);
#endif
}

bool LinkManager::remove(const std::filesystem::path &link_path)
{
    std::error_code ec;
    return std::filesystem::remove(link_path, ec) > 0;
}

bool LinkManager::exists(const std::filesystem::path &link_path)
{
    return std::filesystem::exists(link_path);
}

std::optional<std::filesystem::path> LinkManager::resolve(const std::filesystem::path &link_path)
{
#ifdef _WIN32
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        return std::nullopt;
    }

    std::optional<std::filesystem::path> target;
    IShellLinkW *shell_link = nullptr;
    IPersistFile *persist_file = nullptr;

    hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void **)&shell_link);
    if (SUCCEEDED(hr))
    {
        hr = shell_link->QueryInterface(IID_IPersistFile, (void **)&persist_file);
        if (SUCCEEDED(hr) && SUCCEEDED(persist_file->Load(link_path.wstring().c_str(), STGM_READ)))
        {
            wchar_t buffer[MAX_PATH] = {0};
            if (SUCCEEDED(shell_link->GetPath(buffer, MAX_PATH, nullptr, SLGP_UNCPRIORITY)))
            {
                target = std::filesystem::path(buffer);
            }
        }
    }

    if (persist_file)
    {
        persist_file->Release();
    }
    if (shell_link)
    {
        shell_link->Release();
    }
    CoUninitialize();
    return target;
#else
    std::error_code ec;
    auto target = std::filesystem::read_symlink(link_path, ec);
    return ec ? std::nullopt : std::optional<std::filesystem::path>(target);
#endif
}

bool LinkManager::is_supported(const std::filesystem::path &target_path)
{
    if (target_path.empty())
    {
        return false;
    }
#ifdef _WIN32
    // Shell Link (COM) creation is available on all supported Windows versions.
    return true;
#else
    // Best-effort: test whether the platform can create a symlink at all.
    std::error_code ec;
    const auto probe_dir = std::filesystem::temp_directory_path(ec);
    if (ec)
    {
        return false;
    }
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    const auto probe_link = probe_dir / ("pfm_probe_" + std::to_string(stamp));
    std::filesystem::remove(probe_link, ec);
    ec.clear();
    const bool ok = std::filesystem::create_symlink(target_path, probe_link, ec) == true;
    std::filesystem::remove(probe_link, ec);
    return ok;
#endif
}

std::filesystem::path LinkManager::link_path(const std::filesystem::path &repo,
                                             const std::string &category,
                                             const std::filesystem::path &target_path)
{
    std::filesystem::path name = target_path.filename();
    if (name.empty())
    {
        name = target_path.parent_path().filename();
    }
#ifdef _WIN32
    return repo / category / (name.string() + ".lnk");
#else
    return repo / category / name;
#endif
}
