#!/usr/bin/env python3
"""Receive one Competitive Companion problem, replace samples/, and exit.

Run: python3 tools/samples.py [--port 27121]
Then click Competitive Companion on a problem page in your browser.
The first accepted problem replaces previous samples with sample-N.in/.out.
Invalid requests leave the receiver waiting for a valid problem.
"""

import argparse
import json
import shutil
import sys
import tempfile
from http.server import BaseHTTPRequestHandler, HTTPServer
from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parent.parent
MAX_REQUEST_BYTES = 16 * 1024 * 1024


def validate_samples(problem):
    if not isinstance(problem, dict) or not isinstance(problem.get("tests"), list):
        raise ValueError("expected a Competitive Companion object with a tests array")
    samples = []
    for index, test in enumerate(problem["tests"], start=1):
        if not isinstance(test, dict) or not all(
            isinstance(test.get(key), str) for key in ("input", "output")
        ):
            raise ValueError(f"sample {index}: input and output must be strings")
        # Validate encoding before changing anything on disk; preserve exact text.
        samples.append((test["input"].encode("utf-8"), test["output"].encode("utf-8")))
    return samples


def save_samples(samples):
    destination = PROJECT_ROOT / "samples"
    if destination.is_symlink() or (destination.exists() and not destination.is_dir()):
        raise OSError("samples must be a regular directory, not a file or symlink")

    # Stage on the same filesystem so directory renames work on WSL and macOS.
    work = Path(tempfile.mkdtemp(prefix=".samples-", dir=PROJECT_ROOT))
    staged = work / "new"
    backup = work / "old"
    try:
        staged.mkdir()
        for index, (input_data, output_data) in enumerate(samples, start=1):
            (staged / f"sample-{index}.in").write_bytes(input_data)
            (staged / f"sample-{index}.out").write_bytes(output_data)

        if destination.exists():
            destination.rename(backup)
        try:
            staged.rename(destination)
        except OSError:
            if backup.exists():
                backup.rename(destination)
            raise
        if backup.exists():
            shutil.rmtree(backup)
    finally:
        # If rollback itself fails, leave the old data in work/old for recovery.
        if not backup.exists():
            shutil.rmtree(work)


class CompanionHandler(BaseHTTPRequestHandler):
    def log_request(self, code="-", size="-"):
        log = self.log_error if isinstance(code, int) and code >= 400 else self.log_message
        log('"%s" %s %s', self.requestline, code, size)

    def log_message(self, format, *args):
        print(f"[INFO] HTTP {format % args}", file=sys.stderr, flush=True)

    def log_error(self, format, *args):
        print(f"[FAIL] HTTP {format % args}", file=sys.stderr, flush=True)

    def setup(self):
        super().setup()
        self.connection.settimeout(10)

    def respond(self, status, data=None):
        body = b"" if data is None else json.dumps(data, ensure_ascii=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.send_header("Access-Control-Allow-Private-Network", "true")
        self.end_headers()
        self.wfile.write(body)

    def do_OPTIONS(self):
        self.respond(204)

    def do_GET(self):
        if self.path != "/":
            self.respond(404, {"error": "not found"})
            return
        self.respond(200, {"status": "ready", "directory": "samples"})

    def do_POST(self):
        if self.path != "/":
            self.respond(404, {"error": "not found"})
            return
        try:
            length = int(self.headers.get("Content-Length", "0"))
        except ValueError:
            self.respond(400, {"error": "invalid Content-Length"})
            return
        if length <= 0 or length > MAX_REQUEST_BYTES:
            self.respond(413 if length > MAX_REQUEST_BYTES else 400,
                         {"error": "expected a JSON body of at most 16 MiB"})
            return

        try:
            body = self.rfile.read(length)
            if len(body) != length:
                raise ValueError("incomplete request body")
            problem = json.loads(body.decode("utf-8"))
            samples = validate_samples(problem)
        except (ValueError, UnicodeError, RecursionError) as error:
            self.respond(400, {"error": str(error)})
            return
        except OSError as error:
            self.respond(408, {"error": str(error)})
            return

        try:
            save_samples(samples)
        except OSError as error:
            self.respond(500, {"error": f"cannot save samples: {error}"})
            return

        name = problem.get("name")
        name = name if isinstance(name, str) else "unnamed problem"
        print(f"[INFO] saved {len(samples)} sample(s) to samples/: {name!r}", flush=True)
        self.server.received = True
        self.respond(200, {"status": "saved", "samples": len(samples)})


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--port", type=int, default=27121,
                        help="listening port (default: 27121)")
    args = parser.parse_args()
    if not 1 <= args.port <= 65535:
        parser.error("port must be between 1 and 65535")

    try:
        with HTTPServer(("127.0.0.1", args.port), CompanionHandler) as server:
            server.received = False
            print(f"[INFO] listening on http://localhost:{args.port}/", flush=True)
            print(f"[INFO] samples directory: {PROJECT_ROOT / 'samples'}", flush=True)
            print("[INFO] click Competitive Companion on a problem page. Exits after saving one problem.",
                  flush=True)
            while not server.received:
                server.handle_request()
        print("[INFO] saved successfully. Receiver closed.")
    except KeyboardInterrupt:
        print("\n[INFO] stopped.")
    except OSError as error:
        print(f"[FAIL] {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
