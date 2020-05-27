#pragma once

#include "storage/storage_defines.hpp"

#include "transit/transit_graph_data.hpp"
#include "transit/world_feed/color_picker.hpp"
#include "transit/world_feed/world_feed.hpp"

#include <string>

namespace transit_experimental
{
class TransitGenerator
{
public:
  TransitGenerator();

  void LoadData(std::string const & transitDir);
  void BuildTransit(std::string const & mwmDir, storage::CountryId const & countryId,
                    std::string const & osmIdToFeatureIdsPath);
  bool IsLoaded() const;

private:
  transit::IdGenerator m_generator;
  transit::ColorPicker m_colorPicker;
  transit::WorldFeed m_worldFeed;
  bool m_loaded = false;
};
}  // namespace transit_experimental
