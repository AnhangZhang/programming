#!/usr/bin/env python3
"""Build main.cpp only when its dependencies or compiler settings change.

run.py and test.py share the executable a.out and the local .build/state.json
record. Can also be called directly: python3 tools/build.py [--force].
"""

import argparse
import hashlib
import json
import os
import platform
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parent.parent
BUILD_ENVIRONMENT = (
    "CPATH", "CPLUS_INCLUDE_PATH", "C_INCLUDE_PATH", "LIBRARY_PATH",
    "COMPILER_PATH", "GCC_EXEC_PREFIX", "SDKROOT", "MACOSX_DEPLOYMENT_TARGET",
)


class BuildError(Exception):
    pass


def add_build_arguments(parser):
    parser.add_argument("--cxx", default=os.environ.get("CXX", "g++"),
                        help="compiler command (default: CXX environment variable or g++)")
    parser.add_argument("--std", default="c++20",
                        help="C++ standard (default: c++20)")
    parser.add_argument("--force", action="store_true", help="force recompilation")


def file_hash(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def read_dependencies(depfile):
    # -MT fixes the target to a.out, so the first colon is always its separator.
    text = depfile.read_text(encoding="utf-8").replace("\\\n", " ")
    target, separator, prerequisites = text.partition(":")
    if not separator or target.strip() != "a.out":
        raise BuildError("compiler did not produce a valid dependency file")
    paths = set()
    # Make escapes spaces and # with backslashes, and $ by doubling it.
    # Unlike shell syntax, literal quote characters must not be stripped.
    for token in re.findall(r"(?:\\.|[^\s\\])+", prerequisites):
        name = re.sub(r"\\(.)", r"\1", token).replace("$$", "$")
        path = Path(name)
        paths.add(path if path.is_absolute() else PROJECT_ROOT / path)
    paths.add(PROJECT_ROOT / "main.cpp")
    return sorted(paths)


def cache_matches(record_path, executable, build_key):
    try:
        record = json.loads(record_path.read_text(encoding="utf-8"))
        dependencies = record["dependencies"]
        return (
            record["key"] == build_key
            and os.access(executable, os.X_OK)
            and file_hash(executable) == record["executable"]
            and str(PROJECT_ROOT / "main.cpp") in dependencies
            and all(file_hash(path) == digest for path, digest in dependencies.items())
        )
    except (OSError, ValueError, KeyError, TypeError, AttributeError):
        return False


def ensure_built(cxx, standard, force=False):
    compiler = shlex.split(cxx)
    if not compiler:
        raise BuildError("compiler command cannot be empty")
    compiler_path = shutil.which(compiler[0])
    if compiler_path is None:
        raise BuildError(f"compiler not found: {compiler[0]}")
    # Preserve the invoked driver name (e.g. clang++ versus clang).
    compiler_path = Path(compiler_path).absolute()
    compiler[0] = str(compiler_path)
    source = PROJECT_ROOT / "main.cpp"
    if not source.is_file():
        raise BuildError("main.cpp not found")

    executable = PROJECT_ROOT / "a.out"
    cache_dir = PROJECT_ROOT / ".build"
    record_path = cache_dir / "state.json"
    command = [*compiler, f"-std={standard}", "-O2", "-Wall", "-Wextra", "-DLOCAL",
               "-I", str(PROJECT_ROOT), str(source)]
    compiler_stat = compiler_path.stat()
    build_key = {
        "command": command,
        "compiler": [compiler_stat.st_mtime_ns, compiler_stat.st_size],
        "platform": [sys.platform, platform.machine()],
        "environment": {key: os.environ.get(key) for key in BUILD_ENVIRONMENT},
        "builder": file_hash(Path(__file__)),
    }
    if not force and cache_matches(record_path, executable, build_key):
        print("[INFO] reusing a.out", flush=True)
        return executable

    cache_dir.mkdir(exist_ok=True)
    # Build separately so failed compilation never replaces a working executable.
    with tempfile.TemporaryDirectory(prefix="compile-", dir=cache_dir) as temp:
        staged = Path(temp) / "a.out"
        depfile = Path(temp) / "dependencies.d"
        print("[INFO] compiling main.cpp", flush=True)
        result = subprocess.run(
            [*command, "-MMD", "-MF", str(depfile), "-MT", "a.out",
             "-o", str(staged)], cwd=PROJECT_ROOT,
        )
        if result.returncode != 0:
            raise BuildError("compilation failed")

        dependencies = read_dependencies(depfile)
        record = {
            "key": build_key,
            "dependencies": {str(path): file_hash(path) for path in dependencies},
            "executable": file_hash(staged),
        }
        staged_record = Path(temp) / "state.json"
        staged_record.write_text(json.dumps(record, indent=2) + "\n", encoding="utf-8")
        staged.replace(executable)
        staged_record.replace(record_path)
    print("[INFO] built a.out", flush=True)
    return executable


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    add_build_arguments(parser)
    args = parser.parse_args()
    try:
        ensure_built(args.cxx, args.std, args.force)
    except (BuildError, OSError, ValueError) as error:
        print(f"[FAIL] {error}", file=sys.stderr)
        return 2
    except KeyboardInterrupt:
        print("\n[INFO] cancelled", file=sys.stderr)
        return 130
    return 0


if __name__ == "__main__":
    sys.exit(main())
