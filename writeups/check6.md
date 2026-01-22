Checkpoint 6 Writeup
====================

My name: [Richard Qin]

My SUNet ID: [your sunetid here]

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about [3] hours to do. I [did not] attend the lab session.

Program Structure and Design of the Router [Describe data
structures and approach taken. Describe alternative designs considered
or tested.  Describe benefits and weaknesses of your design compared
with alternatives -- perhaps in terms of simplicity/complexity, risk
of bugs, asymptotic performance, empirical performance, required
implementation time and difficulty, and other factors. Include any
measurements if applicable.]:
----------------------------
My router implementation uses a simple `std::vector` to store the routing table.
Each `RouteEntry` struct holds the route prefix, prefix length, optional next
hop, and the output interface index. While a Trie structure would be more
efficient for large-scale routing, a linear scan (O(N)) is sufficient and
performant enough for this lab's constraints.

To handle the "Longest Prefix Match" (LPM) logic and keep the code clean, I
abstracted the matching process into a private helper function,
`match_longest_prefix`. This function iterates through the routing table,
calculates the bitmask for each entry, and returns the matching rule with the
greatest prefix length. The main `route()` method handles the high-level logic:
polling interfaces, checking TTL, updating the checksum, and forwarding the
datagram.


Implementation Challenges:
-------------------------
The most tricky part was calculating the bitmask for the default route (/0).
Shifting a 32-bit integer by 32 bits is undefined behavior in C++, which caused
my mask calculation logic (`0xFFFFFFFF << (32 - len)`) to fail for the default
route. I resolved this by adding a specific check for `prefix_length == 0`.

Another challenge was "bad IPv4 datagram" errors. I initially forgot that
modifying the TTL field invalidates the IP header's checksum. I fixed this by
calling `compute_checksum()` immediately after decrementing the TTL. Finally, I
encountered a `cognitive complexity` error from the linter due to deep nesting;
extracting the matching logic into a helper function solved this neatly.

Remaining Bugs:
--------------
No known bugs. The router successfully handles direct delivery, next-hop
forwarding, and default routes. It passes all automated tests and linter checks.

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
