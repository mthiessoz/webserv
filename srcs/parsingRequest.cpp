#include "request.hpp"
#include "cgi.hpp"

void Request::handleGET(s_request *requestData)
{

	CGI cgi(r_client_sockfd, r_config, r_query_string);
	std::string fullPath = "";
	std::string root = "";
	std::string addr = "";
	if (cgi.is_cgi_request(requestData->addr))
		cgi.handle_cgi_request(r_client_sockfd, (r_route.root + requestData->addr));
	else
	{
		if (requestData->addr[requestData->addr.size() - 1] == '/' && r_route.index.size() != 0)
		{
			root = r_route.root.substr(0, r_route.root.size() - 1);
			addr = requestData->addr.substr(0, requestData->addr.size() - 1);
			fullPath = root + addr;
			sendIndex();
			return;
		}
		else
			fullPath = r_route.root + requestData->addr;
		DIR *dir = opendir(fullPath.c_str());
		if (dir != NULL && r_route.dir_listing)
		{
			directoryListing(requestData);
			closedir(dir);
		}
		else
			sendResponse(requestData, 200);
	}
}

void Request::handlePOST(std::string request, s_request *requestData)
{
	std::string temp = request.substr(request.find("\r\n\r\n") + 4);
	if (temp.size() > r_config.max_client_body_size)
	{
		r_error->send_error(413);
		return;
	}
	CGI cgi(r_client_sockfd, r_config, r_query_string);
	if (cgi.is_cgi_request(requestData->addr))
		cgi.handle_cgi_request(r_client_sockfd, (r_route.root + requestData->addr));
	else
		parsePostRequest(request);
}

void Request::handleDELETE(s_request *requestData)
{
	deleteRequest(requestData);
}

bool Request::checkMethod(std::string methd)
{
	for (std::vector<e_http_method>::const_iterator it = r_route.methods.begin();
			 it != r_route.methods.end(); it++)
	{
		if (methodToString(*it) == methd)
			return true;
	}
	return false;
}

void Request::mainParsing(std::string request, s_request *requestData)
{
	std::cout << "Method: " << requestData->methd << std::endl;
	std::cout << "Requested Address: " << requestData->addr << std::endl;

	// if (requestData->addr == "/exit")
	// 	exit(1);

	if (!checkMethod(requestData->methd))
		r_error->send_error(405);
	else if (!requestData->methd.compare("GET"))
		handleGET(requestData);
	else if (!requestData->methd.compare("POST"))
		handlePOST(request, requestData);
	else if (!requestData->methd.compare("DELETE"))
		handleDELETE(requestData);
	else
		r_error->send_error(405);
}
