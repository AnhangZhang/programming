#!/usr/bin/env python3
"""Reuse or rebuild a.out and run every input/output pair in samples/.

Usage: python3 tools/test.py [--timeout 2] [--cxx g++] [--std c++20]
Output comparison ignores whitespace differences. Exit codes: 0 for all
samples passing, 1 for failed samples, 2 for setup or compilation failure.
"""

import argparse
import math
import re
import subprocess
import sys
import time
from pathlib import Path

from build import BuildError, add_build_arguments, ensure_built


PROJECT_ROOT = Path(__file__).resolve().parent.parent


def sample_order(path):
    return [int(part) if part.isdigit() else part
            for part in re.split(r"(\d+)", path.name)]


def show_block(label, data):
    print(f"[{label}]")
    text = data.decode("utf-8", errors="replace")
    lines = text.splitlines()
    if not lines:
        print("<empty>")
        return
    for line in lines[:40]:
        print(line if len(line) <= 200 else line[:200] + " ...")
    if len(lines) > 40:
        print("... (truncated)")


def run_sample(executable, input_data, expected, timeout):
    started = time.perf_counter()
    try:
        result = subprocess.run(
            [str(executable)], input=input_data, stdout=subprocess.PIPE,
            stderr=subprocess.PIPE, cwd=PROJECT_ROOT, timeout=timeout,
        )
        actual, stderr = result.stdout, result.stderr
        passed = result.returncode == 0 and actual.split() == expected.split()
    except subprocess.TimeoutExpired as error:
        actual, stderr = error.stdout or b"", error.stderr or b""
        passed = False
    elapsed = time.perf_counter() - started
    return passed, actual, stderr, elapsed


def test_samples(cxx, standard, timeout, force=False):
    samples_dir = PROJECT_ROOT / "samples"
    inputs = sorted(samples_dir.glob("*.in"), key=sample_order)
    if not inputs:
        raise ValueError("no .in files found in samples/")
    for input_path in inputs:
        if not input_path.with_suffix(".out").is_file():
            raise ValueError(f"missing expected output: samples/{input_path.stem}.out")

    executable = ensure_built(cxx, standard, force)
    passed_count = 0
    for input_path in inputs:
        input_data = input_path.read_bytes()
        expected = input_path.with_suffix(".out").read_bytes()
        print(f"[INFO] {input_path.stem}", flush=True)
        passed, actual, stderr, elapsed = run_sample(
            executable, input_data, expected, timeout,
        )
        print(f"[INFO] time: {elapsed:.6f} sec")
        print("[PASS]" if passed else "[FAIL]")
        if passed:
            passed_count += 1
        else:
            show_block("input", input_data)
            show_block("output", actual)
            show_block("expected", expected)
            if stderr:
                show_block("stderr", stderr)
        print("-" * 40, flush=True)

    print(f"[INFO] {passed_count}/{len(inputs)} samples passed")
    return 0 if passed_count == len(inputs) else 1


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--timeout", type=float, default=2.0,
                        help="time limit per sample in seconds (default: 2)")
    add_build_arguments(parser)
    args = parser.parse_args()
    if not math.isfinite(args.timeout) or args.timeout <= 0:
        parser.error("timeout must be a positive, finite number")
    try:
        return test_samples(args.cxx, args.std, args.timeout, args.force)
    except (BuildError, OSError, ValueError) as error:
        print(f"[FAIL] {error}", file=sys.stderr)
        return 2
    except KeyboardInterrupt:
        print("\n[INFO] cancelled", file=sys.stderr)
        return 130


if __name__ == "__main__":
    sys.exit(main())
