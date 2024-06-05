#include "File.hpp"

#include <cstring>
#include <unistd.h>

namespace server::common {

std::int32_t File::open(std::string file_path, OpenTypeEnum modificator) {
    std::int32_t ret = -1;

    m_file_fd = ::open(file_path.c_str(), static_cast<std::int32_t>(modificator));

    if (m_file_fd >= 0) {
        ret = update_file_stat();
    }

    return ret;
}

std::int32_t File::close(void) {
    return ::close(m_file_fd);
}

std::string File::latest_error(void) const {
    return std::strerror(errno);
}

std::int32_t File::size(void) const noexcept {
    return m_file_stat.st_size;
}

std::int32_t File::file_fd(void) const noexcept {
    return m_file_fd;
}

std::int32_t File::update_file_stat(void) {
    return ::fstat(m_file_fd, &m_file_stat);
}

}   // !server::common;