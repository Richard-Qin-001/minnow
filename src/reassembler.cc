#include "reassembler.hh"
#include "debug.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
    // EOF
    if(is_last_substring)
    {
        has_last_substring_ = true;
        eof_index = first_index + data.size();
    }
    // current data bound
    uint64_t left_bound_ = first_unassembled_index_;
    uint64_t capacity = output_.writer().available_capacity();
    uint64_t right_bound_ = first_unassembled_index_ + capacity;
    // cut off the overflow parts
    if ( first_index < left_bound_ )
    {
        uint64_t offset = left_bound_ - first_index;
        if (offset >= data.size())
            return;
        data = data.substr(offset);
        first_index = left_bound_;
    }
    if (first_index > right_bound_)
        return;
    if(data.size() > right_bound_ - first_index)
    {
        data = data.substr(0, right_bound_ - first_index);
        if ( data.empty() )
          return;
    }
    // Insert
    // Range of new substring
    // Start index
    // first_index;
    // End index
    uint64_t end_index = first_index + data.size();
    // Find potentially overlapping intervals
    // Using map lower_bound
    auto it = unassembled_strings_.lower_bound(first_index);
    // if not the first element. May have same parts of the previous one
    if(it != unassembled_strings_.begin())
    {
        auto prev_it = prev(it);
        // If the end position of the previous node > the start position of the new node, it indicates an overlap
        if ( prev_it->first + prev_it->second.size() >= first_index )
        {
          // Start processing from the previous node
          it = prev_it;
        }
    }
    // Start scanning backward and merging
    while (it != unassembled_strings_.end() && it->first <= end_index)
    {
      uint64_t cur_end = it->first + it->second.size();
      uint64_t cur_first = it->first;
      // If the current fragment is completely contained by the new fragment, delete it directly.
      if(cur_first >= first_index && cur_end < end_index)
      {
        it = unassembled_strings_.erase(it);
        continue;
      }
      // If the new fragment is completely contained within the current fragment, there's no need to insert it at
      // all; just return.
      if(cur_first < first_index && cur_end > end_index)
      {
        return;
      }
      // Handling left overflow: Current fragment starts earlier
      if(cur_first < first_index)
      {
        data = it->second.substr(0, first_index - cur_first) + data;
        first_index = cur_first;
      }
      // Handling right overflow: Current fragment ends later
      if(cur_end > end_index)
      {
        data = data + it->second.substr(end_index - cur_first);
        end_index = cur_end;
      }
      // erase the old one
      it = unassembled_strings_.erase(it);
    }
    unassembled_strings_[first_index] = data;

    // Pushing
    while ( !unassembled_strings_.empty() && unassembled_strings_.begin()->first == first_unassembled_index_ )
    {
      auto node = unassembled_strings_.begin();
      uint64_t sz = node->second.size();
      output_.writer().push( std::move(node->second) );
      first_unassembled_index_ += sz;
      unassembled_strings_.erase( node );
    }

    if(has_last_substring_ && first_unassembled_index_ == eof_index)
        output_.writer().close();

    debug( "unimplemented insert({}, {}, {}) called", first_index, data, is_last_substring );
}

// How many bytes are stored in the Reassembler itself?
// This function is for testing only; don't add extra state to support it.
uint64_t Reassembler::count_bytes_pending() const
{
  debug( "unimplemented count_bytes_pending() called" );
  uint64_t pending_num = 0;
  for ( const auto& [index, data] : unassembled_strings_ ) {
    pending_num += data.size();
  }
  return {pending_num};
}
