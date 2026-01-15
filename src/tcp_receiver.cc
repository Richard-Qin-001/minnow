#include "tcp_receiver.hh"
#include "debug.hh"

using namespace std;

void TCPReceiver::receive( const TCPSenderMessage& message )
{
  // Your code here.
  // debug( "unimplemented receive() called" );
  // void(message);
  if ( message.RST ) {
    reassembler_.reader().set_error();
    return;
  }
  if ( !this->isn_ ) {
    if ( !message.SYN ) {
      return;
    }
    this->isn_.emplace( message.seqno );
  }
  const uint64_t checkpoint = writer().bytes_pushed() + 1;
  const uint64_t abs_seqno = message.seqno.unwrap( *( this->isn_ ), checkpoint );
  const uint64_t stream_idx = abs_seqno - 1 + ( message.SYN ? 1 : 0 );
  reassembler_.insert( stream_idx, message.payload, message.FIN );
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  // debug( "unimplemented send() called" );
  TCPReceiverMessage msg {};
  const uint64_t capacity = reassembler_.writer().available_capacity();
  msg.window_size = ( capacity > UINT16_MAX ) ? UINT16_MAX : static_cast<uint16_t>( capacity );
  if ( isn_.has_value() ) {
    const uint64_t abs_seqno
      = reassembler_.writer().bytes_pushed() + 1 + ( reassembler_.writer().is_closed() ? 1 : 0 );
    msg.ackno = Wrap32::wrap( abs_seqno, *isn_ );
  }
  if ( reassembler_.writer().has_error() ) {
    msg.RST = true;
  }
  return msg;
}
