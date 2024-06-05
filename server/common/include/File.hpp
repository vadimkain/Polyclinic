#ifndef POLYCLINIC_SERVER_COMMON_FILE_HPP
#define POLYCLINIC_SERVER_COMMON_FILE_HPP

#include <string>
#include <cinttypes>

#include <sys/stat.h>
#include <fcntl.h>

namespace server::common {

class File {
public:
    enum class OpenTypeEnum {
        READ_ONLY = O_RDONLY,
        WRITE_ONLY = O_WRONLY,
        READ_WRITE = O_RDWR
    };

public:
    std::int32_t open(std::string file_path, OpenTypeEnum modificator);
    std::int32_t close(void);
    std::string latest_error(void) const;

    std::int32_t size(void) const noexcept;
    std::int32_t file_fd(void) const noexcept;

private:
    std::int32_t update_file_stat(void);

private:
    std::int32_t m_file_fd;
    struct stat m_file_stat;
};

}   // !server::common;

#endif  // !POLYCLINIC_SERVER_COMMON_FILE_HPP;