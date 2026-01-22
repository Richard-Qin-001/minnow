#include <iostream>

#include "arp_message.hh"
#include "debug.hh"
#include "ethernet_frame.hh"
#include "exception.hh"
#include "helpers.hh"
#include "network_interface.hh"

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( string_view name,
                                    shared_ptr<OutputPort> port,
                                    const EthernetAddress& ethernet_address,
                                    const Address& ip_address )
  : name_( name )
  , port_( notnull( "OutputPort", move( port ) ) )
  , ethernet_address_( ethernet_address )
  , ip_address_( ip_address )
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address_ ) << " and IP address "
       << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but
//! may also be another host if directly connected to the same network as the destination) Note: the Address type
//! can be converted to a uint32_t (raw 32-bit IP address) by using the Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( const InternetDatagram& dgram, const Address& next_hop )
{
  // debug( "unimplemented send_datagram called" );
  // (void)dgram;
  // (void)next_hop;
  const uint32_t target_ip = next_hop.ipv4_numeric();
  const auto& arp_iter = arp_table_.find( target_ip );
  if ( arp_iter != arp_table_.end() && arp_iter->second.expiration_time > current_time_ ) {
    EthernetFrame frame;
    frame.header.type = EthernetHeader::TYPE_IPv4;
    frame.header.src = ethernet_address_;
    frame.header.dst = arp_iter->second.mac;
    frame.payload = serialize( dgram );
    transmit( frame );
  } else {
    waiting_datagrams_[target_ip].push_back( dgram );
    const auto& timer_iter = arp_request_timestamps_.find( target_ip );
    if ( timer_iter == arp_request_timestamps_.end()
         || ( current_time_ - timer_iter->second ) > ARP_REQUEST_TIMEOUT_MS ) {
      ARPMessage arp_request;
      arp_request.opcode = ARPMessage::OPCODE_REQUEST;
      arp_request.sender_ip_address = ip_address_.ipv4_numeric();
      arp_request.sender_ethernet_address = ethernet_address_;
      arp_request.target_ip_address = target_ip;
      arp_request.target_ethernet_address = {};

      EthernetFrame frame;
      frame.header.type = EthernetHeader::TYPE_ARP;
      frame.header.src = ethernet_address_;
      frame.header.dst = ETHERNET_BROADCAST;
      frame.payload = serialize( arp_request );

      transmit( frame );

      arp_request_timestamps_[target_ip] = current_time_;
    }
  }
}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( EthernetFrame frame )
{
  // debug( "unimplemented recv_frame called" );
  // (void)frame;
  const auto& header_dst = frame.header.dst;
  if ( header_dst == ethernet_address_ || header_dst == ETHERNET_BROADCAST ) {
    const auto& header_type = frame.header.type;
    if ( header_type == EthernetHeader::TYPE_IPv4 ) {
      InternetDatagram dgram;
      if ( parse( dgram, frame.payload ) ) {
        datagrams_received_.push( dgram );
      }
    } else if ( header_type == EthernetHeader::TYPE_ARP ) {
      ARPMessage arp_msg;
      if ( parse( arp_msg, frame.payload ) ) {
        const uint32_t sender_ip = arp_msg.sender_ip_address;
        const EthernetAddress sender_eth = arp_msg.sender_ethernet_address;
        arp_table_[sender_ip] = { sender_eth, current_time_ + 30000 };
        auto iter = waiting_datagrams_.find( sender_ip );
        if ( iter != waiting_datagrams_.end() ) {
          for ( const auto& dgram : iter->second ) {
            EthernetFrame new_frame;
            new_frame.header.type = EthernetHeader::TYPE_IPv4;
            new_frame.header.src = ethernet_address_;
            new_frame.header.dst = sender_eth;
            new_frame.payload = serialize( dgram );
            transmit( new_frame );
          }
          waiting_datagrams_.erase( iter );
        }
        if ( arp_msg.opcode == ARPMessage::OPCODE_REQUEST
             && arp_msg.target_ip_address == ip_address_.ipv4_numeric() ) {
          ARPMessage arp_reply;
          arp_reply.opcode = ARPMessage::OPCODE_REPLY;
          arp_reply.sender_ip_address = ip_address_.ipv4_numeric();
          arp_reply.sender_ethernet_address = ethernet_address_;
          arp_reply.target_ip_address = sender_ip;
          arp_reply.target_ethernet_address = sender_eth;

          EthernetFrame new_frame;
          new_frame.header.src = ethernet_address_;
          new_frame.header.dst = sender_eth;
          new_frame.header.type = EthernetHeader::TYPE_ARP;
          new_frame.payload = serialize( arp_reply );
          transmit( new_frame );
        }
      }
    }
  }
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  // debug( "unimplemented tick({}) called", ms_since_last_tick );
  current_time_ += ms_since_last_tick;
  auto iter = arp_table_.begin();
  while ( iter != arp_table_.end() ) {
    if ( iter->second.expiration_time <= current_time_ ) {
      iter = arp_table_.erase( iter );
    } else {
      iter++;
    }
  }
  for ( const auto& [ip, last_request_time] : arp_request_timestamps_ ) {
    if ( current_time_ > last_request_time + ARP_REQUEST_TIMEOUT_MS ) {
      waiting_datagrams_.erase( ip );
    }
  }
}
