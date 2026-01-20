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
            return static_cast<int>(result);
        }
        return static_cast<int>(poly::cli::CommandResult::Success);
    }
    catch (const CLI::ParseError &e)
    {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 2;
    }
}