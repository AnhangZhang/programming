#!/usr/bin/env python3
"""Save an unchanged copy of main.cpp in archive/.

Usage:
    python3 tools/archive.py abc123_a

Creates archive/abc123_a.cpp relative to this project, regardless of the
working directory. Existing archives are never overwritten. main.cpp is
preserved, including its local includes, encoding, and line endings.
"""

import argparse
import re
import sys
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parent.parent


def archive_filename(name):
    """Accept a filename, not a path, and add the C++ extension once."""
    if name.lower().endswith(".cpp"):
        name = name[:-4]
    if not name or name in (".", ".."):
        raise argparse.ArgumentTypeError("please provide a nonempty filename")
    if re.search(r'[<>:"/\\|?*\x00-\x1f]', name):
        raise argparse.ArgumentTypeError("use a filename without paths or special characters")
    if name != name.strip() or name.endswith("."):
        raise argparse.ArgumentTypeError("filename cannot start/end with spaces or end with a dot")
    return name + ".cpp"


def archive_main(filename):
    source = PROJECT_ROOT / "main.cpp"
    destination = PROJECT_ROOT / "archive" / filename
    # Read before creating the destination; preserve all source bytes.
    content = source.read_bytes()
    destination.parent.mkdir(parents=True, exist_ok=True)

    # Exclusive creation also prevents overwrites if two calls run at once.
    with destination.open("xb") as output:
        try:
            output.write(content)
            output.flush()
        except BaseException:
            output.close()
            destination.unlink()
            raise
    return destination


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("name", type=archive_filename,
                        help="archive filename, e.g. abc123_a (.cpp is optional)")
    args = parser.parse_args()

    try:
        destination = archive_main(args.name)
    except FileExistsError:
        print(f"[FAIL] archive/{args.name} already exists", file=sys.stderr)
        return 1
    except OSError as error:
        print(f"[FAIL] {error}", file=sys.stderr)
        return 1

    print(f"[INFO] archived: {destination.relative_to(PROJECT_ROOT).as_posix()}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
