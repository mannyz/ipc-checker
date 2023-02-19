#pragma once

#include <string_view>

enum class Mode { Consumer, Producer, Analyzer, RunTest };

Mode Convert(std::string_view role, Mode);
