#pragma once

#include "routing_common/car_model.hpp"
#include "routing_common/bicycle_model.hpp"
#include "routing_common/pedestrian_model.hpp"

#include "indexer/feature.hpp"
#include "indexer/classificator.hpp"

#include "base/sunrise_sunset.hpp"

#include <cstdint>
#include <string>
#include <type_traits>

namespace track_analyzing
{
std::string TypeToString(uint32_t type);
bool DayTimeToBool(DayTimeType type);

class ModelTypesBase
{

};

template <class T>
class ModelTypes : public ModelTypesBase
{
public:
  ModelTypes();

  struct Type
  {
    bool operator<(Type const & rhs) const;
    bool operator==(Type const & rhs) const;
    bool operator!=(Type const & rhs) const;

    uint32_t m_hwType = 0;
    uint32_t m_surfaceType = 0;
  };

  Type GetType(FeatureType & feature) const;

private:
  std::vector<uint32_t> m_hwtags;
  std::vector<uint32_t> m_surfaceTags;
};

template<class T>
bool ModelTypes<T>::Type::operator<(Type const & rhs) const
{
  return std::tie(m_hwType, m_surfaceType) < std::tie(rhs.m_hwType, rhs.m_surfaceType);
}

template<class T>
bool ModelTypes<T>::Type::operator==(Type const & rhs) const
{
  return std::tie(m_hwType, m_surfaceType) == std::tie(rhs.m_hwType, m_surfaceType);
}
template  <class T>
bool ModelTypes<T>::Type::operator!=(Type const & rhs) const { return !(*this == rhs); }

template <class T>
ModelTypes<T>::ModelTypes()
{
  if constexpr(std::is_same<T, routing::CarModel>::value)
  {
    for (auto const & additionalTag : T::GetAdditionalTags())
      m_hwtags.push_back(classif().GetTypeByPath(additionalTag.m_hwtag));
  }

  for (auto const & speedForType : T::GetOptions())
    m_hwtags.push_back(classif().GetTypeByPath(speedForType.m_types));

  for (auto const & surface : T::GetSurfaces())
    m_surfaceTags.push_back(classif().GetTypeByPath(surface.m_types));
}

template <class T>
typename ModelTypes<T>::Type ModelTypes<T>::GetType(FeatureType & feature) const
{
  Type ret;
  feature::TypesHolder holder(feature);
  for (uint32_t type : m_hwtags)
  {
    if (holder.Has(type))
    {
      ret.m_hwType = type;
      break;
    }
  }

  for (uint32_t type : m_surfaceTags)
  {
    if (holder.Has(type))
    {
      ret.m_surfaceType = type;
      break;
    }
  }

  return ret;
}
} // namespace track_analyzing
