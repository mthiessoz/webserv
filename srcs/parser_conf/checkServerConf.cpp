#include <set>
#include <utility>
#include <iostream>

#include "server_conf.hpp"

bool checkPort(unsigned int const &port)
{
	if (1 <= port && port <= 65535)
		return (false);
	return (true);
}

bool checkServerConf(t_server const &server)
{
	std::set<std::string> locations;

	if (server.routes.size() == 0)
	{
		std::cerr << "checServerConf - server.routes should have at least one" << std::endl;
		return (true);
	}

	for (std::vector<t_route>::const_iterator it_route = server.routes.begin(); it_route != server.routes.end(); ++it_route)
	{
		std::pair<std::set<std::string>::iterator, bool> ret = locations.insert(it_route->location);
		if (!ret.second)
		{
			std::cerr << "checkServerConf - routes.location not unique";
			return (true);
		}
	}

	return (false);
}