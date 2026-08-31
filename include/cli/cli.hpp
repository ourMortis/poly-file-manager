#ifndef CLI_HPP
#define CLI_HPP

/**
 * @brief Command-line front-end for PolyFileManager.
 *
 * Built on CLI11 with a git-style subcommand layout. The repository path is
 * auto-detected from the current directory (walking up like git) unless an
 * explicit --repo is provided.
 */
class Cli
{
  public:
    int run(int argc, char **argv);
};

#endif // CLI_HPP
