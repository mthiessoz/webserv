#include <iostream>

#include "server_conf.hpp"

bool parse_http_methods(ParserJSON const &json,
												std::vector<ParserJSON::t_lexem>::const_iterator const &method,
												std::vector<enum e_http_method> &methods)
{
	enum e_http_method tmp;

	(void)json;
	if (method->lexem != ParserJSON::WORD)
		return true;
	if (!method->value.compare("POST"))
		tmp = (enum e_http_method)POST;
	else if (!method->value.compare("GET"))
		tmp = (enum e_http_method)GET;
	else if (!method->value.compare("DELETE"))
		tmp = (enum e_http_method)DELETE;
	else
	{
		std::cerr << ("route.methods: expect GET | POST | DELETE") << std::endl;
		return true;
	}
	methods.push_back(tmp);
	return false;
}

bool parse_routes(ParserJSON const &json,
									std::vector<ParserJSON::t_lexem>::const_iterator const &route,
									std::vector<t_route> &routes)
{
	t_route rou;
	int tmp;

	if (json.key("location", rou.location, parse_word, route))
		return (true);
	if (checkLocation(rou.location))
		return (true);

	if (json.key_map("methods", rou.methods, parse_http_methods, route) == 1)
		return (true);
	
	if ((tmp = json.key("http_redir", rou.http_redir, ParserJSON::to_string, route)) == 1)
		return (true);
	
	if ((tmp = json.key("root", rou.root, ParserJSON::to_string, route)) == 1)
		return (true);
	
	if ((tmp = json.key("dir_listing", rou.dir_listing, ParserJSON::to_bool, route)) == 1)
		return (true);
	if (tmp == 2)
		rou.dir_listing = false;
	
	if ((tmp = json.key("index", rou.index, ParserJSON::to_string, route)) == 1)
		return (true);
	
	routes.push_back(rou);
	return (false);
}
