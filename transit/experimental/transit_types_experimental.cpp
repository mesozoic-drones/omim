#include "transit/experimental/transit_types_experimental.hpp"

#include <tuple>

namespace transit
{
namespace experimental
{
double constexpr kPointsEqualEpsilon = 1e-5;

std::string GetTranslation(Translations const & titles)
{
  CHECK(!titles.empty(), ());

  // If there is only one language we return title in this only translation.
  if (titles.size() == 1)
    return titles.begin()->second;

  // Otherwise we try to extract default language for this region.
  auto it = titles.find("default");
  if (it != titles.end())
    return it->second;

  // If there is no default language we return one of the represented translations.
  return titles.begin()->second;
}

// TransitHeader ----------------------------------------------------------------------------------
TransitHeader::TransitHeader(uint16_t version, uint32_t stopsOffset, uint32_t gatesOffset,
                             uint32_t edgesOffset, uint32_t transfersOffset, uint32_t linesOffset,
                             uint32_t shapesOffset, uint32_t routesOffset, uint32_t networksOffset,
                             uint32_t endOffset)
  : m_version(version)
  , m_reserve(0)
  , m_stopsOffset(stopsOffset)
  , m_gatesOffset(gatesOffset)
  , m_edgesOffset(edgesOffset)
  , m_transfersOffset(transfersOffset)
  , m_linesOffset(linesOffset)
  , m_shapesOffset(shapesOffset)
  , m_routesOffset(routesOffset)
  , m_networksOffset(networksOffset)
  , m_endOffset(endOffset)
{
}

bool TransitHeader::IsEqualForTesting(TransitHeader const & header) const
{
  return m_version == header.m_version && m_reserve == header.m_reserve &&
         m_stopsOffset == header.m_stopsOffset && m_gatesOffset == header.m_gatesOffset &&
         m_edgesOffset == header.m_edgesOffset && m_transfersOffset == header.m_transfersOffset &&
         m_linesOffset == header.m_linesOffset && m_shapesOffset == header.m_shapesOffset &&
         m_routesOffset == header.m_routesOffset && m_networksOffset == header.m_networksOffset &&
         m_endOffset == header.m_endOffset;
}

bool TransitHeader::IsValid() const
{
  return m_stopsOffset <= m_gatesOffset && m_gatesOffset <= m_edgesOffset &&
         m_edgesOffset <= m_transfersOffset && m_transfersOffset <= m_linesOffset &&
         m_linesOffset <= m_shapesOffset && m_shapesOffset <= m_routesOffset &&
         m_routesOffset <= m_networksOffset && m_networksOffset <= m_endOffset;
}

// SingleMwmSegment --------------------------------------------------------------------------------
SingleMwmSegment::SingleMwmSegment(FeatureId featureId, uint32_t segmentIdx, bool forward)
  : m_featureId(featureId), m_segmentIdx(segmentIdx), m_forward(forward)
{
}

bool SingleMwmSegment::IsEqualForTesting(SingleMwmSegment const & s) const
{
  return m_featureId == s.m_featureId && m_segmentIdx == s.m_segmentIdx && m_forward == s.m_forward;
}

bool SingleMwmSegment::IsValid() const { return m_featureId != kInvalidFeatureId; }

// IdBundle ----------------------------------------------------------------------------------------
IdBundle::IdBundle(bool serializeFeatureIdOnly) : m_serializeFeatureIdOnly(serializeFeatureIdOnly)
{
}

IdBundle::IdBundle(FeatureId featureId, OsmId osmId, bool serializeFeatureIdOnly)
  : m_featureId(featureId), m_osmId(osmId), m_serializeFeatureIdOnly(serializeFeatureIdOnly)
{
}

bool IdBundle::operator<(IdBundle const & rhs) const
{
  CHECK_EQUAL(m_serializeFeatureIdOnly, rhs.m_serializeFeatureIdOnly, ());

  if (m_serializeFeatureIdOnly)
    return m_featureId < rhs.m_featureId;

  if (m_featureId != rhs.m_featureId)
    return m_featureId < rhs.m_featureId;

  return m_osmId < rhs.m_osmId;
}

bool IdBundle::operator==(IdBundle const & rhs) const
{
  CHECK_EQUAL(m_serializeFeatureIdOnly, rhs.m_serializeFeatureIdOnly, ());
  return m_serializeFeatureIdOnly ? m_featureId == rhs.m_featureId
                                  : m_featureId == rhs.m_featureId && m_osmId == rhs.m_osmId;
}

bool IdBundle::operator!=(IdBundle const & rhs) const { return !(*this == rhs); }

bool IdBundle::IsValid() const
{
  return m_serializeFeatureIdOnly ? m_featureId != kInvalidFeatureId
                                  : m_featureId != kInvalidFeatureId && m_osmId != kInvalidOsmId;
}

void IdBundle::SetOsmId(OsmId osmId) { m_osmId = osmId; }

void IdBundle::SetFeatureId(FeatureId featureId) { m_featureId = featureId; }

OsmId IdBundle::GetOsmId() const { return m_osmId; }

FeatureId IdBundle::GetFeatureId() const { return m_featureId; }

bool IdBundle::SerializeFeatureIdOnly() const { return m_serializeFeatureIdOnly; }

// Network -----------------------------------------------------------------------------------------
Network::Network(TransitId id, Translations const & title) : m_id(id), m_title(title) {}

Network::Network(TransitId id) : m_id(id), m_title{} {}

bool Network::operator<(Network const & rhs) const { return m_id < rhs.m_id; }

bool Network::operator==(Network const & rhs) const { return m_id == rhs.m_id; }

bool Network::IsEqualForTesting(Network const & network) const
{
  return m_id == network.m_id && m_title == network.m_title;
}

bool Network::IsValid() const { return m_id != kInvalidTransitId; }

TransitId Network::GetId() const { return m_id; }

std::string const Network::GetTitle() const { return GetTranslation(m_title); }

// Route -------------------------------------------------------------------------------------------
Route::Route(TransitId id, TransitId networkId, std::string const & routeType,
             Translations const & title, std::string const & color)
  : m_id(id), m_networkId(networkId), m_routeType(routeType), m_title(title), m_color(color)
{
}

bool Route::operator<(Route const & rhs) const { return m_id < rhs.m_id; }

bool Route::operator==(Route const & rhs) const { return m_id == rhs.m_id; }

bool Route::IsEqualForTesting(Route const & route) const
{
  return std::tie(m_id, m_networkId, m_routeType, m_title, m_color) ==
         std::tie(route.m_id, route.m_networkId, route.m_routeType, route.m_title, route.m_color);
}

bool Route::IsValid() const
{
  return m_id != kInvalidTransitId && m_networkId != kInvalidTransitId && !m_routeType.empty();
}

TransitId Route::GetId() const { return m_id; }

std::string const Route::GetTitle() const { return GetTranslation(m_title); }

std::string const & Route::GetType() const { return m_routeType; }

std::string const & Route::GetColor() const { return m_color; }

TransitId Route::GetNetworkId() const { return m_networkId; }

// Line --------------------------------------------------------------------------------------------
Line::Line(TransitId id, TransitId routeId, ShapeLink shapeLink, Translations const & title,
           Translations const & number, IdList stopIds, std::vector<LineInterval> const & intervals,
           osmoh::OpeningHours const & serviceDays)
  : m_id(id)
  , m_routeId(routeId)
  , m_shapeLink(shapeLink)
  , m_title(title)
  , m_number(number)
  , m_stopIds(stopIds)
  , m_intervals(intervals)
  , m_serviceDays(serviceDays)
{
}

bool Line::operator<(Line const & rhs) const { return m_id < rhs.m_id; }
bool Line::operator==(Line const & rhs) const { return m_id == rhs.m_id; }
bool Line::IsEqualForTesting(Line const & line) const
{
  return std::tie(m_id, m_routeId, m_shapeLink, m_title, m_number, m_stopIds) ==
         std::tie(line.m_id, line.m_routeId, line.m_shapeLink, line.m_title, line.m_number,
                  line.m_stopIds);
}

bool Line::IsValid() const
{
  return m_id != kInvalidTransitId && m_routeId != kInvalidTransitId &&
         m_shapeLink.m_shapeId != kInvalidTransitId && !m_stopIds.empty();
}

TransitId Line::GetId() const { return m_id; }

std::string Line::GetNumber() const { return GetTranslation(m_number); }

std::string Line::GetTitle() const { return GetTranslation(m_title); }

TransitId Line::GetRouteId() const { return m_routeId; }

ShapeLink const & Line::GetShapeLink() const { return m_shapeLink; }

IdList const & Line::GetStopIds() const { return m_stopIds; }

std::vector<LineInterval> Line::GetIntervals() const { return m_intervals; }

osmoh::OpeningHours Line::GetServiceDays() const { return m_serviceDays; }

// Gate --------------------------------------------------------------------------------------------
Gate::Gate() : m_ids(false /* serializeFeatureIdOnly */) {}

Gate::Gate(TransitId id, FeatureId featureId, OsmId osmId, bool entrance, bool exit,
           std::vector<TimeFromGateToStop> const & weights, m2::PointD const & point)
  : m_id(id)
  , m_ids(featureId, osmId, false /* serializeFeatureIdOnly */)
  , m_entrance(entrance)
  , m_exit(exit)
  , m_weights(weights)
  , m_point(point)
{
}

bool Gate::operator<(Gate const & rhs) const
{
  if (m_id != rhs.m_id)
    return m_id < rhs.m_id;

  if (m_ids != rhs.m_ids)
    return m_ids < rhs.m_ids;

  if (m_entrance != rhs.m_entrance)
    return m_entrance < rhs.m_entrance;

  return m_exit < rhs.m_exit;
}

bool Gate::operator==(Gate const & rhs) const
{
  return std::tie(m_id, m_ids, m_entrance, m_exit) ==
         std::tie(rhs.m_id, rhs.m_ids, rhs.m_entrance, rhs.m_exit);
}

bool Gate::IsEqualForTesting(Gate const & gate) const
{
  return *this == gate && base::AlmostEqualAbs(m_point, gate.m_point, kPointsEqualEpsilon);
}

bool Gate::IsValid() const
{
  return ((m_id != kInvalidTransitId) || (m_ids.GetOsmId() != kInvalidOsmId)) &&
         (m_entrance || m_exit) && !m_weights.empty();
}

void Gate::SetBestPedestrianSegments(std::vector<SingleMwmSegment> const & seg)
{
  m_bestPedestrianSegments = seg;
};

FeatureId Gate::GetFeatureId() const { return m_ids.GetFeatureId(); }

OsmId Gate::GetOsmId() const { return m_ids.GetOsmId(); }

std::vector<SingleMwmSegment> const & Gate::GetBestPedestrianSegments() const
{
  return m_bestPedestrianSegments;
}

bool Gate::IsEntrance() const { return m_entrance; }

bool Gate::IsExit() const { return m_exit; }

std::vector<TimeFromGateToStop> const & Gate::GetStopsWithWeight() const { return m_weights; }

m2::PointD const & Gate::GetPoint() const { return m_point; }

// Stop --------------------------------------------------------------------------------------------
Stop::Stop() : m_ids(true /* serializeFeatureIdOnly */) {}

Stop::Stop(TransitId id, FeatureId featureId, OsmId osmId, Translations const & title,
           TimeTable const & timetable, m2::PointD const & point)
  : m_id(id)
  , m_ids(featureId, osmId, true /* serializeFeatureIdOnly */)
  , m_title(title)
  , m_timetable(timetable)
  , m_point(point)
{
}

Stop::Stop(TransitId id) : m_id(id), m_ids(true /* serializeFeatureIdOnly */) {}

bool Stop::operator<(Stop const & rhs) const
{
  if (m_id != kInvalidTransitId || rhs.m_id != kInvalidTransitId)
    return m_id < rhs.m_id;

  return m_ids.GetFeatureId() < rhs.m_ids.GetFeatureId();
}

bool Stop::operator==(Stop const & rhs) const
{
  if (m_id != kInvalidTransitId || rhs.m_id != kInvalidTransitId)
    return m_id == rhs.m_id;

  return m_ids.GetFeatureId() == rhs.m_ids.GetFeatureId();
}

bool Stop::IsEqualForTesting(Stop const & stop) const
{
  return (std::tie(m_id, m_ids, m_title, m_timetable) ==
          std::tie(stop.m_id, stop.m_ids, stop.m_title, stop.m_timetable)) &&
         base::AlmostEqualAbs(m_point, stop.m_point, kPointsEqualEpsilon);
}

bool Stop::IsValid() const
{
  return ((m_id != kInvalidTransitId) || (m_ids.GetOsmId() != kInvalidOsmId)) && !m_title.empty();
}

void Stop::SetBestPedestrianSegments(std::vector<SingleMwmSegment> const & seg)
{
  m_bestPedestrianSegments = seg;
}

std::vector<SingleMwmSegment> const & Stop::GetBestPedestrianSegments() const
{
  return m_bestPedestrianSegments;
}

FeatureId Stop::GetId() const { return m_id; }

FeatureId Stop::GetFeatureId() const { return m_ids.GetFeatureId(); }

OsmId Stop::GetOsmId() const { return m_ids.GetOsmId(); }

std::string Stop::GetTitle() const { return GetTranslation(m_title); }

TimeTable const & Stop::GetTimeTable() const { return m_timetable; }

m2::PointD const & Stop::GetPoint() const { return m_point; }

// Edge --------------------------------------------------------------------------------------------
Edge::Edge(TransitId stop1Id, TransitId stop2Id, EdgeWeight weight, TransitId lineId, bool transfer,
           ShapeLink const & shapeLink)
  : m_stop1Id(stop1Id)
  , m_stop2Id(stop2Id)
  , m_weight(weight)
  , m_isTransfer(transfer)
  , m_lineId(lineId)
  , m_shapeLink(shapeLink)
{
}

bool Edge::operator<(Edge const & rhs) const
{
  if (m_stop1Id != rhs.m_stop1Id)
    return m_stop1Id < rhs.m_stop1Id;
  if (m_stop2Id != rhs.m_stop2Id)
    return m_stop2Id < rhs.m_stop2Id;
  return m_lineId < rhs.m_lineId;
}

bool Edge::operator==(Edge const & rhs) const
{
  return std::tie(m_stop1Id, m_stop2Id, m_lineId) ==
         std::tie(rhs.m_stop1Id, rhs.m_stop2Id, rhs.m_lineId);
}

bool Edge::operator!=(Edge const & rhs) const { return !(*this == rhs); }
bool Edge::IsEqualForTesting(Edge const & edge) const
{
  return (*this == edge) && (std::tie(m_isTransfer, m_shapeLink, m_weight) ==
                             std::tie(edge.m_isTransfer, edge.m_shapeLink, edge.m_weight));
}

bool Edge::IsValid() const
{
  if (m_isTransfer && (m_lineId != kInvalidTransitId || m_shapeLink.m_shapeId != kInvalidTransitId))
    return false;

  if (!m_isTransfer && m_lineId == kInvalidTransitId)
    return false;

  return m_stop1Id != kInvalidTransitId && m_stop2Id != kInvalidTransitId;
}

void Edge::SetWeight(EdgeWeight weight) { m_weight = weight; }

TransitId Edge::GetStop1Id() const { return m_stop1Id; }

TransitId Edge::GetStop2Id() const { return m_stop2Id; }

EdgeWeight Edge::GetWeight() const { return m_weight; }

TransitId Edge::GetLineId() const { return m_lineId; }

bool Edge::IsTransfer() const { return m_isTransfer; }

ShapeLink const & Edge::GetShapeLink() const { return m_shapeLink; }

// Transfer ----------------------------------------------------------------------------------------
Transfer::Transfer(TransitId id, m2::PointD const & point, IdList const & stopIds)
  : m_id(id), m_point(point), m_stopIds(stopIds)
{
}

bool Transfer::operator<(Transfer const & rhs) const { return m_id < rhs.m_id; }

bool Transfer::operator==(Transfer const & rhs) const { return m_id == rhs.m_id; }

bool Transfer::IsEqualForTesting(Transfer const & transfer) const
{
  return std::tie(m_id, m_point, m_stopIds) ==
         std::tie(transfer.m_id, transfer.m_point, transfer.m_stopIds);
}

bool Transfer::IsValid() const { return m_id != kInvalidTransitId && m_stopIds.size() > 1; }

TransitId Transfer::GetId() const { return m_id; }

m2::PointD const & Transfer::GetPoint() const { return m_point; }

IdList const & Transfer::GetStopIds() const { return m_stopIds; }

// Shape -------------------------------------------------------------------------------------------
Shape::Shape(TransitId id, std::vector<m2::PointD> const & polyline)
  : m_id(id), m_polyline(polyline)
{
}

bool Shape::operator<(Shape const & rhs) const { return m_id < rhs.m_id; }

bool Shape::operator==(Shape const & rhs) const { return m_id == rhs.m_id; }

bool Shape::IsEqualForTesting(Shape const & shape) const
{
  return std::tie(m_id, m_polyline) == std::tie(shape.m_id, shape.m_polyline);
}

bool Shape::IsValid() const { return m_id != kInvalidTransitId && m_polyline.size() > 1; }

TransitId Shape::GetId() const { return m_id; }

std::vector<m2::PointD> const & Shape::GetPolyline() const { return m_polyline; }
}  // namespace experimental
}  // namespace transit
