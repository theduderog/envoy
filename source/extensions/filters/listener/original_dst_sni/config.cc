#include <string>

#include "envoy/registry/registry.h"
#include "envoy/server/filter_config.h"

#include "extensions/filters/listener/original_dst_sni/original_dst_sni.h"
#include "extensions/filters/listener/well_known_names.h"

namespace Envoy {
namespace Extensions {
namespace ListenerFilters {
namespace OriginalDstSni {

/**
 * Config registration for the original dst filter. @see NamedNetworkFilterConfigFactory.
 */
class OriginalDstSniConfigFactory : public Server::Configuration::NamedListenerFilterConfigFactory {
public:
  // NamedListenerFilterConfigFactory
  Network::ListenerFilterFactoryCb
  createFilterFactoryFromProto(const Protobuf::Message&,
                               Server::Configuration::ListenerFactoryContext&) override {
    return [](Network::ListenerFilterManager& filter_manager) -> void {
      filter_manager.addAcceptFilter(std::make_unique<OriginalDstSniFilter>());
    };
  }

  ProtobufTypes::MessagePtr createEmptyConfigProto() override {
    return std::make_unique<Envoy::ProtobufWkt::Empty>();
  }

  std::string name() override { return ListenerFilterNames::get().OriginalDstSni; }
};

/**
 * Static registration for the original dst filter. @see RegisterFactory.
 */
REGISTER_FACTORY(OriginalDstSniConfigFactory, Server::Configuration::NamedListenerFilterConfigFactory);

} // namespace OriginalDstSni
} // namespace ListenerFilters
} // namespace Extensions
} // namespace Envoy
