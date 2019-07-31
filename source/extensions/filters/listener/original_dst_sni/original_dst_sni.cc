#include "extensions/filters/listener/original_dst_sni/original_dst_sni.h"

#include <regex>
#include <sstream>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "envoy/network/listen_socket.h"

#include "common/network/address_impl.h"
#include "common/common/assert.h"
#include "common/network/utility.h"
#include "common/singleton/const_singleton.h"

namespace Envoy {
namespace Extensions {
namespace ListenerFilters {
namespace OriginalDstSni {

class ConstantValues {
public:
  const std::regex NODE_PATTERN{"^[a-z0-9]{4}\\.", std::regex::optimize};
  const u_int16_t PORT = 9092; //TODO - get from config
};
using Constants = ConstSingleton<ConstantValues>;

Network::FilterStatus OriginalDstSniFilter::onAccept(Network::ListenerFilterCallbacks& cb) {
  ENVOY_LOG(debug, "original_dst_sni: New connection accepted");
  Network::ConnectionSocket& socket = cb.socket();

  //TODO - get this via config
  u_int32_t first_2_octects;
  sscanf("36d5", "%x", &first_2_octects);

  //TLS inspector must be configured first so that this is set from SNI
  auto server_name = socket.requestedServerName();
  if (!server_name.empty()) {
    // Check if server name matches broker pattern
    if (matchesNodeNamePattern(server_name)) {
      auto addr = getNodeAddress(first_2_octects, server_name);
      //Set this as "local address" so the the original destination LB will use it
      socket.restoreLocalAddress(addr);
    }
  }

  return Network::FilterStatus::Continue;
}

bool OriginalDstSniFilter::matchesNodeNamePattern(absl::string_view server_name) {
  return std::regex_search(server_name.begin(), server_name.end(), Constants::get().NODE_PATTERN);
}

Envoy::Network::Address::InstanceConstSharedPtr OriginalDstSniFilter::getNodeAddress(u_int32_t first_2_octets, absl::string_view server_name) {
  auto hex_str = server_name.substr(0, 4);
  auto last_2_octets = parseHexOctet(hex_str);
  return makeAddress(first_2_octets, last_2_octets);
}

u_int32_t OriginalDstSniFilter::parseHexOctet(absl::string_view hex_str) {
  std::stringstream ss;
  ss << std::hex << hex_str;
  u_int32_t result;
  ss >> result;
  return result;
}

Envoy::Network::Address::InstanceConstSharedPtr OriginalDstSniFilter::makeAddress(u_int32_t first_2_octets, u_int32_t last_2_octets) {
  in_addr_t iaddr = (htonl(first_2_octets) >> 16) | htonl(last_2_octets);
  sockaddr_in sa4;
  sa4.sin_addr = in_addr{iaddr};
  sa4.sin_family = AF_INET;
  sa4.sin_port = htons(Constants::get().PORT);
  return std::make_shared<Envoy::Network::Address::Ipv4Instance>(&sa4);
}

} // namespace OriginalDstSni
} // namespace ListenerFilters
} // namespace Extensions
} // namespace Envoy
