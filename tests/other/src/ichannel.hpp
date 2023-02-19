#pragma once

#include "interface/interface.hpp"

#include <chrono>

using TimeTick = std::chrono::time_point<std::chrono::steady_clock>;

INTERFACE(IChannel)

DECLARE_METHOD(void, Read, (std::string & object));
DECLARE_METHOD(void, Write, (const std::string &object));

DECLARE_METHOD(void, Read, (TimeTick & object));
DECLARE_METHOD(void, Write, (const TimeTick &object));
END_INTERFACE();
