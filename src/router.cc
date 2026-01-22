#include "router.hh"
#include "debug.hh"

#include <iostream>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{
  cerr << "DEBUG: adding route " << Address::from_ipv4_numeric( route_prefix ).ip() << "/"
       << static_cast<int>( prefix_length ) << " => " << ( next_hop.has_value() ? next_hop->ip() : "(direct)" )
       << " on interface " << interface_num << "\n";

  // debug( "unimplemented add_route() called" );
  TrieNode* curr = root_.get();
  for ( uint8_t i = 0; i < prefix_length; ++i ) {
    const uint8_t bit = ( route_prefix >> ( 31 - i ) ) & 1;
    if ( !curr->children.at( bit ) ) {
      curr->children.at( bit ) = std::make_unique<TrieNode>();
    }
    curr = curr->children.at( bit ).get();
  }
  curr->entry = { route_prefix, prefix_length, next_hop, interface_num };
}

// Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
void Router::route()
{
  // debug( "unimplemented route() called" );
  for ( auto& my_interface : interfaces_ ) {
    auto& dgram_queue = my_interface->datagrams_received();
    while ( !dgram_queue.empty() ) {
      auto dgram = dgram_queue.front();
      dgram_queue.pop();

      auto best_match = match_longest_prefix( dgram.header.dst );

      if ( best_match.has_value() && dgram.header.ttl > 1 ) {
        dgram.header.ttl--;
        dgram.header.compute_checksum();
        const Address next_hop = best_match->next_hop.has_value() ? best_match->next_hop.value()
                                                                  : Address::from_ipv4_numeric( dgram.header.dst );
        interface( best_match->interface_num )->send_datagram( dgram, next_hop );
      }
    }
  }
}

std::optional<Router::RouteEntry> Router::match_longest_prefix( uint32_t target_ip ) const
{
  std::optional<RouteEntry> best_match;
  TrieNode* curr = root_.get();

  if ( curr->entry.has_value() ) {
    best_match = curr->entry;
  }
  for ( uint8_t i = 0; i < 32; ++i ) {
    const uint8_t bit = ( target_ip >> ( 31 - i ) ) & 1;
    if ( !curr->children.at( bit ) ) {
      break;
    }
    curr = curr->children.at( bit ).get();
    if ( curr->entry.has_value() ) {
      best_match = curr->entry;
    }
  }
  return best_match;
}