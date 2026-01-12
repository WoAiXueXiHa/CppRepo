#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "../include/c_db_os.h"

int dbos_read_entire_file(const char *path, char **out, size_t *out_len) {
    if (!path || !out || !out_len) return 0;
    *out = NULL; *out_len = 0;

    int fd = open(path, O_RDONLY);
    if (fd < 0) return 0;

    struct stat st;
    if (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size >= 0) {
        size_t need = (size_t)st.st_size;
        char *buf = (char *)malloc(need + 1);
        if (!buf) { close(fd); return 0; }
        size_t total = 0;
        while (total < need) {
            ssize_t r = read(fd, buf + total, need - total);
            if (r < 0) { free(buf); close(fd); return 0; }
            if (r == 0) break;
            total += (size_t)r;
        }
        buf[total] = '\0';
        *out = buf; *out_len = total;
        close(fd);
        return 1;
    }

    // Fallback: unknown size (pipe / other) -> read loop with realloc
    const size_t CHUNK = 4096;
    size_t cap = CHUNK;
    char *buf = (char *)malloc(cap);
    if (!buf) { close(fd); return 0; }
    size_t total = 0;
    while (1) {
        if (total + CHUNK > cap) {
            size_t ncap = cap * 2;
            char *n = (char *)realloc(buf, ncap);
            if (!n) { free(buf); close(fd); return 0; }
            buf = n; cap = ncap;
        }
        ssize_t r = read(fd, buf + total, CHUNK);
        if (r < 0) { free(buf); close(fd); return 0; }
        if (r == 0) break;
        total += (size_t)r;
    }
    // shrink to fit
    char *sh = (char *)realloc(buf, total + 1);
    if (sh) buf = sh;
    buf[total] = '\0';
    *out = buf; *out_len = total;
    close(fd);
    return 1;
}

int dbos_write_entire_file(const char *path, const char *data, size_t len) {
    if (!path) return 0;
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return 0;
    if (!data) { close(fd); return 1; }
    size_t total = 0;
    while (total < len) {
        ssize_t w = write(fd, data + total, len - total);
        if (w < 0) { close(fd); return 0; }
        total += (size_t)w;
    }
    fsync(fd);
    close(fd);
    return 1;
}

int dbos_append_text_file(const char *path, const char *data, size_t len) {
    if (!path) return 0;
    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) return 0;
    if (!data) { close(fd); return 1; }
    size_t total = 0;
    while (total < len) {
        ssize_t w = write(fd, data + total, len - total);
        if (w < 0) { close(fd); return 0; }
        total += (size_t)w;
    }
    fsync(fd);
    close(fd);
    return 1;
}
