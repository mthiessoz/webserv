#include "request.hpp"

Request::Request(int client_sock_fd, char **envp, s_server w_config)
{
        r_client_sockfd = client_sock_fd;
        r_wenvp = envp;

    	r_config = w_config;
        this->r_error = new error(client_sock_fd, r_config.default_error_pages);
}
Request::~Request()
{
	delete this->r_error;
}

