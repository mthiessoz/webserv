#include "webserv.hpp"

int Webserv::socketBinding(void)
{

    // Create a new socket file descriptor using the socket() system call. 
    // The first argument AF_INET specifies the address family (in this case, IPv4)
    // The second argument SOCK_STREAM specifies the socket type (in this case, a TCP stream socket)
    // The third argument 0 indicates that the protocol should be chosen automatically.
    addrinfo hints, *res;
    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;
    for (unsigned int i = 0; i < w_config.size(); i++)
    {
        w_sockfd.push_back(socket(AF_INET, SOCK_STREAM, 0));
        if (w_sockfd[i] < 0)
        {
            std::cerr << "Error creating socket" << std::endl;
            return 1;
        }

        int enable = 1;
        if (setsockopt(w_sockfd[i], SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        {
            perror("setsockopt(SO_REUSEADDR) failed");
            return 1;
        }
        
        // This sets up the server address structure, server_addr, which will be used to bind the socket to a specific port. 

        // The sin_family member specifies the address family
        //  sin_addr.s_addr specifies the IP address to bind to (in this case, INADDR_ANY indicates that the socket
        // should listen on all available network interfaces)
        
        //sin_port specifies the port number to bind to (which is taken from the ports).

        // The bind() system call is then used to associate the socket file descriptor with the server address. 
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if (getaddrinfo(w_config[i].host.c_str(), NULL, &hints, &res) != 0)
        {
            std::cerr << "getaddrinfo failed" << std::endl;
            return 1;
        }

        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr = ((struct sockaddr_in*) res->ai_addr)->sin_addr;
        server_addr.sin_port = htons(w_config[i].port);

        freeaddrinfo(res);

        if (bind(w_sockfd[i], (sockaddr*) &server_addr, sizeof(server_addr)) < 0)
        {
            std::cerr << "Error binding socket : " << w_sockfd[i] << std::endl;
            return 1;
        }
        // std::cout << "Socket '" << w_sockfd[i] << "' binded" << " to port " << w_ports[i] << std::endl;


        // Set the socket to listen for incoming connections.
        // The second argument to the listen() call specifies the size of the backlog queue,
        // which is the number of incoming connections that can be queued up before the system starts rejecting them. 

        // In this case, the backlog is set to 64. 
        // The _client_len variable is also initialized to the size of the _client_addr structure,
        // which will be used to store client connection information.
        listen(w_sockfd[i], 64);
        w_client_len.push_back(sizeof(sockaddr));

    }

    // create a set of file descriptors to monitor for activity
    w_max_fd = -1;
    for (unsigned int i = 0; i < w_config.size(); i++)
    {
        FD_SET(w_sockfd[i], &w_fds);
        w_max_fd = std::max(w_max_fd, w_sockfd[i]);
    }
    return (0);
}

