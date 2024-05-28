#ifndef POLYCLINIC_SERVER_COMMON_SIGNAL_HPP
#define POLYCLINIC_SERVER_COMMON_SIGNAL_HPP

#include "Slot"

#include <string>
#include <vector>
#include <functional>

namespace server::common {

template<typename... Args>
class Signal {
public:
    void emit(Args... args);
    void connect(const Slot<Args...>& slot);

private:
    std::vector<Slot<Args...>> m_slots_container;
};

template<typename... Args>
void connect(Signal<Args...>* signal, Slot<Args...>* slot) {
    signal->connect(*slot);
}


}   // !server::common;

#endif  // !POLYCLINIC_SERVER_COMMON_SIGNAL_HPP;