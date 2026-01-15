#pragma once

#include "byte_stream.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"

#include <functional>

class RetransmissionTimer
{
public:
  explicit RetransmissionTimer( uint64_t initial_rto )
    : initial_RTO_ms_( initial_rto ), current_RTO_ms_( initial_rto )
  {}
  // Status Inquiry
  bool is_running() const;
  uint64_t consecutive_retransmissions() const;
  // action
  void start();
  void stop();
  // The passage of time
  void tick( uint64_t ms );
  // Check if it has timed out
  bool is_expired() const;
  // Handle retransmission
  void on_expiration_backoff();
  void on_expiration_reset_time();
  // Reset upon receiving new data confirmation
  void reset_rto();

private:
  uint64_t initial_RTO_ms_;
  uint64_t current_RTO_ms_;
  uint64_t elapsed_ms_ { 0 };
  uint64_t consecutive_retransmissions_ { 0 };
  bool running_ { false };
};

class TCPSender
{
public:
  /* Construct TCP sender with given default Retransmission Timeout and possible ISN */
  TCPSender( ByteStream&& input, Wrap32 isn, uint64_t initial_RTO_ms )
    : input_( std::move( input ) ), isn_( isn ), initial_RTO_ms_( initial_RTO_ms ), timer_( initial_RTO_ms )
  {}

  /* Generate an empty TCPSenderMessage */
  TCPSenderMessage make_empty_message() const;

  /* Receive and process a TCPReceiverMessage from the peer's receiver */
  void receive( const TCPReceiverMessage& msg );

  /* Type of the `transmit` function that the push and tick methods can use to send messages */
  using TransmitFunction = std::function<void( const TCPSenderMessage& )>;

  /* Push bytes from the outbound stream */
  void push( const TransmitFunction& transmit );

  /* Time has passed by the given # of milliseconds since the last time the tick() method was called */
  void tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit );

  // Accessors
  uint64_t sequence_numbers_in_flight() const;  // How many sequence numbers are outstanding?
  uint64_t consecutive_retransmissions() const; // How many consecutive retransmissions have happened?
  const Writer& writer() const { return input_.writer(); }
  const Reader& reader() const { return input_.reader(); }
  Writer& writer() { return input_.writer(); }

private:
  Reader& reader() { return input_.reader(); }

  ByteStream input_;
  Wrap32 isn_;
  uint64_t initial_RTO_ms_;

  // timer
  RetransmissionTimer timer_;
  // uint64_t current_RTO_ms_ {0};                    // Currently effective RTO
  // bool timer_running_ { false };               // Timer Switch
  // uint64_t timer_elapsed_ { 0 };               // Timer cumulative time
  // uint64_t consecutive_retransmissions_ { 0 }; // Consecutive retransmission count

  // window
  uint64_t window_size_ { 1 }; // Receiver window size

  // track
  uint64_t next_seqno_ { 0 };                            // Next absolute sequence number
  uint64_t sequence_numbers_in_flight_ { 0 };            // Bytes in transit
  std::deque<TCPSenderMessage> outstanding_segments_ {}; // Retransmission Queue
  bool fin_sent_ { false };                              // If FIN was sent
};
