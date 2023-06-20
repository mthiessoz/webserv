#include "request.hpp"

void Request::sendResponse(s_request *requestData, int success_code)
{
		const std::string successCodes[] = {"200 OK", "201 Created", "202 Accepted"};
		const std::string successPage[] = {"www/success/201.html", "www/success/202.html"};
		const int success[] = {200, 201, 202};
		const int size = sizeof(success) / sizeof(success[0]);
		std::string base, name;
		std::string response;
		std::stringstream   buff;
		if (success_code == 200)
		{
			std::string path = r_route.root + requestData->addr.substr(1, requestData->addr.size());
        	std::ifstream   file(path.c_str());
			if (isRedirect())
			{
				redirectURL();
				return;
			}
			buff << file.rdbuf();
			if (!buff.str().size())
			{
				r_error->send_error(404);
				return;
			}
		}
		else
		{
			for (int i = 0; i < size; ++i) 
			{
				if (success_code == success[i]) 
				{
					name = successPage[i - 1];
					std::ifstream file(name.c_str());
					buff << file.rdbuf();
				}
			}
		}
     	for (int i = 0; i < size; ++i) {
			if (success_code == success[i]) {
				base = "HTTP/1.1 " + successCodes[i] + "\n\n";
			}
		}
		response = base + buff.str();
        ft_send(response, response.size());
}

void Request::sendIndex()
{
	std::string path = r_route.root + r_route.index;
	std::ifstream   file(path.c_str());
	std::stringstream   buff;
	std::string response;
	buff << file.rdbuf();
	std::string base = "HTTP/1.1 200 OK \n\n";
	response = base + buff.str();
	ft_send(response, response.size());
}

void Request::ft_send(std::string response, int size)
{
	ssize_t sendedpacket = 0;
	ssize_t temp = 0;
	while (sendedpacket < (ssize_t)size)
	{
		temp = send(r_client_sockfd, response.c_str() + sendedpacket, size - sendedpacket, 0);
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
