#include "track_analyzing/track_analyzer/table_common_model.hpp"



namespace track_analyzing
{
std::string TypeToString(uint32_t type)
{
  if (type == 0)
    return "unknown-type";

  return classif().GetReadableObjectName(type);
}

bool DayTimeToBool(DayTimeType type)
{
  switch (type)
  {
  case DayTimeType::Day:
  case DayTimeType::PolarDay: return true;
  case DayTimeType::Night:
  case DayTimeType::PolarNight: return false;
  }
}
}  // namespace track_analyzing