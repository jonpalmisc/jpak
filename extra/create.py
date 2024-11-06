#!/usr/bin/env python3

# This is an alternative Python implementation for creating JPAK archives;
# useful if creating a JPAK is needed in CI, etc. where the reference
# implementation might not be available.

import struct
import sys

HEADER_SIZE = 16
ENTRY_SIZE = 32
MAX_NAME_SIZE = 24

if len(sys.argv) < 3:
    print(f"Usage: {sys.argv[0]} <output> <file>...")
    sys.exit(1)

out_path = sys.argv[1]
paths = sys.argv[2:]

header = b"JPAK"  # Magic
header += struct.pack("<I", 1)  # Format version
header += struct.pack("<I", len(paths))  # File count
header += struct.pack("<I", 2**32 - 1)  # Reserved

with open(out_path, "wb") as f:
    f.write(header)

    # Reserve space for entries
    for path in paths:
        f.write(b"\0" * 32)

    i = 0
    for path in paths:
        with open(path, "rb") as in_file:
            data = in_file.read()

        offset = f.tell()
        f.write(data)

        entry = struct.pack("<I", offset)
        entry += struct.pack("<I", len(data))

        name = path.encode()[:23]
        while len(name) < MAX_NAME_SIZE - 1:
            name += b"\0"  # Pad out to 24 bytes
        entry += name + b"\0"

        # Update entry in-place
        f.seek(HEADER_SIZE + (i * ENTRY_SIZE))
        f.write(entry)

        # Seek back to end
        f.seek(0, 2)

        i += 1
