#include "Signal.hpp"

#include <thread>
#include <utility>

namespace server::common {

template<typename... Args>
void Signal<Args...>::emit(Args... args) {
    for (auto &slot : m_slots_container) {
        std::thread(&Slot<Args...>::invoke, &slot, std::forward<Args>(args)...).detach();
    }
}

template<typename... Args>
void Signal<Args...>::connect(const Slot<Args...>& slot) {
    m_slots_container.push_back(slot);
}

}   // !server::common;