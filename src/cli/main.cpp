#include "cli_parser.hpp"
#include "cmd.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    try
    {
        poly::cli::CliParser parser(argc, argv);
        auto command = parser.parse();

        if (command)
        {
            auto result = command->execute();
            return static_cast<int>(result.code);
        }
        return 0;
    }
    catch (const CLI::ParseError &e)
    {
        if (e.get_exit_code() == static_cast<int>(CLI::ExitCodes::Success))
        {
            return 0;
        }
        else
        {
            std::cerr << "[ERROR]: " << e.what() << std::endl;
            return e.get_exit_code();
        }
    }
    catch (const poly::cli::CommandError &e)
    {
        std::cerr << "[ERROR]: " << e.message << std::endl;
        return static_cast<int>(e.code);
    }
}