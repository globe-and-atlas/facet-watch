---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-06T10:23:03-06:00"
---
# Build and native QA
- `make test`, `make build`, `make verify` from project root.
- Root owns the shared emulator. Initial fixture:10:08:30,2026-11-06,82percent.
- Fixture keys11000hour11001minute11002second11004battery11005year11006month11007day; production compiles these out.
- `execution/capture_native.py --monitor PORT` samples minute/hour luminous axes, hub, palette background and footerpresence, then saves nativecases/contact sheets.
- Source-level/pixel checks need separate semantic visual inspection for calendar order, small labels, complication crossings and dial resemblance.
- Root verifies second-frame pixels outside the subdial remain unchanged and main hands remain above the seconds layer at crossings.
- After native fixtures, rebuild clean production and archive exactverifiedPBW. Physical contrast remains a separate check.

##2026-11-06 — Live proof
- On production, controlled10:08time allowed pixel comparisons acrossseconds whileminutehandsstayfixed.
- Blank subdialRect16,78,72,72 forcomparison; outsidepixelsmustmatch acrossframes.
- Also sample hourlume insidesubdialrectangle toensuretransparenttophandlayer survives everysecondsredraw.
- TurnsecondsOffandverifywholeframeunchangedwithinminute; restoreactualemulatortimeafterproof.
