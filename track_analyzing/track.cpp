#include "track_analyzing/track.hpp"

namespace track_analyzing
{
TrackFilter::TrackFilter(TrackParams && params) : m_params(std::move(params)) {}

bool TrackFilter::Passes(uint64_t duration, double length, double speed,
                         bool hasTrafficPoints) const
{
  if (duration < m_params.m_minDuration)
    return false;

  if (length < m_params.m_minLength)
    return false;

  if (speed < m_params.m_minSpeed || speed > m_params.m_maxSpeed)
    return false;

  return !(m_params.m_ignoreTraffic && hasTrafficPoints);
}
}  // namespace track_analyzing
