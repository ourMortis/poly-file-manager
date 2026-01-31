#ifndef CLI_PARSER_HPP
#define CLI_PARSER_HPP

#include "cmd.hpp"
#include <memory>
#include <CLI11.hpp>

namespace poly::cli {

class CliParser {
public:
    CliParser(int argc, char** argv);

    std::unique_ptr<Cmd> parse();

private:
    CLI::App app_;
    int argc_;
    char** argv_;

    std::string repo_path_;

    std::vector<std::string> tag_add_;
    std::vector<std::string> tag_remove_;
    std::pair<std::string, std::string> tag_modify_;

    std::vector<std::string> path_add_;
    std::vector<std::string> path_remove_;
    std::pair<std::string, std::string> path_modify_;

    bool list_flag_ = false;

    std::vector<std::string> tags_;
    std::vector<std::string> paths_;
    std::vector<std::string> tag_or_path_;
    bool remove_flag_ = false;
    bool create_flag_ = false;

    void init_cli() noexcept;
};

} // namespace poly::cli


#endif // CLI_PARSER_HPP