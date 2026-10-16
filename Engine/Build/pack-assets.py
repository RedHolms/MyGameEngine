#!/usr/bin/env python3
"""Pack raw files into the small MyGameEngine asset archive format."""

import argparse
import os
from pathlib import Path
import struct
import tempfile


U32_MAX = (1 << 32) - 1
U64_MAX = (1 << 64) - 1


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--asset", action="append", nargs=2, metavar=("SOURCE", "ARCHIVE_PATH"), default=[])
    return parser.parse_args()


def pack(output: Path, assets):
    entries = []
    seen = set()
    for source_text, name in assets:
        source = Path(source_text)
        name = name.replace("\\", "/")
        name_bytes = name.encode("utf-8")
        if not name or name.startswith("/") or ":" in name or any(part in ("", ".", "..") for part in name.split("/")):
            raise ValueError(f"invalid archive-relative asset path: {name!r}")
        if name in seen:
            raise ValueError(f"duplicate asset path in archive: {name!r}")
        seen.add(name)
        if len(name_bytes) > U32_MAX:
            raise ValueError(f"asset path is too long: {name!r}")
        data_size = source.stat().st_size
        if data_size > U64_MAX:
            raise ValueError(f"asset is too large: {source}")
        entries.append((name, name_bytes, source, data_size))

    entries.sort(key=lambda entry: entry[0])
    if len(entries) > U32_MAX:
        raise ValueError("too many assets in archive")

    header_size = 4 + sum(4 + len(name_bytes) + 8 + 8 for _, name_bytes, _, _ in entries)
    offset = header_size
    indexed = []
    for name, name_bytes, source, data_size in entries:
        indexed.append((name_bytes, source, offset, data_size))
        offset += data_size
        if offset > U64_MAX:
            raise ValueError("archive is too large")

    output.parent.mkdir(parents=True, exist_ok=True)
    temp_path = None
    try:
        with tempfile.NamedTemporaryFile(dir=output.parent, prefix=output.name + ".", delete=False) as stream:
            temp_path = Path(stream.name)
            stream.write(struct.pack("<I", len(indexed)))
            for name_bytes, _, data_offset, data_size in indexed:
                stream.write(struct.pack("<I", len(name_bytes)))
                stream.write(name_bytes)
                stream.write(struct.pack("<QQ", data_offset, data_size))
            for _, _, source, expected_size in entries:
                copied_size = 0
                with source.open("rb") as asset_stream:
                    while chunk := asset_stream.read(1024 * 1024):
                        stream.write(chunk)
                        copied_size += len(chunk)
                if copied_size != expected_size:
                    raise ValueError(f"asset changed while the archive was being written: {source}")
        os.replace(temp_path, output)
    finally:
        if temp_path is not None and temp_path.exists():
            temp_path.unlink()


def main():
    args = parse_args()
    try:
        pack(args.output, args.asset)
    except (OSError, UnicodeError, ValueError) as error:
        raise SystemExit(f"asset archive error: {error}") from error


if __name__ == "__main__":
    main()
