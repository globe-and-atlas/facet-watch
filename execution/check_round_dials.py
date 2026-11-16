"""Measure native subdial text clearance on an already-owned round fixture.

First run build_round_fixture.py then capture_round.py for the chosen platform.
This uses fixture-only isolated text masks; no location or Health data is faked
in production builds.
"""
from pathlib import Path
import argparse
import json
import math
import time

import capture_round as native

ROOT = Path(__file__).resolve().parents[1]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--platform", choices=["chalk", "gabbro"], required=True)
    args = parser.parse_args()
    native.TEMP.mkdir(parents=True, exist_ok=True)
    keys = json.loads((native.TEMP / "message_keys.json").read_text())
    uuid = json.loads((ROOT / "watchface/package.json").read_text())["pebble"]["uuid"]
    size = 180 if args.platform == "chalk" else 260
    scale = lambda n: n * size // 180
    center = (scale(50), scale(90))
    inner = scale(27) - scale(2) / 2
    baseline = {"Palette": 0, "LeftDial": 0, "WeatherUnits": 0,
                "ShowSeconds": 1, "ShowBattery": 1, "DigitalTime": 0,
                "DateFormat": 1, "11000": 10, "11001": 8, "11002": 30,
                "11004": 82, "11009": 5832, "11010": 1, "11011": 222,
                "11012": 2, "11013": 0, "11022": 0}
    cases = [("seconds", {}), ("battery-zero", {"LeftDial": 2, "11004": 0}),
             ("battery-full", {"LeftDial": 2, "11004": 100}),
             ("steps-missing", {"LeftDial": 3, "11009": -1}),
             ("steps-zero", {"LeftDial": 3, "11009": 0})]
    for value in [9999, 99999, 999999]:
        cases.append((f"steps-{value}", {"LeftDial": 3, "11009": value}))
    for name, code in [("clear", 0), ("cloud", 2), ("fog", 45), ("rain", 61),
                       ("snow", 71), ("storm", 95)]:
        cases.append((f"weather-{name}", {"LeftDial": 1, "11012": code}))
    for units in [0, 1]:
        for value in [-900, 650]:
            cases.append((f"weather-{units}-{value}", {"LeftDial": 1,
                          "WeatherUnits": units, "11011": value}))
    cases += [("weather-missing", {"LeftDial": 1, "11010": 0}),
              ("weather-stale", {"LeftDial": 1, "11010": 2})]

    def send(values: dict) -> None:
        native.call(["pebble", "send-app-message", "--emulator", args.platform,
                     "--app-uuid", uuid, "--int",
                     *[f"{keys.get(key, key)}={value}" for key, value in values.items()]])

    evidence = []
    try:
        for name, change in cases:
            send({**baseline, **change, "11013": 1, "11022": 1})
            proof = (scale(89) + 1, scale(10) + 1)
            for attempt in range(20):
                image = native.capture()
                if image.size == (size, size) and image.getpixel(proof) == (255, 0, 255):
                    break
                time.sleep(0.1)
            else:
                raise AssertionError("Expected PBL-2500 fixture not in foreground")
            send({"11022": 0})
            for attempt in range(20):
                image = native.capture()
                if image.getpixel(proof) != (255, 0, 255):
                    break
                time.sleep(0.1)
            else:
                raise AssertionError("Proof marker did not clear")
            ink = [math.hypot(x - center[0], y - center[1])
                   for y in range(size) for x in range(size)
                   if image.getpixel((x, y)) != (0, 0, 0)]
            if not ink:
                raise AssertionError(f"{name}: empty text mask")
            gap = inner - max(ink)
            image.save(native.TEMP / f"{args.platform}-mask-{name}.png")
            if gap < 2:
                raise AssertionError(f"{name}: {gap:.2f}px text-to-inner-ring clearance; need2px")
            evidence.append({"case": name, "minimum_inner_ring_gap_px": round(gap, 2)})
            print(args.platform, name, round(gap, 2), flush=True)
        (native.TEMP / f"dial-clearance-{args.platform}.json").write_text(json.dumps(evidence, indent=2))
    finally:
        send(baseline)


if __name__ == "__main__":
    main()
