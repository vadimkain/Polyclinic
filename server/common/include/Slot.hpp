#ifndef POLYCLINIC_SERVER_COMMON_SLOT_HPP
#define POLYCLINIC_SERVER_COMMON_SLOT_HPP

#include <string>
#include <vector>
#include <functional>

namespace server::common {

template<typename... Args>
class Slot {
public:
    Slot(std::function<void(Args...)> func);

    void invoke(Args... args);

private:
    std::function<void(Args...)> m_bind_method;
};

}   // !server::common;

#endif  // !POLYCLINIC_SERVER_COMMON_SLOT_HPP;