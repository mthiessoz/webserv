#include "request.hpp"
#include "cgi.hpp"

std::string Request::methodToString(e_http_method method)
{
	switch (method)
	{
	case GET:
		return "GET";
	case POST:
		return "POST";
	case DELETE:
		return "DELETE";
	default:
		return "";
	}
}

void Request::getAddrMethodData(std::string request, s_request *requestData)
{
	std::size_t space_pos = request.find(' ');

	if (space_pos != std::string::npos)
	{
		requestData->methd = request.substr(0, space_pos);
		std::size_t next_space_pos = request.find(' ', space_pos + 1);
		if (next_space_pos != std::string::npos)
			requestData->addr = request.substr(space_pos + 1, next_space_pos - space_pos - 1);
	}

	if (requestData->methd == "POST")
	{
		std::string query_string = request.substr(request.find("\r\n\r\n") + 4);
		r_query_string = query_string;
	}
}

std::string Request::unset_last_slash(std::string const &str)
{
	if (!str.empty() && str.at(str.size() - 1) == '/')
		return str.substr(0, str.size() - 1);
	return str;
}
