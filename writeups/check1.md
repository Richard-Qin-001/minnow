Checkpoint 1 Writeup
====================

My name: [Richard Qin]

My SUNet ID: []

I collaborated with: []

I would like to thank/reward these classmates for their help: []

This lab took me about [3] hours to do. I [did not] attend the lab session.

I was surprised by or edified to learn that: [how to use iterator and std::move()]

Report from the hands-on component of the lab checkpoint: [include
information from 2.1(4), and report on your experience in 2.2]

Describe Reassembler structure and design.
* **The member variable of the Reassembler:**
    * `ByteStream output_`
    * `uint64_t first_unassembled_index_`: the data in buffer whoes `first_index` is less than this varible can be push into `output_`
    * `std::map<uint64_t, std::string> unassembled_strings_`: core container acts as the buffer because its ordered and we can use `.lower_bound()` to quickly identify the elements which may have same parts with data
    * `bool has_last_substring_`: to handle the eof and avoid close unexcepted erly
    * `uint64_t eof_index`: when `first_unassembled_index_` reaches `eof_index`, close the connection
* **Approach:**
    * Handle EOF: process it first to avoid early return
    * Calculate the effective range: The key point is how to deal with overflow. Due to the left and right bound we need to cut off the data and return if the new data is empty.However, if we use add, Test #10 would fail because it's first_index is the MAX uint64_t. So we need to handle it specialized and use sub instead.
    * Insert: The most difficult part of this Lab checkpoint. Firstly, we need to calculate the range of new data(if the process reaches here). Then, I use `.lower_bound()` to find the first index fit the new data. But note that the previous one may have the overlapping part of new data, let the iterator to be the previous one if it is existed. Then we traverse the container map to merge overlapping parts and then put the whole string into the buffer.
    * Push: Start from the first element of map and judge if it is the proper time (the index equals `first_unassembled_index_`) to put it into `output_`. Here I use std::move() to avoid the memory and time overhead caused by extra copying.
    * Close: make sure it is the proper time to close connection.

* **Benefits:**
    * Here are the results:
```Bash
Test project /home/richard/code/minnow/build
      Start  1: compile with bug-checkers
 1/18 Test  #1: compile with bug-checkers ........   Passed    4.66 sec
      Start  3: byte_stream_basics
 2/18 Test  #3: byte_stream_basics ...............   Passed    0.02 sec
      Start  4: byte_stream_capacity
 3/18 Test  #4: byte_stream_capacity .............   Passed    0.02 sec
      Start  5: byte_stream_one_write
 4/18 Test  #5: byte_stream_one_write ............   Passed    0.02 sec
      Start  6: byte_stream_two_writes
 5/18 Test  #6: byte_stream_two_writes ...........   Passed    0.02 sec
      Start  7: byte_stream_many_writes
 6/18 Test  #7: byte_stream_many_writes ..........   Passed    0.14 sec
      Start  8: byte_stream_stress_test
 7/18 Test  #8: byte_stream_stress_test ..........   Passed    0.33 sec
      Start  9: reassembler_single
 8/18 Test  #9: reassembler_single ...............   Passed    0.02 sec
      Start 10: reassembler_cap
 9/18 Test #10: reassembler_cap ..................   Passed    0.03 sec
      Start 11: reassembler_seq
10/18 Test #11: reassembler_seq ..................   Passed    0.04 sec
      Start 12: reassembler_dup
11/18 Test #12: reassembler_dup ..................   Passed    0.08 sec
      Start 13: reassembler_holes
12/18 Test #13: reassembler_holes ................   Passed    0.02 sec
      Start 14: reassembler_overlapping
13/18 Test #14: reassembler_overlapping ..........   Passed    0.03 sec
      Start 15: reassembler_win
14/18 Test #15: reassembler_win ..................   Passed    0.71 sec
      Start 37: no_skip
15/18 Test #37: no_skip ..........................   Passed    0.01 sec
      Start 38: compile with optimization
16/18 Test #38: compile with optimization ........   Passed    1.93 sec
      Start 39: byte_stream_speed_test
        ByteStream throughput (pop length 4096): 16.75 Gbit/s
        ByteStream throughput (pop length 128):  11.90 Gbit/s
        ByteStream throughput (pop length 32):    5.26 Gbit/s
17/18 Test #39: byte_stream_speed_test ...........   Passed    0.27 sec
      Start 40: reassembler_speed_test
        Reassembler throughput (no overlap):  50.71 Gbit/s
        Reassembler throughput (10x overlap): 10.10 Gbit/s
18/18 Test #40: reassembler_speed_test ...........   Passed    0.23 sec

100% tests passed, 0 tests failed out of 18

Total Test time (real) =   8.57 sec
Built target check1
```
Implementation Challenges:
[boundary condition handling and insert logic]

Remaining Bugs:
[]

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [boundary condition handling]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]
