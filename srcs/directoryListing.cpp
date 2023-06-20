#include "request.hpp"

std::string Request::listFilesInDirectory(const std::string &directoryPath)
{
    DIR *dir = opendir(directoryPath.c_str());
    if (dir == NULL)
    {
        std::cerr << "Could not open directory: " << directoryPath << std::endl;
        return "";
    }

    std::string html = "<html>\n<head><title>Directory Listing</title></head>\n<body>\n<h1>Directory Listing for " + directoryPath + "</h1>\n<ul>\n";
    html += "<li><a href=\"..\">..</a></li>\n";

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string entryName = std::string(entry->d_name);

        if (entryName != "." && entryName != "..")
        {
            std::string tmp_location = unset_last_slash(r_route.location);
            std::string fullPath = directoryPath + "/" + entryName;
            DIR *entryDir = opendir(fullPath.c_str());
            if (entryDir != NULL)
            {
                entryName += "/";
                closedir(entryDir);
            }
            html += "<li><a href=\"" + tmp_location + "/" + entryName + "\">" + entryName + "</a></li>\n";
        }
    }

    html += "</ul>\n</body>\n</html>";

    closedir(dir);
    return html;
}

void Request::directoryListing(s_request *requestData)
{
    std::string dirPath = r_route.root + requestData->addr.substr(1, requestData->addr.size());
    std::string list = listFilesInDirectory(dirPath);
    std::string base = "HTTP/1.1 200 OK\n\n";
    std::string response = base + list;

    ft_send(response, response.size());
}