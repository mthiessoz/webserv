#include "webserv.hpp"
#include "ParserJSON.hpp"
#include "server_conf.hpp"

int main(int ac, char **av, char **envp)
{
    Webserv ws;

    // Init args
    if (ws.args(ac, av, envp))
        return (1);

    // Parsing config file
    if (parse_configuration(ws._configPath, ws.w_config))
		{
			if (ws._configPath == "config/default.json")
			{
				std::cerr << "\033[1;31mconfig/default.json can't load\033[0m" << std::endl;
				return (1);
			}
			if (parse_configuration("config/default.json", ws.w_config))
			{
				std::cerr << "\033[1;31mconfig/default.json can't load\033[0m" << std::endl;
				return (1);
			}
			else
				std::cout << std::endl <<  "\033[1;35mLoading default config file \033[0m: config/default.json" << std::endl;
		}

    ws.w_client_addr.reserve(ws.w_config.size());

    sockaddr socket_address;;
    if (ws.socketBinding())
        return (1);
    std::cout << std::endl
              << "\033[1;32mWebserv launching... start logs :\033[0m" << std::endl
              << std::endl;
    while (true)
    {
        // wait for activity on the file descriptors using select()
        fd_set temp__fds = ws.w_fds;
        int ret = select(ws.w_max_fd + 1, &temp__fds, NULL, NULL, NULL);
        if (ret == -1)
        {
            std::cerr << "Error in select()" << std::endl;
            break;
        }
        // iterate over the file descriptors to check for activity
        for (unsigned int i = 0; i < ws.w_config.size(); i++)
        {
            if (FD_ISSET(ws.w_sockfd[i], &temp__fds))
            {
                // accept the incoming connection
                int sockfd = accept(ws.w_sockfd[i], &socket_address, &ws.w_client_len[i]);
                if (sockfd < 0)
                {
                    std::cerr << "\033[1;31mError accepting connection\033[0m" << std::endl;
                    continue;
                }
                // receive the request data
                std::string request = ws.receive(sockfd);
                Request rt(sockfd, envp, ws.w_config[i]);
                // handle the request
                rt.handleRequest(request);
                // close the connection
                close(sockfd);
            }
        }
    }
    for (unsigned int i = 0; i < ws.w_config.size(); i++)
        close(ws.w_sockfd[i]);
    return (0);
}