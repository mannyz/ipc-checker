#pragma once

#include "interface/interface.hpp"
#include "interface/measurer/time_tick.hpp"

INTERFACE(IMeasurer)
DECLARE_METHOD(void, Tick, ());
DECLARE_METHOD(TimeTick, FirstTick, (), (const));
DECLARE_METHOD(TimeTick, LastTick, (), (const));

DECLARE_METHOD(void, ToFirst, ());
DECLARE_METHOD(bool, HasNext, (), (const));
DECLARE_METHOD(TimeTick, Next, ());
DECLARE_METHOD(std::size_t, Count, (), (const));

DECLARE_METHOD(bool, IsFull, (), (const));
DECLARE_METHOD(void, PushBack, (const TimeTick &tick));
END_INTERFACE();
