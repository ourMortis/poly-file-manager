#include "cli_parser.hpp"
#include "cmd.hpp"
#include "manager_cmd.hpp"
#include "tag_cmd.hpp"
#include "path_cmd.hpp"
#include <exception>
#include <iostream>
#include <system_error>

int main(int argc, char **argv)
{
    poly::cli::CommandError result;
    try
    {
        poly::cli::CliParser parser(argc, argv);
        auto command = parser.parse();
        if (command)
        {
            result = command->execute();
            if (result.code != poly::cli::ErrorCode::Success)
            {
                std::cerr << result.message;
            }
            else
            {
                std::cout << result.message;
            }
        }
    }
    catch (const CLI::ParseError &e)
    {
        if (e.get_exit_code() == static_cast<int>(CLI::ExitCodes::Success))
        {
            return 0;
        }
        else
        {
            std::cerr << e.what() << std::endl;
            return e.get_exit_code();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return static_cast<int>(result.code);
}