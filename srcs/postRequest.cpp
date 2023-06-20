#include "request.hpp"

std::string     Request::parseBody(std::string request_body)
{
    std::string::size_type data_start_pos = request_body.find("\r\n\r\n");
    if (data_start_pos == std::string::npos)
    {
        std::cerr << "Invalid multipart/form-data request body: no blank line found" << std::endl;
        return "";
    }

    std::string fileData = request_body.substr(data_start_pos + 4);
    return fileData;
}

std::string     Request::getFilename(std::string request_data)
 {
    std::string::size_type disposition_start_pos = request_data.find("Content-Disposition: ");
    if (disposition_start_pos == std::string::npos)
    {
        std::cerr << "Invalid multipart/form-data request body: no Content-Disposition header found" << std::endl;
        return "";
    }

    std::string::size_type filename_start_pos = request_data.find("filename=\"", disposition_start_pos);
    if (filename_start_pos == std::string::npos)
    {
        std::cerr << "Invalid multipart/form-data request body: no filename found" << std::endl;
        return "";
    }

    std::string::size_type filename_end_pos = request_data.find("\"", filename_start_pos + 10);
    if (filename_end_pos == std::string::npos)
    {
        std::cerr << "Invalid multipart/form-data request body: no end quote found for filename" << std::endl;
        return "";
    }

    return request_data.substr(filename_start_pos + 10, filename_end_pos - filename_start_pos - 10);
}

bool isBoundary(std::string const &request)
{
    std::string type = request.substr(request.find("Content-Type:"), request.size());
    std::string content_type = type.substr(0, type.find('\n'));
    return (content_type.find("boundary") != std::string::npos);
}

void Request::parsePostRequest(std::string request)
{
    if (!isBoundary(request))
    {
        std::string request_body = request.substr(request.find("\r\n\r\n") + 4, request.size());
        if (request_body.size() > r_config.max_client_body_size )
        {
            r_error->send_error(413);
            return ;
        }
        sendResponse(NULL, 202);
    }
    else
    {
        std::string request_body = request.substr(request.find("\r\n\r\n") + 4);
        if (request_body.size() > r_config.max_client_body_size )
        {
            r_error->send_error(413);
            return ;
        }
        std::string request_data = parseBody(request_body);
        std::string boundary = request.substr(request.find("boundary") + 11, 55);
        std::string filename = getFilename(request);
        std::string file_data = request_data.substr(0, (request_data.size() - boundary.size()) - 10);
        std::string path = r_route.root + filename;
        std::ofstream file(path.c_str(), std::ios::binary);

        file.write(file_data.data(), file_data.size());
        if (file.good()) 
        {
            file.close();
            sendResponse(NULL, 201);
        }
    }
}
