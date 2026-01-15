#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  // debug( "unimplemented wrap( {}, {} ) called", n, zero_point.raw_value_ );
  return Wrap32 { zero_point + static_cast<uint32_t>( n ) };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  // debug( "unimplemented unwrap( {}, {} ) called", zero_point.raw_value_, checkpoint );
  const Wrap32 ckpt_wrapped = Wrap32::wrap( checkpoint, zero_point );
  const auto diff = static_cast<int32_t>( this->raw_value_ - ckpt_wrapped.raw_value_ );
  const int64_t res = static_cast<int64_t>( checkpoint ) + diff;
  return ( ( res < 0 ) ? static_cast<uint64_t>( res + ( 1ULL << 32 ) ) : static_cast<uint64_t>( res ) );
}
