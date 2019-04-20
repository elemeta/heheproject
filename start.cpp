// g++ -o start start.cpp
//
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <iostream>

std::string GetAppDir()
{
	char path[4096] = {0};
	char sep = '/';
	int cnt = readlink("/proc/self/exe", path, 4096);

	for (int i = cnt; i >= 0; --i) {
     		if(path[i] == sep) {
        		path[i]='\0';
        		break;
    		}
	}

	return path;
}

bool WriteString(const char *filename, const std::string &text)
{
    int n;
    int fd;
    
    if (!filename)
        return false;
    fd = ::open(filename, O_CREAT|O_WRONLY|O_SYNC, 0644);
    if (fd < 0)
        return false;
    n = ::write(fd, text.c_str(), text.length());
    ::close(fd);
    return (n == text.length());
}

int main(int argc, char **argv)
{
	std::string current_path(GetAppDir());
	if (current_path.empty()) {
		std::cout << "get current path error" << std::endl;
		return -1;
	}
	std::string libs_path(current_path + "/libs");

	if (setenv("JHSE_SAFE_PATH", current_path.c_str(), 1) == -1 ||
		setenv("LD_LIBRARY_PATH", libs_path.c_str(), 1) == -1) {
		std::cout << "set env failed, " << strerror(errno) << std::endl;
		return -1;
	}

	if (!WriteString("/var/log/version_control", current_path)) {
		std::cout << "write version_control failed" << std::endl;
		return -1;
	}

	std::string cmd(current_path+"/secmodel_service -c config.xml -l runlog/run_log.log -e \"ALL\" -t \"ALL\" -s safe -m daemon");
	int ret = system(cmd.c_str());
	std::cout << "exec: " << cmd << "return=" << ret << std::endl;

	return 0;
}

