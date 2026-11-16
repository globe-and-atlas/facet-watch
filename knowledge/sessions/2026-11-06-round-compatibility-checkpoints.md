---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-06T16:55:26-06:00"
---
# PBL-2500 — selectable left dial

Goal: preserve default Seconds; add Weather, Battery percentage, and today's Steps.
Agent: OpenAI Codex
Handoff-from: OpenAI Codex
Handoff-type: continuation
Status: Complete locally; version 0.2.0 unpublished

## Result
- Settings offer Seconds (default), Weather, Battery percentage, and Today's steps. Existing settings migrate with Seconds selected; legacy hidden-dial preference is preserved.
- Weather offers °F/°C, current condition labels, an opt-in phone-location request, coordinate rounding, cached data, failure backoff, and two-hour expiry. Missing/stale data never masquerades as current weather.
- Battery reads the battery service. Steps reads today's Pebble Health count and distinguishes unavailable from zero. Non-seconds modes avoid second ticks.
- The original soft-octagon design and default Seconds render are unchanged. The bottom battery readout can be hidden separately.

## Evidence
- `make test`: three JavaScript suites and eight pytest tests pass. Actual Clay Save/Cancel/reload and Weather→GPS→HTTP→queued-message path exercised with test adapters.
- 38 native cases cover four modes, six palettes, both temperature units, missing/stale conditions, hidden dial, zero/large steps, and temperature extremes.
- Native choice/units/palette/visibility persist across reinstall.
- Seconds retains the original design; the 40/20 labels are now inset slightly to improve circle clearance.
- Live Open-Meteo schema probe passed at fixed public Berlin test coordinates; no user location used.
- Clean production 0.2.0 bundle verifies; production emulator seconds advance, battery dial remains static, health mode renders, defaults restored.
- Fresh independent creator-verifier accepted source, tests, native images, and final rational temperature conversion.

## Artifacts and next step
- `dist/PBL-2500-0.2.0.pbw`
- `media/pbl-2500-left-dials.png` (native comparison, sample data)
- `execution/capture_dials.py --monitor PORT`
- Public store remains verified 0.1.1. No upload was requested for this feature. Publish only on user instruction; refresh media before doing so.
- Prior checkpoints archived under sessions/2026-11-06-release-and-left-dial-checkpoints.md.

S.D.G.: Gives PBL-2500 wearers a useful choice of weather, charge, or daily activity without losing the original seconds dial.
Feynman gap: Real phone location permissions and physical-watch Health readings are unverified; native fixtures, phone adapter tests, live provider schema, and production emulator checks passed.

## Text-clearance polish
User requested no text/circle overlap. Moved headers down, footers up, constrained value widths, and inset seconds40/20 labels. Added fixture-only isolated text masks so native rendered ink is measured radially against the border. Native clearance checks running; local0.2.0 remains unpublished.

### Text-clearance verification complete
38 native cases pass with measured minimum3px text/bar-to-ring clearance, including every condition label and NO DATA. Native comparison refreshed. Clean production0.2.0 bundle verified. Store0.1.1 unchanged.

## Active — round compatibility
Goal: add original Time Round/chalk180×180 and Round2/gabbro260×260 without altering Emery layout or settings identity. Agent:OpenAI Codex. Handoff-type:continuation. Status:Chalk native matrix passed; Gabbro native matrix running; final clean production builds pending. Batch checkpoint: /Users/danielbally/Git/.tmp/round-watchfaces/runlog.md. No publication requested.
