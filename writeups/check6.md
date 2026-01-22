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
To optimize the routing lookup performance from O(N) to O(1) (specifically O(32)),
I implemented a Trie (Prefix Tree) data structure instead of a simple linear
list.

The router maintains a `root_` pointer to a `TrieNode`. Each `TrieNode` contains
an array of two `std::unique_ptr`s (representing bit 0 and bit 1) and an
`std::optional<RouteEntry>`. The `RouteEntry` is stored directly in the node
corresponding to the end of a prefix.

- **Insertion (`add_route`)**: Traverses the tree based on the bits of the
  route prefix, creating new nodes as necessary using `std::make_unique`.
- **Lookup (`match_longest_prefix`)**: Traverses the tree using the destination
  IP's bits. It keeps track of the most recently seen valid `RouteEntry` as it
  descends. This naturally implements "Longest Prefix Match" logic without
  needing manual length comparisons or sorting.


Implementation Challenges:
-------------------------
Transitioning to a Trie structure introduced a strict compiler warning regarding
uninitialized members (`-Weffc++`). The compiler flagged that `std::array` and
`std::optional` in my `TrieNode` struct were not explicitly initialized, even
though they have default constructors. I resolved this by using default member
initializers (e.g., `children {}`), ensuring all members are zero-initialized.

Another challenge was correctly implementing the bitwise traversal. I had to
ensure I was extracting the bits from MSB to LSB (`(ip >> (31-i)) & 1`). Using
a Trie also elegantly solved the "undefined behavior" issue regarding bitwise
shifts (shifting by 32) that I encountered in the linear scan approach, as the
Trie logic relies on bit indexing rather than masking.

Remaining Bugs:
--------------
No known bugs. The Trie implementation correctly handles all edge cases,
including default routes (/0) and nested subnets, and passes all tests.

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
