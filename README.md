---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-06T16:52:31-06:00"
---
# PBL-2500

Native Pebble Time 2 analog watchface with a soft octagonal rim, faceted markers, luminous-style hands, and a three-cell fan calendar.

## Left dial — local version 0.2.0

- **Seconds** remains the default and preserves the published design.
- **Weather** shows current temperature and a condition label. Choose Fahrenheit or Celsius.
- **Battery** shows the actual charge percentage and a fill bar.
- **Steps** shows today's Pebble Health total. Values above 9,999 use a compact `k` format. Unavailable data displays `--`, distinct from a genuine zero.

The left dial can be hidden. Only visible Seconds mode subscribes to second ticks; the other choices use minute ticks and relevant battery/health events. The separate bottom battery percentage can be hidden independently.

Weather requires the paired phone, Internet access, and location permission. Only a selected, visible Weather dial requests location. Coordinates are rounded to two decimal places before transmission; coordinates are not stored. Current data refreshes about every 30 minutes. Failures use backoff and retain valid cached readings. Readings older than two hours are hidden. Weather data by [Open-Meteo](https://open-meteo.com/), [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/).

## Existing choices

Six palettes (Graphite, Steel, Olive, Ivory, Navy, Gold), custom colors, date Off/fan/ISO/US/EU, and optional watch-default/12-hour/24-hour digital footer. The analog dial remains twelve-hour.

```sh
make test
make build
make verify
```

Prior Emery preview: `dist/PBL-2500-0.2.0.pbw`. Native four-mode comparison: `media/pbl-2500-left-dials.png`. Fixture tests require a parent-owned emulator; `python3 execution/capture_dials.py --monitor PORT` captures 38 mode/palette/edge cases with measured text-to-ring clearance.

## Public release

[Store listing](https://apps.repebble.com/3403f49eadb7469d94ae292d): **0.1.1**, published and publicly verified. **0.2.0 is local and unpublished.** Physical phone/watch acceptance remains separate from native fixtures and mocked phone tests.


## Round-compatible release — 0.2.1

PBL-2500 includes native layouts for Pebble Time2 (Emery200×228), original Time Round (Chalk180×180), and Round2 (Gabbro260×260). The rectangular layout and existing settings identities are preserved.

Build: `python3 execution/build.py`, then `python3 execution/verify.py`. Local bundle: `dist/PBL-2500-0.2.1.pbw`. Native round previews: `media/round/`. Repeatable fixture/capture instructions: [round compatibility QA](knowledge/procedural/round_compatibility.md).

This update is [publicly available](https://apps.repebble.com/3403f49eadb7469d94ae292d) with round screenshots. Public PBW bytes and screenshot pixels verify; physical-watch validation remains separate.
