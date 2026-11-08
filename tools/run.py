#!/usr/bin/env python3
"""Reuse or rebuild a.out and run it with interactive terminal input.

Usage: python3 tools/run.py [--cxx g++] [--std c++20]
The executable is kept in the project root for later runs with ./a.out.
"""

import argparse
import subprocess
import sys
from pathlib import Path

from build import BuildError, add_build_arguments, ensure_built


PROJECT_ROOT = Path(__file__).resolve().parent.parent


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    add_build_arguments(parser)
    args = parser.parse_args()

    try:
        executable = ensure_built(args.cxx, args.std, args.force)
        print("[INFO] running ./a.out", flush=True)
        # Inherit stdin/stdout/stderr so cin/scanf read directly from the terminal.
        result = subprocess.run([str(executable)], cwd=PROJECT_ROOT)
        return result.returncode if result.returncode >= 0 else 128 - result.returncode
    except (BuildError, OSError, ValueError) as error:
        print(f"[FAIL] {error}", file=sys.stderr)
        return 2
    except KeyboardInterrupt:
        print("\n[INFO] cancelled", file=sys.stderr)
        return 130


if __name__ == "__main__":
    sys.exit(main())
