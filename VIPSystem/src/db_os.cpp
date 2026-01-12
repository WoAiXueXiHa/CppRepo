// Compatibility wrapper: delegate C++ namespace `db_os` calls to the new C API.
#include <string>
#include <cstdlib>
#include "../include/c_db_os.h"

namespace db_os {

bool read_entire_file(const std::string &path, std::string &out) {
    out.clear();
    char *buf = NULL; size_t len = 0;
    if (!dbos_read_entire_file(path.c_str(), &buf, &len)) return false;
    out.assign(buf, len);
    free(buf);
    return true;
}

bool write_entire_file(const std::string &path, const std::string &data) {
    return dbos_write_entire_file(path.c_str(), data.c_str(), data.size()) != 0;
}

bool append_text_file(const std::string &path, const std::string &data) {
    return dbos_append_text_file(path.c_str(), data.c_str(), data.size()) != 0;
}

} // namespace db_os
