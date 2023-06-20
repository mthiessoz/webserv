#include "request.hpp"

bool Request::isRedirect()
{
    if (r_route.http_redir.size() > 0)
        return true;
    else
        return false;
}

void Request::redirectURL()
{
    std::string base = "HTTP/1.1 301 Moved Permanently\n";
    std::string body = "Location: " + r_route.http_redir + "\r\n";
    ft_send(body, body.size());
    return;
}