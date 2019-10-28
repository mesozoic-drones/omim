#pragma once

#include "routing/geometry.hpp"
#include "routing/vehicle_mask.hpp"

#include "routing_common/vehicle_model.hpp"

#include "storage/storage.hpp"

#include <cstdint>
#include <string>

namespace track_analyzing
{
struct TrackParams
{
  // Minimum track length in meters.
  double m_minLength;
  // Minimum track average speed in km/hour.
  double m_minSpeed;
  // Maximum track average speed in km/hour
  double m_maxSpeed;
  // Minimum track duration in seconds
  uint64_t m_minDuration;
  // Ignore tracks with traffic data
  bool m_ignoreTraffic;
};

TrackParams GetParamsForType(routing::VehicleType const & trackType);

std::shared_ptr<routing::VehicleModelInterface> GetVehicleModel(
    routing::VehicleType const & vehicleType, std::string country);

std::shared_ptr<routing::VehicleModelFactoryInterface> GetVehicleModelFactory(
    routing::VehicleType const & vehicleType);

routing::VehicleType GetVehicleType(std::string const & trackType);

void InitModelData(routing::VehicleType const & trackType, std::string const & mwmName,
                   storage::Storage const & storage,
                   std::shared_ptr<routing::VehicleModelInterface> & vehicleModel,
                   routing::Geometry & geometry);
}  // namespace track_analyzing
