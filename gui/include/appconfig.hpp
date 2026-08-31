#ifndef APPCONFIG_HPP
#define APPCONFIG_HPP

#include <filesystem>
#include <map>
#include <string>
#include <vector>

/**
 * @brief Persistent application configuration stored as JSON.
 *
 * Records the last opened repository and the history of repositories that have
 * been opened. The file is rooted in the standard AppData location and falls
 * back to the first writable candidate if that is not accessible.
 */
class AppConfig
{
  public:
    AppConfig();

    [[nodiscard]] std::filesystem::path lastRepository() const { return last_; }
    void setLastRepository(const std::filesystem::path &repo);
    void clearLastRepository();

    [[nodiscard]] std::vector<std::filesystem::path> history() const { return history_; }
    void addToHistory(const std::filesystem::path &repo);
    void removeFromHistory(const std::filesystem::path &repo);

    [[nodiscard]] std::string setting(const std::string &key) const;
    void setSetting(const std::string &key, const std::string &value);

  private:
    std::filesystem::path locateFile() const;
    void load();
    void save() const;

    std::filesystem::path file_;
    std::filesystem::path last_;
    std::vector<std::filesystem::path> history_;
    std::map<std::string, std::string> settings_;
};

#endif // APPCONFIG_HPP
