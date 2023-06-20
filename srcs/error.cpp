#include "error.hpp"

error::error(int fd, std::map<int, std::string> error_code)
{
	std::map<int, std::string>::iterator it;
  e_fd = fd;

	e_errorCodes.insert(fill_map(400, "400 Bad Request", "www/errors/400.html"));
	e_errorCodes.insert(fill_map(403, "403 Forbidden", "www/errors/403.html"));
	e_errorCodes.insert(fill_map(404, "404 Not Found", "www/errors/404.html"));
	e_errorCodes.insert(fill_map(405, "405 Method Not Allowed", "www/errors/405.html"));
	e_errorCodes.insert(fill_map(413, "413 Content Too Large", "www/errors/413.html"));
	e_errorCodes.insert(fill_map(508, "508 Loop Detected", "www/errors/508.html"));

	for (it = error_code.begin(); it != error_code.end(); it++)
	{
			std::ifstream ifs;
			ifs.open(it->second.c_str(), std::ios_base::in);
			if (ifs.is_open())
			{
				e_errorCodes[it->first][1] = it->second;
				ifs.close();
			}
	}
  e_size = e_errorCodes.size();
}

error::~error()
{

}

std::pair<int, std::vector<std::string> >	error::fill_map(int error_code, std::string header, std::string path)
{
	std::vector<std::string> value;

	value.push_back(header);
	value.push_back(path);

	return (std::make_pair(error_code, value));
}

void    error::send_error(int error_code)
{
	std::string base, name;
	std::map<int, std::vector<std::string> >::iterator it;

    for (it = e_errorCodes.begin(); it != e_errorCodes.end() ; it++) 
	{
        if (error_code == it->first) 
		{
			base = "HTTP/1.1 " + it->second[0] + "\n\n";
			name = it->second[1];
			break;
        }
    }
    std::ifstream file(name.c_str());
    std::stringstream buff;
    buff << file.rdbuf();

    std::string response = base + buff.str();
    send(e_fd, response.c_str(), response.size(), 0);
}

void error::ft_send(std::string response, int size)
{
	ssize_t sendedpacket = 0;
	ssize_t temp = 0;
	while (sendedpacket < (ssize_t)size)
	{
		temp = send(e_fd, response.c_str() + sendedpacket, size - sendedpacket, 0);
		if (temp == -1)
		{
			std::cout << "Error in the function send()" << std::endl;
			return ;
		}
		else if (temp == 0)
			return ;
		sendedpacket += temp;
	}

}