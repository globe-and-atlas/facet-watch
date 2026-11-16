---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-06T10:23:03-06:00"
---
# Build watchface
1. `node watchface/settings.test.js`: actual ClaySave/Cancel/reload and normalized toggle choices.
2. `python3 -m pytest tests/ -q`: host calendar/angle/color policy plus fixture-date validation.
3. `python3 execution/build.py`: clean production build stripping FACET_*environment flags.
4. `python3 execution/verify.py`: PBW identity/Emery target and production binary match.
Fixtures: root only, `pebble clean`, then `FACET_FIXTURE=1 pebble build` from watchface. No delegated emulator use.
`execution/capture_native.py --monitor PORT` captures31nativecases and palette/contact previews.
Never publish fixtures. No store publication authorized in this build task.
