#include "cgi.hpp"
#include <sys/wait.h>
#include <ctime>

CGI::CGI(int fd, s_server r_config, std::string r_query_string)
{
	this->c_config = r_config;
	this->c_error = new error(fd, c_config.default_error_pages);
    this->c_env_query = r_query_string;
    return;
}

CGI::~CGI()
{
	delete this->c_error;
    return;
}

void CGI::handle_cgi_request(int sockfd, const std::string& cgi_path)
{
    std::stringstream   buff;
    std::ifstream   file(cgi_path.c_str());
    buff << file.rdbuf();
    if (!buff.str().size())
    {
        c_error->send_error(404);
        return;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cout << "Error creating pipe" << std::endl;
        
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
       std::cout << "Error forking process" << std::endl;
        return;
    }

    if (pid == 0)
    {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        char* args[] = { const_cast<char*>(PYTHON), const_cast<char*>(cgi_path.c_str()), const_cast<char*>(c_env_query.c_str()), NULL};
        execve(PYTHON, args, NULL);
        std::cout << "Error executing CGI program" << std::endl;
        exit(EXIT_FAILURE);
    }

    close(pipefd[1]);

    fcntl(pipefd[0], F_SETFL, O_NONBLOCK);

    int wpid, Stat;
    std::time_t start_time = std::time(NULL);
    const int timeout = 5;
	int bytes_read = 1;
    std::string tmp;

    wpid = waitpid(pid, &Stat, WNOHANG);
    while (wpid == 0 && (std::time(NULL) - start_time) <= timeout && bytes_read) 
	{
        std::time_t current_time = std::time(NULL);
        if (current_time - start_time < timeout) 
		{
            char buffer[1024];
			
            while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0)
				tmp = buffer;
            if (bytes_read == -1 || bytes_read == 0)
                continue;
        }
        else 
		{
            std::cout << "CGI execution time exceeded the limit. Terminating process." << std::endl;
            kill(pid, SIGKILL);
			c_error->send_error(508);
            close(pipefd[0]);
            return;
        }

        wpid = waitpid(pid, &Stat, WNOHANG);
    }
    std::string status = "HTTP/1.1 200 OK\n\n";
	ft_write(sockfd, status.c_str(), status.size());
    ft_write(sockfd, tmp, tmp.size());
    close(pipefd[0]);
}

bool CGI::is_cgi_request(const std::string& request_path)
{
    if (request_path.size() <= 1)
        return false;

    std::vector<std::string> cgi_extensions;
    cgi_extensions.push_back(".py");

    size_t dot_pos = request_path.find_last_of('.');
    if (dot_pos == std::string::npos)
        return false;

    std::string ext = request_path.substr(dot_pos);


    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return std::find(cgi_extensions.begin(), cgi_extensions.end(), ext) != cgi_extensions.end();
}

bool CGI::ft_write(int fd, std::string string, int size)
{
	ssize_t writtenpacket = 0;
	ssize_t temp = 0;
	while (writtenpacket < (ssize_t)size)
	{
		temp = write(fd, string.c_str() + writtenpacket, size - writtenpacket);
		if (temp == -1)
		{
			std::cout << "Error in the function write()" << std::endl;
			return false;
		}
		else if (temp == 0)
			return true;
		writtenpacket += temp;
	}
	return true;
}