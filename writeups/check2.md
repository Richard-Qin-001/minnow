Checkpoint 2 Writeup
====================

My name: [Richard Qin]

My SUNet ID: [your sunetid here]

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [list sunetids here]

This lab took me about [3] hours to do. I [did not] attend the lab session.

Describe Wrap32 and TCPReceiver structure and design. [
1. WrappingIntegers
* Design idea: By leveraging the characteristics of C++ unsigned integer overflow and two's complement arithmetic, an efficient sequence number conversion is implemented.

* `wrap`: Directly perform the addition `zero_point + n`, using the automatic overflow behavior of uint32_t to simulate modulo $2^{32}$ operations. 

* `unwrap`: Calculate `offset = raw_value - wrap(checkpoint)` and cast it to `int32_t`. This directly yields the shortest signed distance between the checkpoint and the target value under this modulus. The absolute sequence number is then restored via `checkpoint + diff`, handling boundary cases where the result is negative by adding back $2^{32}$.

2. TCPReceiver
* State Management: Use `std::optional<Wrap32>` isn_ to track whether a SYN packet has been received. This is the only additional state member.

* Data Reception (`receive`):

  * ISN Initialization: Initialize `isn_` when the first SYN packet is received.

  * Checkpoint Calculation: Use `reassembler_.writer().bytes_pushed() + 1` as the checkpoint to ensure that unwrapping is always based on the current receive progress.

  * Index Conversion: After converting the TCP sequence number to an absolute sequence number, accurately calculate the `stream_index` needed by the Reassembler based on whether the SYN flag is present (`abs_seqno - 1`, handling the SYN offset).

  * RST Handling: Added a check for the `RST` flag; if set, it sets the ByteStream to an error state.

* Feedback Sending (send):

  * Ackno: Calculated as `bytes_pushed + 1 (SYN) + (stream_ended ? 1 : 0)`.

  * Window Size: Calculated as `capacity - bytes_buffered`, and forcibly truncated to `uint16_t` (maximum 65535) to comply with TCP header standards.
]

Implementation Challenges:
[
* A. Performance Pitfall: Debug Output Causing Timeouts
During the `wrapping_integers_roundtrip` test, a severe timeout issue was encountered.

  * Cause: The test script executed 250,000 loop calls, and the code retained the debug() print statements from the Starter Code. This resulted in the program attempting millions of I/O operations, significantly slowing down the CPU.

  * Solution: By removing all unnecessary `debug` output, the test execution time instantly dropped from timeout levels to milliseconds. This clearly demonstrates the importance of avoiding I/O operations in high-performance paths (Hot Path).

* B. Strict Clang-Tidy Standards
To meet the requirements of static code analysis, several refactorings were made:

  * Cognitive Complexity: The original logic of `Reassembler::insert` was too complex (score 27/25). By extracting a private helper function `flush()`, the logic of "writing to ByteStream" and "handling EOF" was decoupled, reducing function complexity and improving readability.

  * String Concatenation Performance: Changed `data = sub + data` to `data.insert(0, sub)` to avoid creating and copying temporary string objects.

  * Type Safety: Corrected `const` correctness.
]

Remaining Bugs:
[]

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: [submit as GitHub PR
  and include URL here]
