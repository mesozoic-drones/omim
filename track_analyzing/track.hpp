#pragma once

#include "routing/segment.hpp"
#include "track_analyzing/track_analyzer/track_type_handler.hpp"

#include "routing_common/num_mwm_id.hpp"

#include "coding/traffic.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace track_analyzing
{
using DataPoint = coding::TrafficGPSEncoder::DataPoint;
using Track = std::vector<DataPoint>;
using UserToTrack = std::unordered_map<std::string, Track>;
using MwmToTracks = std::unordered_map<routing::NumMwmId, UserToTrack>;

class MatchedTrackPoint final
{
public:
  MatchedTrackPoint(DataPoint const & dataPoint, routing::Segment const & segment)
    : m_dataPoint(dataPoint), m_segment(segment)
  {
  }

  DataPoint const & GetDataPoint() const { return m_dataPoint; }
  routing::Segment const & GetSegment() const { return m_segment; }

private:
  DataPoint const m_dataPoint;
  routing::Segment const m_segment;
};

using MatchedTrack = std::vector<MatchedTrackPoint>;
using UserToMatchedTracks = std::unordered_map<std::string, std::vector<MatchedTrack>>;
using MwmToMatchedTracks = std::unordered_map<routing::NumMwmId, UserToMatchedTracks>;

class TrackFilter final
{
public:
  TrackFilter(TrackParams && params);

  bool Passes(uint64_t duration, double length, double speed, bool hasTrafficPoints) const;

private:
  TrackParams m_params;
};
}  // namespace track_analyzing
