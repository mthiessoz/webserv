#include <set>
#include <utility>
#include <iostream>

#include "server_conf.hpp"


bool checkServersConf(std::vector<t_server> const &servers)
{
	std::set<unsigned int> ports;
	std::map<std::vector<t_server>::const_iterator, std::string> set_servers_names;

	if (servers.size() == 0)
	{
		std::cerr << "checkServersConf - no servers configuration found" << std::endl;
		return (true);
	}

	for (std::vector<t_server>::const_iterator it = servers.begin(); it != servers.end(); ++it)
	{
		std::pair<std::set<unsigned int>::iterator, bool> set_ports_ret = ports.insert(it->port);
		if (!set_ports_ret.second)
		{
			std::cerr << "checkServersConf - servers port not unique" << std::endl;
			return (true);
		}

		for (std::vector<std::string>::const_iterator it_server_name = it->server_names.begin(); it_server_name != it->server_names.end(); ++it_server_name)
		{
			std::pair<std::map<std::vector<t_server>::const_iterator, std::string>::iterator, bool> set_servers_names_ret;
			set_servers_names_ret = set_servers_names.insert(std::pair<std::vector<t_server>::const_iterator, std::string>(it, *it_server_name));
			if (!set_servers_names_ret.second && it != set_servers_names_ret.first->first)
			{
				std::cerr << "checkServersConf - servers names not unique" << std::endl;
				return (true);
			}
		}
	}

	return false;
}