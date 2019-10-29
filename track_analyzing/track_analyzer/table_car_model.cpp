#include "track_analyzing/track_analyzer/table_car_model.hpp"
#include "track_analyzing/track_analyzer/table_common_model.hpp"

#include "indexer/classificator.hpp"

#include "base/assert.hpp"
#include "base/logging.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <tuple>
#include <utility>

#include "routing_common/pedestrian_model.hpp"
#include "routing_common/bicycle_model.hpp"
namespace
{
routing::MaxspeedType constexpr kMaxspeedTopBound = 200;
auto constexpr kValidTrafficValue = traffic::SpeedGroup::G5;
}  // namespace

namespace track_analyzing
{
/*
bool CarModelTypes::Type::operator<(Type const & rhs) const
{
  return std::tie(m_hwType, m_surfaceType) < std::tie(rhs.m_hwType, rhs.m_surfaceType);
}

bool CarModelTypes::Type::operator==(Type const & rhs) const
{
  return std::tie(m_hwType, m_surfaceType) == std::tie(rhs.m_hwType, m_surfaceType);
}

bool CarModelTypes::Type::operator!=(Type const & rhs) const { return !(*this == rhs); }

CarModelTypes::CarModelTypes()
{
  for (auto const & additionalTag : routing::CarModel::GetAdditionalTags())
    m_hwtags.push_back(classif().GetTypeByPath(additionalTag.m_hwtag));

  for (auto const & speedForType : routing::CarModel::GetOptions())
    m_hwtags.push_back(classif().GetTypeByPath(speedForType.m_types));

  for (auto const & surface : routing::CarModel::GetSurfaces())
    m_surfaceTags.push_back(classif().GetTypeByPath(surface.m_types));

}

CarModelTypes::Type CarModelTypes::GetType(FeatureType & feature) const
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

bool RoadInfo::operator==(RoadInfo const & rhs) const
{
  return std::tie(m_type, m_maxspeedKMpH, m_isCityRoad, m_isOneWay) ==
         std::tie(rhs.m_type, rhs.m_maxspeedKMpH, rhs.m_isCityRoad, rhs.m_isOneWay);
}

bool RoadInfo::operator!=(RoadInfo const & rhs) const { return !(*this == rhs); }

bool RoadInfo::operator<(RoadInfo const & rhs) const
{
  return std::tie(m_type, m_maxspeedKMpH, m_isCityRoad, m_isOneWay) <
         std::tie(rhs.m_type, rhs.m_maxspeedKMpH, rhs.m_isCityRoad, rhs.m_isOneWay);
}

std::string RoadInfo::GetSummary() const
{
  std::ostringstream out;
  out << TypeToString(m_type.m_hwType) << "," << TypeToString(m_type.m_surfaceType) << ","
      << m_maxspeedKMpH << "," << m_isCityRoad << "," << m_isOneWay;
  return out.str();
}

MoveType::MoveType(RoadInfo const & roadType, traffic::SpeedGroup speedGroup,
                   DataPoint const & dataPoint)
  : m_roadInfo(roadType), m_speedGroup(speedGroup), m_latLon(dataPoint.m_latLon)
{
  m_isDayTime = DayTimeToBool(GetDayTime(dataPoint.m_timestamp, m_latLon.m_lat, m_latLon.m_lon));
}

bool MoveType::operator==(MoveType const & rhs) const
{
  return std::tie(m_roadInfo, m_speedGroup) == std::tie(rhs.m_roadInfo, rhs.m_speedGroup);
}

bool MoveType::operator<(MoveType const & rhs) const
{
  auto const lhsGroup = base::Underlying(m_speedGroup);
  auto const rhsGroup = base::Underlying(rhs.m_speedGroup);
  return std::tie(m_roadInfo, lhsGroup) < std::tie(rhs.m_roadInfo, rhsGroup);
}

bool MoveType::IsValid() const
{
  // In order to collect cleaner data we don't use speed group lower than G5.
  return m_roadInfo.m_type.m_hwType != 0 && m_roadInfo.m_type.m_surfaceType != 0 &&
         m_speedGroup == kValidTrafficValue;
}

std::string MoveType::GetSummary() const
{
  std::ostringstream out;
  out << m_roadInfo.GetSummary() << "," << m_isDayTime << "," << m_latLon.m_lat << " "
      << m_latLon.m_lon;

  return out.str();
}

void SpeedInfo::Add(double distance, uint64_t time,
                    routing::IsCrossroadChecker::CrossroadInfo const & crossroads)
{
  m_totalDistance += distance;
  m_totalTime += time;
  routing::IsCrossroadChecker::MergeCrossroads(crossroads, m_crossroads);
}

std::string SpeedInfo::GetSummary() const
{
  std::ostringstream out;
  out << m_totalDistance << "," << m_totalTime << "," << CalcSpeedKMpH(m_totalDistance, m_totalTime)
      << ",";

  for (size_t i = 1; i < m_crossroads.size(); ++i)
  {
    out << m_crossroads[i];
    if (i != m_crossroads.size() - 1)
      out << ",";
  }

  return out.str();
}

void MoveTypeAggregator::Add(MoveType && moveType,
                             routing::IsCrossroadChecker::CrossroadInfo const & crossroads,
                             MatchedTrack::const_iterator begin, MatchedTrack::const_iterator end,
                             routing::Geometry & geometry)
{
  if (begin + 1 >= end)
    return;

  auto const & beginDataPoint = begin->GetDataPoint();
  auto const & endDataPoint = (end - 1)->GetDataPoint();
  uint64_t const time = endDataPoint.m_timestamp - beginDataPoint.m_timestamp;

  if (time == 0)
  {
    LOG(LWARNING, ("Track with the same time at the beginning and at the end. Beginning:",
                   beginDataPoint.m_latLon, " End:", endDataPoint.m_latLon,
                   " Timestamp:", beginDataPoint.m_timestamp, " Segment:", begin->GetSegment()));
    return;
  }

  double const length = CalcSubtrackLength(begin, end, geometry);
  m_moveInfos[moveType].Add(length, time, crossroads);
}

std::string MoveTypeAggregator::GetSummary(std::string const & user, std::string const & mwm) const
{
  std::ostringstream out;
  for (auto const & it : m_moveInfos)
  {
    if (!it.first.IsValid())
      continue;

    out << user << "," << mwm << "," << it.first.GetSummary() << "," << it.second.GetSummary()
        << '\n';
  }

  return out.str();
}

MatchedTrackPointToMoveType::MatchedTrackPointToMoveType(
    FilesContainerR const & container, routing::VehicleModelInterface & vehicleModel)
  : m_featuresVector(container), m_vehicleModel(vehicleModel)
{
  if (container.IsExist(CITY_ROADS_FILE_TAG))
    LoadCityRoads(container.GetFileName(), container.GetReader(CITY_ROADS_FILE_TAG), m_cityRoads);

  if (container.IsExist(MAXSPEEDS_FILE_TAG))
    LoadMaxspeeds(container.GetReader(MAXSPEEDS_FILE_TAG), m_maxspeeds);
}

MoveType MatchedTrackPointToMoveType::GetMoveType(MatchedTrackPoint const & point)
{
  auto const & dataPoint = point.GetDataPoint();
  return MoveType(GetRoadInfo(point.GetSegment()),
                  static_cast<traffic::SpeedGroup>(dataPoint.m_traffic), dataPoint);
}

RoadInfo MatchedTrackPointToMoveType::GetRoadInfo(routing::Segment const & segment)
{
  auto const featureId = segment.GetFeatureId();
  if (featureId == m_prevFeatureId)
    return m_prevRoadInfo;

  auto feature = m_featuresVector.GetVector().GetByIndex(featureId);
  CHECK(feature, ());

  auto const maxspeed = m_maxspeeds.GetMaxspeed(featureId);
  auto const maxspeedValueKMpH =
      maxspeed.IsValid() ? std::min(maxspeed.GetSpeedKmPH(segment.IsForward()), kMaxspeedTopBound)
                         : routing::kInvalidSpeed;

  m_prevFeatureId = featureId;
  m_prevRoadInfo = {m_carModelTypes.GetType(*feature), maxspeedValueKMpH,
                    m_cityRoads.IsCityRoad(featureId), m_vehicleModel.IsOneWay(*feature)};

  return m_prevRoadInfo;
}
*/
}  // namespace track_analyzing
