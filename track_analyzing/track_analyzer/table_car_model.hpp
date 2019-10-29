#pragma once
#include "track_analyzing/track_analyzer/table_common_model.hpp"
#include "track_analyzing/track_analyzer/crossroad_checker.hpp"

#include "track_analyzing/track.hpp"
#include "track_analyzing/utils.hpp"

#include "routing/city_roads.hpp"
#include "routing/geometry.hpp"
#include "routing/maxspeeds.hpp"

#include "routing_common/car_model.hpp"
#include "routing_common/maxspeed_conversion.hpp"
#include "routing_common/vehicle_model.hpp"

#include "traffic/speed_groups.hpp"

#include "indexer/data_source.hpp"
#include "indexer/feature.hpp"
#include "indexer/feature_data.hpp"
#include "indexer/features_vector.hpp"

#include "storage/routing_helpers.hpp"
#include "storage/storage.hpp"

#include "coding/file_reader.hpp"

#include "geometry/latlon.hpp"

#include "base/file_name_utils.hpp"
#include "base/stl_helpers.hpp"
#include "base/sunrise_sunset.hpp"

#include <cstdint>
#include <limits>
#include <map>
#include <string>
#include <vector>

#include "defines.hpp"

namespace track_analyzing
{
/*
struct RoadInfo
{
  bool operator==(RoadInfo const & rhs) const;
  bool operator!=(RoadInfo const & rhs) const;
  bool operator<(RoadInfo const & rhs) const;

  std::string GetSummary() const;

  CarModelTypes::Type m_type;
  routing::MaxspeedType m_maxspeedKMpH = routing::kInvalidSpeed;
  bool m_isCityRoad = false;
  bool m_isOneWay = false;
};

class MoveType final
{
public:
  MoveType() = default;
  MoveType(RoadInfo const & roadType, traffic::SpeedGroup speedGroup, DataPoint const & dataPoint);

  bool operator==(MoveType const & rhs) const;
  bool operator<(MoveType const & rhs) const;

  bool IsValid() const;
  std::string GetSummary() const;

private:
  RoadInfo m_roadInfo;
  traffic::SpeedGroup m_speedGroup = traffic::SpeedGroup::Unknown;
  ms::LatLon m_latLon;
  bool m_isDayTime = false;
};

class SpeedInfo final
{
public:
  void Add(double distance, uint64_t time,
           routing::IsCrossroadChecker::CrossroadInfo const & crossroads);
  std::string GetSummary() const;

private:
  double m_totalDistance = 0.0;
  uint64_t m_totalTime = 0;
  routing::IsCrossroadChecker::CrossroadInfo m_crossroads{};
};

class MoveTypeAggregator final
{
public:
  void Add(MoveType && moveType, routing::IsCrossroadChecker::CrossroadInfo const & crossroads,
           MatchedTrack::const_iterator begin, MatchedTrack::const_iterator end,
           routing::Geometry & geometry);
  std::string GetSummary(std::string const & user, std::string const & mwm) const;

private:
  std::map<MoveType, SpeedInfo> m_moveInfos;
};

class MatchedTrackPointToMoveType final
{
public:
  MatchedTrackPointToMoveType(FilesContainerR const & container,
                              routing::VehicleModelInterface & vehicleModel);

  MoveType GetMoveType(MatchedTrackPoint const & point);

private:
  RoadInfo GetRoadInfo(routing::Segment const & segment);

  FeaturesVectorTest m_featuresVector;
  routing::VehicleModelInterface & m_vehicleModel;
  CarModelTypes const m_carModelTypes;
  routing::CityRoads m_cityRoads;
  routing::Maxspeeds m_maxspeeds;
  uint32_t m_prevFeatureId = std::numeric_limits<uint32_t>::max();
  RoadInfo m_prevRoadInfo;
}; */
}  // namespace track_analyzing
