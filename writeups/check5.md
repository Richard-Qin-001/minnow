Checkpoint 5 Writeup
====================

My name: [Richard Qin]

My SUNet ID: [your sunetid here]

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about [3] hours to do. I [did not] attend the lab session.

Program Structure and Design of the NetworkInterface [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]:
----------------------------
My implementation of the `NetworkInterface` relies on three primary data
structures, all utilizing `std::unordered_map` for O(1) lookup efficiency:

1.  `arp_table_`: Maps target IP addresses (uint32_t) to a custom `ArpEntry`
    struct containing the EthernetAddress and an expiration timestamp. This
    serves as the ARP cache (Soft State).
2.  `waiting_datagrams_`: Maps a next-hop IP address to a `std::vector` of
    queued `InternetDatagram`s. This buffers packets while waiting for ARP.
3.  `arp_request_timestamps_`: Tracks the last time an ARP request was broadcast
    for a specific IP to enforce the 5-second rate limit.

A key design choice was "Aggressive Passive Learning." The interface updates its
ARP cache upon receiving *any* ARP frame (Request or Reply) that contains a
sender IP/MAC pair. This maximizes the cache hit rate and minimizes redundant
ARP traffic. The `tick` method acts as the garbage collector, removing expired
ARP entries based on the absolute `current_time_`.

Implementation Challenges:
-------------------------
The most significant challenge was managing the lifecycle of iterators within the
`tick` function. When removing expired entries from the `arp_table_`, I
initially encountered memory errors (heap-use-after-free) because `erase()`
invalidates the current iterator. I resolved this by correctly utilizing the
iterator returned by `erase()` to safely continue the loop.

Another logical challenge was ensuring the "Unblock" mechanism worked correctly.
It was crucial to iterate through *all* queued datagrams for a specific IP
immediately after learning its MAC address, rather than just sending the first
one. I also ensured that pending packets are dropped if the ARP request times
out (implied by the 5-second cooldown logic and general timeout handling),
preventing infinite buffering.

Remaining Bugs:
--------------
No known bugs. The implementation passes all `check5` test cases, including
corner cases for ARP expiration, rate limiting, and queue draining.


- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
