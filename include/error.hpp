#ifndef ERROR_HPP
#define ERROR_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <utility>
#include <fstream>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

class error
{

    public:
        error(int fd, std::map<int, std::string> error_code);
        ~error();
        void send_error(int error_code);
		void ft_send(std::string response, int size);
		std::pair<int, std::vector<std::string> > fill_map(int error_code, std::string header, std::string path);

    int   	e_fd;
		int		e_size;
		std::map<int, std::vector<std::string> > e_errorCodes;
};

#endif