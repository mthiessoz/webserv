#ifndef CGI_HPP
# define CGI_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <fstream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "webserv.hpp"
#include "error.hpp"
#include "request.hpp"
#include "server_conf.hpp"
#include <signal.h>

class CGI
{
    public:

   CGI(int fd, s_server r_config, std::string r_query_string);
    ~CGI();

	error                               *c_error;
	s_server                            c_config;

    std::string                         c_env_query;

    void handle_cgi_request(int sockfd, const std::string& query_string);
    bool is_cgi_request(const std::string& request_path);
	bool ft_write(int fd, std::string string, int size);


};

#endif