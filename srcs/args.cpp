#include "webserv.hpp"



int Webserv::args(int ac, char **av, char **envp)
{
    if (ac > 2)
    {
        std::cerr << std::endl << "\033[1;31mUsage: " << av[0] << " [configuration file]\033[0m" << std::endl;
        return (1);
    }
    else if (ac == 1)
    {
        std::cout << std::endl <<  "\033[1;35mLoading default config file \033[0m: config/default.json" << std::endl;
        _configPath = "config/default.json";
    }
    else
    {
        std::cout << std::endl << "\033[1;35mLoading config file : \033[0m" << av[1] << std::endl;
        _configPath = av[1];
    }
    w_wenvp = envp;

    return (0);
}