#!/usr/bin/env python3
"""Expand local C++ headers into a single submission file.

Usage (from the project root):
    python3 tools/expander.py main.cpp > output.cpp

Inspired by https://github.com/maspypy/library/blob/main/scripts/expander.py
Uses only the Python standard library on macOS and WSL. Each local file is
expanded once; system includes and comments are preserved. This is a textual
expander, not a C++ preprocessor: use unconditional, literal local includes.
"""

import argparse
import json
import re
import sys
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parent.parent
LOCAL_INCLUDE = re.compile(r'^\s*#\s*include\s*"([^"\n]+)"')
PRAGMA_ONCE = re.compile(r'^\s*#\s*pragma\s+once\b')


class ExpansionError(Exception):
    pass


class Expander:
    def __init__(self, include_dirs=()):
        self.include_dirs = [PROJECT_ROOT, *map(Path, include_dirs)]
        self.visited = set()

    @staticmethod
    def display_name(path):
        try:
            return path.relative_to(PROJECT_ROOT).as_posix()
        except ValueError:
            return path.as_posix()

    def resolve_include(self, header, including_file):
        # Match quoted-include semantics before trying project include paths.
        for directory in [including_file.parent, *self.include_dirs]:
            candidate = (directory / header).resolve()
            if candidate.is_file():
                return candidate
        raise ExpansionError(f'local header not found: "{header}"')

    def expand(self, path):
        path = Path(path).resolve()
        if path in self.visited:
            return []
        self.visited.add(path)

        name = self.display_name(path)
        try:
            source = path.read_text(encoding="utf-8-sig")
        except (OSError, UnicodeError) as error:
            raise ExpansionError(f"cannot read {name}: {error}") from error

        result = [f"// BEGIN: {name}\n"]
        next_line = None
        for line_number, line in enumerate(source.splitlines(), start=1):
            if PRAGMA_ONCE.match(line):
                next_line = None
                continue

            match = LOCAL_INCLUDE.match(line)
            if match:
                try:
                    header = self.resolve_include(match.group(1), path)
                    result.extend(self.expand(header))
                except ExpansionError as error:
                    raise ExpansionError(f"{name}:{line_number}: {error}") from error
                next_line = None
                continue

            if next_line != line_number:
                # JSON quoting also escapes backslashes and quotes for #line.
                result.append(f"#line {line_number} {json.dumps(name, ensure_ascii=False)}\n")
            result.append(line + "\n")
            next_line = line_number + 1

        result.append(f"// END: {name}\n")
        return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", nargs="?", default="main.cpp", help="source file (default: main.cpp)")
    parser.add_argument("-I", dest="include_dirs", action="append", default=[], metavar="DIR",
                        help="additional local header search directory (repeatable)")
    args = parser.parse_args()

    try:
        output = "".join(Expander(args.include_dirs).expand(args.source))
    except (ExpansionError, OSError, RuntimeError) as error:
        print(f"[FAIL] {error}", file=sys.stderr)
        return 1

    # Do not emit partial source when an include cannot be expanded.
    sys.stdout.write(output)
    return 0


if __name__ == "__main__":
    sys.exit(main())
