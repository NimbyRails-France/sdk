#pragma once
#include <nimby/detail/control.h>
#include <stdexcept>
namespace nimby::detail::control {
class Server {
public:
    void start(const char*,NimbyControlHandler){throw std::runtime_error("Live mod control is not qualified on Linux");}
    void stop(){}
};
}
