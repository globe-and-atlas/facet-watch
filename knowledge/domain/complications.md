---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-11-06T16:52:31-06:00"
---
# Left-dial data and lifecycle

- AppMessage keys are appended after the original nine, preserving older mappings. Persistent keys 1–9 stay unchanged; keys 10/11 store LeftDial/WeatherUnits. Absent new keys mean Seconds/Fahrenheit.
- Legacy ShowSeconds now controls left-dial visibility; old hidden-dial settings stay hidden. `seconds_active` enables SECOND_UNIT only for visible Seconds. Other modes update on minute/battery/health events.
- Steps uses HealthMetricStepCount, time_start_of_today(), accessibility checks, and health_service_sum_today(). Health unavailability is -1, not zero. The manifest declares health. Daily reads and Health events avoid retaining yesterday's total.
- Weather uses Open-Meteo's HTTPS forecast endpoint with `current=temperature_2m,weather_code`, Celsius, and Unix timestamps. Phone data uses tenths Celsius; native UI performs °C/°F conversion. Weather code0 is CLEAR (not necessarily daytime sun).
- Location capability is declared. Weather must be selected and visible before GPS/network calls. Coordinates are rounded to 0.01 degrees, not cached or logged. Weather cache holds temperature/code/timestamp only.
- Normal refresh is 30 minutes; errors back off from 5 to 30 minutes. Retry-After is honored up to two hours. GPS and HTTP timeouts are 10s and 15s. Permission errors use the longer cooldown. Expiry is two hours, with five minutes allowed clock skew.
- All phone messages share one ACK-driven outbox queue with at most two retries. Settings and cached weather must not compete for PebbleKit's single outstanding message.
- Source references: https://developer.rebble.io/guides/events-and-services/health/ and https://open-meteo.com/en/docs . Weather attribution is included in settings and README.

Temperature rounding uses the complete rational Fahrenheit conversion `(9*c10+1600)/50` before rounding, avoiding intermediate truncation near 32°F.

## Text clearance
The 72px left dial has center(36,36), ring radius33 with2px stroke. Headers use a46px box at(13,13); main values use58px at(7,23); footer labels use46px at(13,43). Native fixture key11013 isolates text/bar ink while hiding the ring, base artwork and main hands. capture_dials.py requires ink radius≤29 (at least3px inside the ring's inner edge), including weather labels, negative/max temperatures,100% battery, unavailable data and large step counts. The fixture hook is excluded from production.
