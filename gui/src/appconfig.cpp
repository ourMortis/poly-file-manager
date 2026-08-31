#include "appconfig.hpp"

#include "json.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>

using json = nlohmann::json;

namespace
{

// Return the config directory: prefer AppData, then the first writable candidate.
std::filesystem::path configDir()
{
    std::vector<std::filesystem::path> candidates;

    if (const char *p = std::getenv("LOCALAPPDATA"))
    {
        candidates.emplace_back(std::filesystem::path(p) / "PolyFileManager");
    }
    if (const char *p = std::getenv("APPDATA"))
    {
        candidates.emplace_back(std::filesystem::path(p) / "PolyFileManager");
    }
    if (const char *p = std::getenv("USERPROFILE"))
    {
        candidates.emplace_back(std::filesystem::path(p) / ".config" / "PolyFileManager");
    }
    candidates.emplace_back(std::filesystem::temp_directory_path() / "PolyFileManager");

    for (const auto &dir : candidates)
    {
        if (dir.empty())
        {
            continue;
        }
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        if (ec)
        {
            continue;
        }
        const auto probe = dir / ".probe";
        std::ofstream out(probe, std::ios::out);
        if (out.is_open())
        {
            out.close();
            std::filesystem::remove(probe, ec);
            return dir;
        }
    }
    return std::filesystem::temp_directory_path() / "PolyFileManager";
}

} // namespace

AppConfig::AppConfig() : file_(configDir() / "PolyFileManager.json")
{
    load();
}

void AppConfig::load()
{
    std::error_code ec;
    if (!std::filesystem::is_regular_file(file_, ec))
    {
        return;
    }
    std::ifstream in(file_);
    if (!in.is_open())
    {
        return;
    }

    json root;
    try
    {
        in >> root;
    }
    catch (...)
    {
        return;
    }

    if (root.contains("last") && root["last"].is_string())
    {
        last_ = std::filesystem::path(root["last"].get<std::string>());
    }
    if (root.contains("history") && root["history"].is_array())
    {
        for (const auto &entry : root["history"])
        {
            if (entry.is_string())
            {
                history_.push_back(std::filesystem::path(entry.get<std::string>()));
            }
        }
    }
    if (root.contains("settings") && root["settings"].is_object())
    {
        for (const auto &[key, value] : root["settings"].items())
        {
            if (value.is_string())
            {
                settings_[key] = value.get<std::string>();
            }
        }
    }
}

void AppConfig::save() const
{
    json root;
    root["last"] = last_.string();
    root["history"] = json::array();
    for (const auto &repo : history_)
    {
        root["history"].push_back(repo.string());
    }
    root["settings"] = json::object();
    for (const auto &[key, value] : settings_)
    {
        root["settings"][key] = value;
    }

    std::ofstream out(file_, std::ios::out | std::ios::trunc);
    if (out.is_open())
    {
        out << root.dump(2);
    }
}

void AppConfig::setLastRepository(const std::filesystem::path &repo)
{
    last_ = repo;
    save();
}

void AppConfig::clearLastRepository()
{
    last_.clear();
    save();
}

void AppConfig::addToHistory(const std::filesystem::path &repo)
{
    if (std::find(history_.begin(), history_.end(), repo) == history_.end())
    {
        history_.push_back(repo);
        save();
    }
}

void AppConfig::removeFromHistory(const std::filesystem::path &repo)
{
    auto it = std::remove(history_.begin(), history_.end(), repo);
    if (it != history_.end())
    {
        history_.erase(it, history_.end());
        save();
    }
}

std::string AppConfig::setting(const std::string &key) const
{
    const auto it = settings_.find(key);
    return it == settings_.end() ? std::string() : it->second;
}

void AppConfig::setSetting(const std::string &key, const std::string &value)
{
    settings_[key] = value;
    save();
}
