#include "track_analyzing/track_analyzer/track_type_handler.hpp"
#include "track_analyzing/utils.hpp"

#include "routing_common/bicycle_model.hpp"
#include "routing_common/car_model.hpp"
#include "routing_common/pedestrian_model.hpp"

#include "base/assert.hpp"
#include "base/logging.hpp"

namespace track_analyzing
{
TrackParams GetParamsForType(routing::VehicleType const & trackType)
{
  switch (trackType)
  {
  case routing::VehicleType::Car: return {1000.0, 15.0, 110.0, 5 * 60, true};
  case routing::VehicleType::Bicycle: return {300.0, 4.0, 50.0, 5, true};
  case routing::VehicleType::Pedestrian: return {100.0, 0.1, 20.0, 5, true};
  default: LOG(LERROR, ("Unknown track type", (trackType))); UNREACHABLE();
  }
}

std::shared_ptr<routing::VehicleModelInterface> GetVehicleModel(
    routing::VehicleType const & vehicleType, std::string country)
{
  switch (vehicleType)
  {
  case routing::VehicleType::Car:
    return routing::CarModelFactory({}).GetVehicleModelForCountry(country);
  case routing::VehicleType::Bicycle:
    return routing::BicycleModelFactory().GetVehicleModelForCountry(country);
  case routing::VehicleType::Pedestrian:
    return routing::PedestrianModelFactory().GetVehicleModelForCountry(country);
  default: UNREACHABLE();
  }
}

std::string GetCsvTableHeader(routing::VehicleType const & trackType)
{
  switch (trackType)
  {
  case routing::VehicleType::Car:
    return "user,mwm,hw type,surface type,maxspeed km/h,is city road,is one way,is day,lat "
           "lon,distance,time,"
           "mean speed km/h,turn from smaller to bigger,turn from bigger to smaller,from link,to "
           "link,"
           "intersection with big,intersection with small,intersection with link";
  case routing::VehicleType::Bicycle:
    return "user,mwm,hw type,surface type,maxspeed km/h,is city road,is day,lat "
                      "lon,distance,time,"
                      "mean speed km/h,turn from smaller to bigger,turn from bigger to smaller,from link,to "
                      "link,"
                      "intersection with big,intersection with small,intersection with link";
  case routing::VehicleType::Pedestrian:
    return "user,mwm,hw type,surface type,maxspeed km/h,is city road,is day,lat "
           "lon,distance,time,"
           "mean speed km/h,turn from smaller to bigger,turn from bigger to smaller,from link,to "
           "link,"
           "intersection with big,intersection with small,intersection with link";
  default: UNREACHABLE();
  }
}

std::shared_ptr<routing::VehicleModelFactoryInterface> GetVehicleModelFactory(
    routing::VehicleType const & vehicleType)
{
  switch (vehicleType)
  {
  case routing::VehicleType::Car:
    return std::make_shared<routing::CarModelFactory>(routing::VehicleModelFactory::CountryParentNameGetterFn{});
  case routing::VehicleType::Bicycle:
    return std::make_shared<routing::BicycleModelFactory>(routing::VehicleModelFactory::CountryParentNameGetterFn{});
  case routing::VehicleType::Pedestrian:
    return std::make_shared<routing::PedestrianModelFactory>(routing::VehicleModelFactory::CountryParentNameGetterFn{});
  default: UNREACHABLE();
  }
}

routing::VehicleType GetVehicleType(std::string const & trackType)
{
  if (trackType == "car")
    return routing::VehicleType::Car;
  if (trackType == "bicycle")
    return routing::VehicleType::Bicycle;
  if (trackType == "pedestrian")
    return routing::VehicleType::Pedestrian;
  UNREACHABLE();
}

void InitModelData(routing::VehicleType const & trackType, std::string const & mwmName,
                   storage::Storage const & storage,
                   std::shared_ptr<routing::VehicleModelInterface> & vehicleModel,
                   routing::Geometry & geometry)
{
  switch (trackType)
  {
  case routing::VehicleType::Car:
    vehicleModel = routing::CarModelFactory({}).GetVehicleModelForCountry(mwmName);
    break;
  case routing::VehicleType::Bicycle:
    vehicleModel = routing::BicycleModelFactory().GetVehicleModelForCountry(mwmName);
    break;
  case routing::VehicleType::Pedestrian:
    vehicleModel = routing::PedestrianModelFactory().GetVehicleModelForCountry(mwmName);
    break;
  default: UNREACHABLE();
  }
  geometry = routing::Geometry(routing::GeometryLoader::CreateFromFile(
      GetCurrentVersionMwmFile(storage, mwmName), vehicleModel));
}
}  // namespace track_analyzing
