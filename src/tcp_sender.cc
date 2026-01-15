#include "tcp_sender.hh"
#include "debug.hh"
#include "tcp_config.hh"

using namespace std;

// How many sequence numbers are outstanding?
uint64_t TCPSender::sequence_numbers_in_flight() const
{
  // debug( "unimplemented sequence_numbers_in_flight() called" );
  return sequence_numbers_in_flight_;
}

// How many consecutive retransmissions have happened?
uint64_t TCPSender::consecutive_retransmissions() const
{
  // debug( "unimplemented consecutive_retransmissions() called" );
  return timer_.consecutive_retransmissions();
}

void TCPSender::push( const TransmitFunction& transmit )
{
  // debug( "unimplemented push() called" );
  // (void)transmit;
  const uint64_t current_window_size = ( window_size_ == 0 ? 1 : window_size_ );
  while ( sequence_numbers_in_flight_ < current_window_size ) {
    TCPSenderMessage message;
    message.seqno = Wrap32::wrap( next_seqno_, isn_ );
    if ( next_seqno_ == 0 ) {
      message.SYN = true;
    }
    const uint64_t remaining_window = current_window_size - sequence_numbers_in_flight_ - ( message.SYN ? 1 : 0 );
    const size_t playload_limit = std::min( remaining_window, TCPConfig::MAX_PAYLOAD_SIZE );
    read( reader(), playload_limit, message.payload );
    if ( !fin_sent_ && reader().is_finished() && message.payload.size() < remaining_window ) {
      message.FIN = true;
      fin_sent_ = true;
    }
    if ( reader().has_error() ) {
      message.RST = true;
    }
    if ( message.sequence_length() == 0 && !message.RST ) {
      break;
    }
    transmit( message );
    next_seqno_ += message.sequence_length();
    sequence_numbers_in_flight_ += message.sequence_length();
    outstanding_segments_.push_back( message );
    if ( !timer_.is_running() ) {
      timer_.start();
    }
    if ( message.RST ) {
      break;
    }
  }
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  // debug( "unimplemented make_empty_message() called" );
  TCPSenderMessage message;
  message.seqno = Wrap32::wrap( next_seqno_, isn_ );
  if ( reader().has_error() ) {
    message.RST = true;
  }
  return message;
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  // debug( "unimplemented receive() called" );
  // (void)msg;
  if ( msg.RST ) {
    reader().set_error();
    return;
  }
  window_size_ = msg.window_size;
  if ( msg.ackno.has_value() ) {
    const uint64_t abs_ackno = msg.ackno.value().unwrap( isn_, next_seqno_ );
    if ( abs_ackno > next_seqno_ ) {
      return;
    }
    bool new_data_acked = false;
    while ( !outstanding_segments_.empty() ) {
      const auto& front_msg = outstanding_segments_.front();
      const uint64_t seg_abs_seqno = front_msg.seqno.unwrap( isn_, next_seqno_ );
      const uint64_t seg_length = front_msg.sequence_length();
      if ( seg_abs_seqno + seg_length <= abs_ackno ) {
        sequence_numbers_in_flight_ -= seg_length;
        outstanding_segments_.pop_front();
        new_data_acked = true;
      } else {
        break;
      }
    }
    if ( new_data_acked ) {
      timer_.reset_rto();
      if ( !outstanding_segments_.empty() ) {
        timer_.start();
      } else {
        timer_.stop();
      }
    }
    if ( outstanding_segments_.empty() ) {
      timer_.stop();
    }
  }
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  // debug( "unimplemented tick({}, ...) called", ms_since_last_tick );
  // (void)transmit;
  timer_.tick( ms_since_last_tick );
  if ( timer_.is_expired() ) {
    transmit( outstanding_segments_.front() );
    if ( window_size_ > 0 ) {
      timer_.on_expiration_backoff();

    } else {
      timer_.on_expiration_reset_time();
    }
  }
}
// Status Inquiry
bool RetransmissionTimer::is_running() const
{
  return running_;
}
uint64_t RetransmissionTimer::consecutive_retransmissions() const
{
  return consecutive_retransmissions_;
}

// action
void RetransmissionTimer::start()
{
  running_ = true;
  elapsed_ms_ = 0;
}

void RetransmissionTimer::stop()
{
  running_ = false;
  elapsed_ms_ = 0;
}

// The passage of time
void RetransmissionTimer::tick( uint64_t ms )
{
  if ( running_ ) {
    elapsed_ms_ += ms;
  }
}

// Check if it has timed out
bool RetransmissionTimer::is_expired() const
{
  return running_ && ( elapsed_ms_ >= current_RTO_ms_ );
}

// Handle retransmission
void RetransmissionTimer::on_expiration_backoff()
{
  consecutive_retransmissions_++;
  current_RTO_ms_ *= 2;
  elapsed_ms_ = 0;
}

void RetransmissionTimer::on_expiration_reset_time()
{
  elapsed_ms_ = 0;
}

// Reset upon receiving new data confirmation
void RetransmissionTimer::reset_rto()
{
  current_RTO_ms_ = initial_RTO_ms_;
  consecutive_retransmissions_ = 0;
  elapsed_ms_ = 0;
}