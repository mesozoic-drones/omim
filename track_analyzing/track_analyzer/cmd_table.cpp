#include "track_analyzing/track_analyzer/crossroad_checker.hpp"
//#include "track_analyzing/track_analyzer/table_car_model.hpp"
#include "track_analyzing/track_analyzer/track_type_handler.hpp"
#include "track_analyzing/track_analyzer/table_common_model.hpp"

#include "track_analyzing/track.hpp"
#include "track_analyzing/utils.hpp"

#include "routing/city_roads.hpp"
#include "routing/geometry.hpp"
#include "routing/index_graph.hpp"
#include "routing/index_graph_loader.hpp"

#include "routing_common/vehicle_model.hpp"

#include "routing_common/car_model.hpp"
#include "routing_common/pedestrian_model.hpp"
#include "routing_common/bicycle_model.hpp"

#include "indexer/data_source.hpp"

#include "storage/routing_helpers.hpp"
#include "storage/storage.hpp"

#include "coding/file_reader.hpp"

#include "base/assert.hpp"
#include "base/logging.hpp"

#include <iostream>
#include <limits>

#include <memory>
#include <sstream>

using namespace routing;
using namespace std;
using namespace track_analyzing;

namespace track_analyzing
{
void CmdTagsTable(string const & filepath, string const & trackExtension, StringFilter mwmFilter,
                  StringFilter userFilter, routing::VehicleType const & trackType)
{
  cout << GetCsvTableHeader(trackType) << endl;
  storage::Storage storage;
  storage.RegisterAllLocalMaps(false /* enableDiffs */);
  FrozenDataSource dataSource;
  auto numMwmIds = CreateNumMwmIds(storage);

  auto processMwm = [&](string const & mwmName, UserToMatchedTracks const & userToMatchedTracks) {
    if (mwmFilter(mwmName))
      return;

    auto const countryName = storage.GetTopmostParentFor(mwmName);
    auto const modelFactory = GetVehicleModelFactory(trackType);
    shared_ptr<VehicleModelInterface> vehicleModel =
        modelFactory->GetVehicleModelForCountry(mwmName);
    string const mwmFile = GetCurrentVersionMwmFile(storage, mwmName);
    Geometry geometry(GeometryLoader::CreateFromFile(mwmFile, vehicleModel));

    auto const edgeEstimator = EdgeEstimator::Create(trackType, *vehicleModel, nullptr /* trafficStash */);
    auto indexGraphLoader = IndexGraphLoader::Create(trackType, false /* loadAltitudes */, numMwmIds,
                                                     modelFactory, edgeEstimator, dataSource);

    platform::CountryFile const countryFile(mwmName);
    auto localCountryFile = storage.GetLatestLocalFile(countryFile);
    CHECK(localCountryFile, ("Can't find latest country file for", countryFile.GetName()));
    if (!dataSource.IsLoaded(countryFile))
    {
      auto registerResult = dataSource.Register(*localCountryFile);
      CHECK_EQUAL(registerResult.second, MwmSet::RegResult::Success,
                  ("Can't register mwm", countryFile.GetName()));
    }

    auto const mwmId = numMwmIds->GetId(countryFile);
    IsCrossroadChecker checker(indexGraphLoader->GetIndexGraph(mwmId), geometry);
    shared_ptr<ModelTypesBase> modelTypes = nullptr;
    switch (trackType)
    {
    case routing::VehicleType::Car:
      modelTypes = make_shared<ModelTypes<routing::CarModel>>();
      break;
    case routing::VehicleType::Bicycle:
      modelTypes = make_shared<ModelTypes<routing::BicycleModel>>();
      break;
    case routing::VehicleType::Pedestrian:
      modelTypes = shared_ptr<ModelTypes<routing::PedestrianModel>>(new ModelTypes<routing::PedestrianModel>);//make_shared<ModelTypes<routing::PedestrianModel>>();
      break;
    default:
      UNREACHABLE();
    }

    for (auto const & kv : userToMatchedTracks)
    {
      cout << kv.first;
    }
    /* MatchedTrackPointToMoveType pointToMoveType(FilesContainerR(make_unique<FileReader>(mwmFile)), *vehicleModel);
    for (auto const & kv : userToMatchedTracks)
    {
      string const & user = kv.first;
      if (userFilter(user))
        continue;

      for (auto const & track : kv.second)
      {
        if (track.size() <= 1)
          continue;

        MoveTypeAggregator aggregator;
        IsCrossroadChecker::CrossroadInfo info{};
        for (auto subtrackBegin = track.begin(); subtrackBegin != track.end();)
        {
          auto moveType = pointToMoveType.GetMoveType(*subtrackBegin);
          auto prev = subtrackBegin;
          auto end = subtrackBegin + 1;
          while (end != track.end() && pointToMoveType.GetMoveType(*end) == moveType)
          {
            IsCrossroadChecker::MergeCrossroads(checker(prev->GetSegment(), end->GetSegment()),
                                                info);
            prev = end;
            ++end;
          }

          // If it's not the end of the track than it could be a crossroad.
          IsCrossroadChecker::CrossroadInfo crossroad{};
          if (end != track.end())
          {
            crossroad = checker(prev->GetSegment(), end->GetSegment());
            IsCrossroadChecker::MergeCrossroads(crossroad, info);
          }

          aggregator.Add(move(moveType), info, subtrackBegin, end, geometry);
          subtrackBegin = end;
          info = move(crossroad);
        }

        auto const summary = aggregator.GetSummary(user, countryName);
        if (!summary.empty())
          cout << summary;
      }
    } */
  };

  auto processTrack = [&](string const & filename, MwmToMatchedTracks const & mwmToMatchedTracks) {
    LOG(LINFO, ("Processing", filename));
    ForTracksSortedByMwmName(mwmToMatchedTracks, *numMwmIds, processMwm);
  };

  ForEachTrackFile(filepath, trackExtension, numMwmIds, processTrack);
}
}  // namespace track_analyzing
