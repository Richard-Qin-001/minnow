Checkpoint 0 Writeup
====================

My name: [Richard Qin]

My SUNet ID: [richard]

I collaborated with: [None]

I would like to credit/thank these classmates for their help: [None]

This lab took me about [n] hours to do. I [did not] attend the lab session.

My secret code from section 2.1 was: [455392]

I was surprised by or edified to learn that: [I learned how to use queue]

Describe ByteStream implementation. 
* **Implementation:** I implemented the internal buffer using a std::queue<std::string> to store the pushed data chunks. To track the stream's state, I maintained a buffer_size_ counter and a buffer_view_offset_ variable.

* **Approach:**
    * Push: Incoming strings are simply pushed into the queue. If the string is too long for the remaining capacity, it is truncated before pushing. This operation is $O(1)$ (or linear to the size of the chunk being copied, but no existing data is moved).
    * Peek: I return a std::string_view of the front element of the queue, starting from buffer_view_offset_. This avoids allocating new memory or copying data for peeking.
    * Pop: I update buffer_view_offset_ and reduce buffer_size_. If the offset reaches the end of the front string, I pop the string from the queue and reset the offset to 0.
* **Benefits:**
    * Performance: The design minimizes memory copying. Unlike a single circular buffer (which might require memmove or complex wrap-around logic) or a single large string (which requires reallocation), the queue approach handles data chunks as they arrive.
    * Efficiency: Popping is highly efficient because it primarily involves arithmetic operations (incrementing the offset) and only deallocates memory when a full chunk is consumed.
    * Measurements: My implementation achieved a throughput of 17.35 Gbit/s in the speed test.


Implementation Challenges:
[Handling the partial pop logic efficiently without erasing from the string front]

Remaining Bugs:
[]

- If applicable: I received help from a former student in this class,
  another expert, or a chatbot or other AI system (e.g. ChatGPT,
  Gemini, Claude, etc.), with the following questions or prompts:
  [please list questions/prompts]

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I contributed a new test case that catches a plausible bug
  not otherwise caught: [provide Pull Request URL]

