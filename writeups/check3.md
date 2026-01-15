Checkpoint 3 Writeup
====================

My name: [Richard Qin]

My SUNet ID: [your sunetid here]

I collaborated with: [list sunetids here]

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about [4] hours to do. I [did not] attend the lab session.

Program Structure and Design of the TCPSender [
* Data Structure: std::deque was chosen to manage outstanding_segments because it supports efficient removal from the front (ACK) and access at the front (retransmission).
* Adopts the design of a separate RetransmissionTimer class, This 'separation of mechanism and strategy' design makes the `tick` function clearer and simplifies state management.
]

Implementation Challenges:
[
* Zero Window Probe
* Conditions for RTO Doubling (No Doubling During Zero Window)
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
