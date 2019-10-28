#pragma once

#include "base/sunrise_sunset.hpp"

#include <cstdint>
#include <string>

namespace track_analyzing
{
std::string TypeToString(uint32_t type);
bool DayTimeToBool(DayTimeType type);
} // namespace track_analyzing
