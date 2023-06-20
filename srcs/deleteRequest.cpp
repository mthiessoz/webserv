#include "request.hpp"

bool isValidPath(const std::string& path)
{
    if (path.find("..") != std::string::npos)
        return false;
    std::string invalidChars = "*:?|<>\"";
    if (path.find_first_of(invalidChars) != std::string::npos)
        return false;

    return true;
}

void Request::deleteRequest(s_request *requestData)
{
    std::string fullPath = r_route.root + requestData->addr;
    if (!isValidPath(requestData->addr))
		r_error->send_error(404);
    else if (std::remove(fullPath.c_str()) == 0)
    {
       sendResponse(NULL, 202);
    }
    else
        r_error->send_error(404);
}
