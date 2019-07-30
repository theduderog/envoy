#include "extensions/filters/listener/original_dst_sni/original_dst_sni.h"

#include "envoy/network/listen_socket.h"

#include "common/common/assert.h"
#include "common/network/utility.h"

namespace Envoy {
namespace Extensions {
namespace ListenerFilters {
namespace OriginalDstSni {

Network::Address::InstanceConstSharedPtr OriginalDstSniFilter::getOriginalDst(int fd) {
  return Network::Utility::getOriginalDst(fd);
}

Network::FilterStatus OriginalDstSniFilter::onAccept(Network::ListenerFilterCallbacks& cb) {
  ENVOY_LOG(debug, "original_dst_sni: New connection accepted");
  Network::ConnectionSocket& socket = cb.socket();

  auto serverName = socket.requestedServerName();
  if (!serverName.empty()) {
    //ENVOY_LOG(debug, serverName);
    auto addr = Network::Utility::parseInternetAddressAndPort("54.213.74.14:9092", false);
    socket.restoreLocalAddress(addr);
  }

  // if (local_address.type() == Network::Address::Type::Ip) {
  //   Network::Address::InstanceConstSharedPtr original_local_address =
  //       getOriginalDst(socket.ioHandle().fd());

  //   // A listener that has the use_original_dst flag set to true can still receive
  //   // connections that are NOT redirected using iptables. If a connection was not redirected,
  //   // the address returned by getOriginalDst() matches the local address of the new socket.
  //   // In this case the listener handles the connection directly and does not hand it off.
  //   if (original_local_address) {
  //     // Restore the local address to the original one.
  //     socket.restoreLocalAddress(original_local_address);
  //   }
  // }

  return Network::FilterStatus::Continue;
}

} // namespace OriginalDstSni
} // namespace ListenerFilters
} // namespace Extensions
} // namespace Envoy
