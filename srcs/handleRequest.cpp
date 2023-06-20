#include "request.hpp"

unsigned int count_different_char(std::string const &a, std::string const &b)
{
    unsigned int i = 0;
    unsigned int diff_char = 0;

     for (; a[i] && b[i] && a[i] == b[i]; ++i) {}

    for (; a[i] && b[i] && a[i] != b[i] && i < a.size(); ++i)
    {
        diff_char++;
    }
    if (a.size() > b.size())
        diff_char += (a.size() - b.size());
    return (diff_char);
}

unsigned int count_same_char(std::string const &a, std::string const &b)
{
    unsigned int i = 0;

    for (; a[i] && b[i] && a[i] == b[i]; ++i)
    {
    }

    return (i);
}

void Request::handleRequest(std::string buffer)
{
    s_request requestData;
    std::string::size_type maxCommonLetters = 0;
    std::string::size_type maxDifferentLetters = 0;

    getAddrMethodData(buffer, &requestData);
    if (!check_hostname(buffer))
        return;
    for (unsigned long i = 0; i < r_config.routes.size(); i++)
    {
        /*
            /success
        
            /succ
            /
            /successa
            
            /success/
            /success
            /success/dsadsa
        */
        unsigned int tmp_n = count_same_char(r_config.routes[i].location, requestData.addr);
        if (tmp_n != 0 && tmp_n == r_config.routes[i].location.size())
        {
            maxCommonLetters = tmp_n;
            r_route = r_config.routes[i];
        }
    }
    for (unsigned long i = 0; i < r_config.routes.size(); i++)
    {
        unsigned int tmp_n = count_same_char(r_config.routes[i].location, requestData.addr);
        unsigned int tmp_d = count_different_char(r_config.routes[i].location, requestData.addr);
        if (tmp_d <= maxDifferentLetters && tmp_n == maxCommonLetters )
        {
            maxDifferentLetters = tmp_d;
            r_route = r_config.routes[i];
        }
    }
    if (r_route.location.empty())
    {
        r_error->send_error(403);
        return;
    }
    mainParsing(buffer, &requestData);
}

bool Request::check_hostname(std::string address)
{
    std::string temp;
    std::size_t end_pos = address.find('\n');
    if (address.size() == 0)
        return true;
    temp = address.substr(end_pos + 7, address.size() - end_pos);
    end_pos = temp.find('\n');
    temp = temp.substr(0, end_pos);
    end_pos = temp.find(':');
    temp = temp.substr(0, end_pos);
    if (r_config.server_names.size() == 0)
        return true;
    std::vector<std::string>::iterator it;
    for (it = r_config.server_names.begin(); it != r_config.server_names.end(); it++)
    {
        if (it->find(temp) != std::string::npos)
            return true;
    }

    r_error->send_error(400);
    return false;
}