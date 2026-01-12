#ifndef C_DB_OS_H
#define C_DB_OS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// C 风格的文件读写接口（返回 1 成功，0 失败）
// 注意：read 接口在成功时会通过 malloc 分配 *out，调用方需使用 free() 释放。

int dbos_read_entire_file(const char *path, char **out, size_t *out_len);
int dbos_write_entire_file(const char *path, const char *data, size_t len);
int dbos_append_text_file(const char *path, const char *data, size_t len);

#ifdef __cplusplus
}
#endif

#endif // C_DB_OS_H
