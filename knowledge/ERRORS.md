---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-06T10:23:03-06:00"
---
# Errors —2026-11-06
## Invalid fixture calendar accepted by max-only validation
Initial wscript checked only upperbounds, allowing year0/month0/day0 orFebruary31 into compiled fixture defaults. Freshreview flagged possible invalidmonth indexing before inbox validation ran. Fixed with combined datetime.date validation and explicit1900..2099year range before SDKbuild. Negative tests cover zeros,Feb31,1900Feb29,year2100;2000Feb29is accepted.
## Strict compiler indentation and bounded strings
The SDK rejects a same-line if followed by unrelated calls as misleading indentation. Split the statements and sized date/footer buffers for compiler-known maximuminteger formatting. Rebuilt successfully.

## 2026-11-06 — phone settings/weather outbox contention
A settings send immediately followed by cached weather can collide with PebbleKit's single outstanding message. Use the shared ACK-driven transport queue; delayed-ACK and bounded-retry tests verify ordering. Graduated to domain/complications.md.

## 2026-11-06 — weather fixture missing vs stale
Fixture state0 must reset the weather timestamp, otherwise a prior stale fixture can make the missing-data capture say STALE. Reset state0 to timestamp0 and state2 to timestamp1; native capture compares the two condition strips. Production WeatherTime0 already clears correctly.

## 2026-11-06 — rounding near freezing
Converting tenths Celsius to tenths Fahrenheit with integer division before rounding can produce a one-degree error (−0.3°C became32°F). Round the rational `(9*c10+1600)/50` directly, using signed half-divisor rounding. Compiled regressions verify −0.3°C and −0.8°C both display31°F. Graduated to domain/complications.md.

## 2026-11-06 — rectangular text boxes inside a circular dial
A rectangular bounding box can fit the layer yet let footer glyphs touch a circular border. The NO DATA native mask reached radius30.4 despite its58px box. Inset header/footer boxes and measure rendered ink radially; corrected layouts keep ink within radius29. See domain/complications.md and execution/capture_dials.py.

## 2026-11-06 — Native round subdial clearance
Error: a width-constrained bold CLOUD footer truncated on Chalk; seconds40/20 labels had only0.45px inner-ring clearance, and STORM had1.92px. Outer-screen circle checks did not detect this inner complication boundary.
Cause: system font advances and visible glyph ink differ from nominal text boxes; the subdial has its own smaller safe circle.
Fix: regular small footer font, one-pixel footer lift, seconds labels inset3px horizontally and2px upward. `execution/check_round_dials.py` measures isolated native ink against the actual ring's inner edge;20Chalkcases pass withminimum2.4px.
Graduated-to: procedural/round_compatibility.md and execution/check_round_dials.py.

## 2026-11-06 — Chalk emulator panic during round rim drawing
Error: the PBL fixture produced the firmware sad-face and unresponsive watch-info; other round faces booted.
Fix observed: precomputed soft-octagon vertices, static working point arrays, and integer math replaced the runtime outline calculations. Default/full native Chalk runs subsequently passed. The runtime hypot/stack path was suspected but not isolated as a proven root cause; do not attribute the failure to Health without evidence.
Graduated-to: round_analog.h and procedural/round_compatibility.md.
