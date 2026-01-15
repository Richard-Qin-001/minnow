#include "byte_stream.hh"
#include "debug.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

// Push data to stream, but only as much as available capacity allows.
void Writer::push( string data )
{
  // Your code here (and in each method below)
  if ( is_closed() || data.empty() ) {
    return;
  }
  const uint64_t available = available_capacity();

  if ( data.size() > available ) {
    data.resize( available );
  }

  if ( data.empty() ) {
    return;
  }

  buffer_size_ += data.size();
  total_pushed_ += data.size();

  buffer_.push( std::move( data ) );

  // debug( "Writer::push({}) not yet implemented", data );
}

// Signal that the stream has reached its ending. Nothing more will be written.
void Writer::close()
{
  closed_ = true;
  // debug( "Writer::close() not yet implemented" );
}

// Has the stream been closed?
bool Writer::is_closed() const
{
  // debug( "Writer::is_closed() not yet implemented" );
  return closed_; // Your code here.
}

// How many bytes can be pushed to the stream right now?
uint64_t Writer::available_capacity() const
{
  // debug( "Writer::available_capacity() not yet implemented" );
  return ( capacity_ - buffer_size_ ); // Your code here.
}

// Total number of bytes cumulatively pushed to the stream
uint64_t Writer::bytes_pushed() const
{
  // debug( "Writer::bytes_pushed() not yet implemented" );
  return ( total_pushed_ ); // Your code here.
}

// Peek at the next bytes in the buffer -- ideally as many as possible.
// It's not required to return a string_view of the *whole* buffer, but
// if the peeked string_view is only one byte at a time, it will probably force
// the caller to do a lot of extra work.
string_view Reader::peek() const
{
  // debug( "Reader::peek() not yet implemented" );
  return { string_view( buffer_.front() ).substr( buffer_view_offset_ ) }; // Your code here.
}

// Remove `len` bytes from the buffer.
void Reader::pop( uint64_t len )
{
  uint64_t pop_len = min( len, buffer_size_ );

  total_popped_ += pop_len;
  buffer_size_ -= pop_len;

  while ( pop_len > 0 ) {
    const uint64_t front_size = buffer_.front().size();
    const uint64_t remaining_in_front = front_size - buffer_view_offset_;

    if ( pop_len >= remaining_in_front ) {
      pop_len -= remaining_in_front;
      buffer_.pop();
      buffer_view_offset_ = 0;
    } else {
      buffer_view_offset_ += pop_len;
      pop_len = 0;
    }
  }
  // debug( "Reader::pop({}) not yet implemented", len );
}

// Is the stream finished (closed and fully popped)?
bool Reader::is_finished() const
{
  // debug( "Reader::is_finished() not yet implemented" );
  return ( closed_ && ( buffer_size_ == 0 ) ); // Your code here.
}

// Number of bytes currently buffered (pushed and not popped)
uint64_t Reader::bytes_buffered() const
{
  // debug( "Reader::bytes_buffered() not yet implemented" );
  return ( buffer_size_ ); // Your code here.
}

// Total number of bytes cumulatively popped from stream
uint64_t Reader::bytes_popped() const
{
  // debug( "Reader::bytes_popped() not yet implemented" );
  return total_popped_; // Your code here.
}
