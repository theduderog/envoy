#pragma once

#include "envoy/network/filter.h"

#include "common/common/logger.h"

namespace Envoy {
namespace Extensions {
namespace ListenerFilters {
namespace OriginalDstSni {

/**
 * Implementation of an original destination listener filter.
 */
class OriginalDstSniFilter : public Network::ListenerFilter, Logger::Loggable<Logger::Id::filter> {
public:
  //virtual Network::Address::InstanceConstSharedPtr getOriginalDst(int fd);

  // Network::ListenerFilter
  Network::FilterStatus onAccept(Network::ListenerFilterCallbacks& cb) override;

private:
  bool matchesNodeNamePattern(absl::string_view server_name);
  Envoy::Network::Address::InstanceConstSharedPtr getNodeAddress(u_int32_t first_2_octets, absl::string_view server_name);
  u_int32_t parseHexOctet(absl::string_view hex_str);
  Envoy::Network::Address::InstanceConstSharedPtr makeAddress(u_int32_t first_2_octets, u_int32_t last_2_octets);
};

} // namespace OriginalDstSni
} // namespace ListenerFilters
} // namespace Extensions
} // namespace Envoy
