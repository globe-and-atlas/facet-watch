---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-06T10:14:22-06:00"
---
# PBL-2500 native watchface
## Contract
- Source reference: Casio2500 series front dial; PBL-2500 identity with P-SHOCK dial.
- Native Emery200×228; publication authorized by Daniel.
- Execution: build.py,verify.py,capture_native.py.
- Root owns emulator; this implementation does not install.
- No .env access or SDK lockfile commits.
## Validation contract
- [x] Native dial is octagonal.
- [x] Hour hand angle includes the current minute fraction.
- [x] Minute hand shows current minute.
- [x] Small seconds dial shows current second.
- [x] Seconds Off uses minute-only subscription.
- [x] Main hands render above complications.
- [x] Fan date displays tomorrow/current/yesterday.
- [x] Calendar handles leap-year transitions.
- [x] Date Off hides calendar.
- [x] ISO footer fits.
- [x] US footer fits.
- [x] EU footer fits.
- [x] Six palettes render.
- [x] Custom colors render.
- [x] Battery reads current charge.
- [x] Optional digital footer supports12hours.
- [x] Optional digital footer supports24hours.
- [x] Clay Save sends settings.
- [x] Clay Cancel preserves settings.
- [x] Clay reload restores settings.
- [x] Settings persist on watch relaunch.
- [x] Host tests pass.
- [x] Production bundle excludes fixtures.
- [x] Fresh verifier reviews native outputs.
## Master's brief
Primary good: clear analog time with restrained depth.
Stone removed: mimic only dial geometry, not branding or fake durability claims.
Human mark: Daniel judges fidelity on the native screen.
Non Finito: physical contrast and publication remain separate.
Structural weakness: layered analog complications must stay legible at200px.
Myth under test: broad luminous hands and compact subdials preserve hierarchy; native crossings/refutations guide polish.
Misericord: correct month rollover and localized redraws matter as much as appearance.
S.D.G.: Give Pebble owners a clear dimensional analog dial.
Feynman gap: native geometry verified; physical-watch contrast remains untested.

## Verified completion
-31nativefixturecases passed.
-2nativewatch-defaultfooterformatcases passed.
- Settings persist pixel-identically after relaunch.
- Production seconds advance within72×72subdial; pixels outside stay unchanged.
- Main hour lume remains above live seconds redraw.
- SecondsOff remains static within the minute.
- Clean production installed; clock restored to actual time.
- Initial prototype was local; approved public release is recorded below.

## PBL-2500 public release —2026-11-06
Authorization: rename displayNamePBL-2500, dialwordmarkP-SHOCK, publish with paletteGIFfirst.
- [x] Native dial shows P-SHOCK.
- [x] Package display name is PBL-2500.
- [x] Six fresh native palette frames show the new wordmark.
- [x] Animated GIF is first public preview.
- [x] Public GIF matches local bytes.
- [x] Public download matches archived production bytes.
- [x] Four public stills match local pixels.
- [x] Fresh independent release review passes.

## Dial coherence refinement — 2026-11-06
- [x] Outer rim has consistent inset thickness.
- [x] Hour markers share a consistent metal/lume hierarchy.
- [x] Subdial ring remains legible at native resolution.
- [x] Native palette matrix passes.
- [x] Before/after render is saved.
- [x] Production bundle verifies.
No publication requested for this refinement.

## Publish approved soft-octagon update — 2026-11-06
- [x] Six fresh native palette captures show the approved perimeter.
- [x] Animated palette GIF is the first public preview.
- [x] Four public stills match fresh native captures.
- [x] Public version is 0.1.1.
- [x] Public PBW matches the archived production bundle.
- [x] Independent release review passes.

## Selectable left dial — 2026-11-06
Source: native C renderer and phone Clay/JS. Output: PBL-2500 0.2.0 local preview. Execution: build.py, verify.py, native capture scripts, host and JS tests. Preserve public0.1.1.
- [x] Seconds remains the default left dial.
- [x] Battery dial uses the live battery service.
- [x] Steps dial uses today's Pebble Health count.
- [x] Unavailable health shows a placeholder.
- [x] Weather uses current temperature from Open-Meteo.
- [x] Weather supports Celsius.
- [x] Weather supports Fahrenheit.
- [x] Weather location requests occur only when weather is selected.
- [x] Weather failures show unavailable or valid cached data.
- [x] Weather older than two hours is hidden.
- [x] Non-seconds dials use minute ticks.
- [x] Prior settings migrate with Seconds selected.
- [x] Clay Save persists the dial choice.
- [x] Clay Cancel preserves the dial choice.
- [x] Native comparison shows four dial modes.
- [x] Clean production bundle verifies.
- [x] Fresh independent review passes.

## Left-dial text clearance — 2026-11-06
- [x] Native text masks stay within radius29 of the left dial's center.
- [x] Weather condition labels CLEAR/CLOUD/FOG/RAIN/SNOW/STORM fit.
- [x] Missing-data labels remain inside the circular border.
- [x] Native mode comparison is refreshed.
- [x] Clean production bundle verifies.

## Round compatibility — 2026-11-06
- [x] Emery remains a build target.
- [x] Chalk is a build target.
- [x] Gabbro is a build target.
- [x] The UUID is preserved.
- [x] Existing settings key mappings are preserved.
- [x] Native Chalk layout passes visual review.
- [x] Native Gabbro layout passes visual review.
- [x] Clean production PBW contains three native binaries.
- [x] Existing host/settings checks pass.
- [x] Fresh independent review passes.
No store publication is authorized by this compatibility request.

## Store round update — 2026-11-06
- [x] Publish the verified three-platform production update.
- [x] Preserve the current animated preview.
- [x] Add native screenshots for Chalk.
- [x] Add native screenshots for Gabbro.
- [x] Verify the public download matches the production artifact.
- [x] Verify public round screenshot pixels.
- [x] Fresh independent public-proof review passes.
