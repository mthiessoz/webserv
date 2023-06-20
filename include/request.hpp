#ifndef REQUEST_HPP
# define REQUEST_HPP

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
#include <dirent.h>
#include "server_conf.hpp"
#include "error.hpp"

struct s_request {

    std::string methd;
    std::string addr;
    std::map<std::string, std::string> data;

};

class	Request 
{
	public :

	Request(int client_sock_fd, char **envp, s_server w_config);
	~Request();

	int                            		r_client_sockfd;

	char                    			**r_wenvp;
    std::string                         r_query_string;

    s_server                            r_config;

	s_route								r_route;

    error                               *r_error;

	// handleRequest.cpp
    void    		handleRequest(std::string buffer);
	unsigned long	nbCommonLetters(std::string requestLocation, std::string routeLocation);
	bool 			check_hostname(std::string address);

	// parsingRequest.cpp
    void    		mainParsing(std::string request, s_request *requestData);
    void    		handleGET(s_request *requestData);
    void    		handlePOST(std::string request, s_request *requestData);
    void    		handleDELETE(s_request *requestData);
    bool    		checkMethod(std::string methd);

	// parsingRequestUtils.cpp
    void        	getAddrMethodData(std:: string request, s_request *requestData);
    std::string 	url_decode(const std::string& str);
    std::string 	methodToString(e_http_method method);
	std::string		unset_last_slash(std::string const &str);

    // sendResponse.cpp
    void    		sendResponse(s_request *requestData, int success_code);
    void    		sendIndex();
	void 			ft_send(std::string response, int size);

    // postRequest.cpp
    void            parsePostRequest(std::string request);
    std::string     parseBody(std::string request_body);
    std::string     getFilename(std::string request_data);

    // deleteRequest.cpp
    void            deleteRequest(s_request *requestData);

	// directoryListing.cpp
    void        	directoryListing(s_request *requestData);
    std::string 	listFilesInDirectory(const std::string& directoryPath);

    // redirects.cpp
    bool        	isRedirect();
    void        	redirectURL();

};

#endif