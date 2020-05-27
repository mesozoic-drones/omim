#include "generator/transit_generator_experimental.hpp"

#include "base/assert.hpp"
#include "base/logging.hpp"

namespace transit_experimental
{
TransitGenerator::TransitGenerator() : m_worldFeed(m_generator, m_colorPicker) {}

void TransitGenerator::LoadData(std::string const & transitDir)
{
  // Transit section can be empty.
  m_loaded = m_worldFeed.Load(transitDir);
}

bool TransitGenerator::IsLoaded() const { return m_loaded; }

void TransitGenerator::BuildTransit(std::string const & mwmDir,
                                    storage::CountryId const & countryId,
                                    std::string const & osmIdToFeatureIdsPath)
{
  // TODO(o.khlopkova): Add implementation.
  LOG(LINFO, ("Building transit section for", countryId, "mwmDir:", mwmDir));
}
}  // namespace transit_experimental
