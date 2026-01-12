#include "../include/db_os.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

namespace db_os {

bool read_entire_file(const std::string &path, std::string &out) {
    out.clear();
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0) return false;

    const size_t BUF = 4096;
    char buffer[BUF];
    ssize_t n;
    while ((n = read(fd, buffer, BUF)) > 0) {
        out.append(buffer, buffer + n);
    }
    close(fd);
    return n >= 0;
}

bool write_entire_file(const std::string &path, const std::string &data) {
    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return false;
    size_t total = 0;
    const char *buf = data.c_str();
    size_t len = data.size();
    while (total < len) {
        ssize_t w = write(fd, buf + total, len - total);
        if (w < 0) { close(fd); return false; }
        total += (size_t)w;
    }
    fsync(fd);
    close(fd);
    return true;
}

bool append_text_file(const std::string &path, const std::string &data) {
    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) return false;
    size_t total = 0;
    const char *buf = data.c_str();
    size_t len = data.size();
    while (total < len) {
        ssize_t w = write(fd, buf + total, len - total);
        if (w < 0) { close(fd); return false; }
        total += (size_t)w;
    }
    fsync(fd);
    close(fd);
    return true;
}

} // namespace db_os
