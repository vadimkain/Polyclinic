#include "Slot.hpp"

#include <utility>

namespace server::common {

template<typename... Args>
Slot<Args...>::Slot(std::function<void(Args...)> func) 
    : m_bind_method{std::move(func)}
{

}

template<typename... Args>
void Slot<Args...>::invoke(Args... args) {
    m_bind_method(std::forward<Args>(args)...);
}

}   // !server::common;
