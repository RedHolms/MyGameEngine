#!/usr/bin/env python3
"""Example of a tracked generator producing a packaged game asset."""

import sys
from pathlib import Path


output = Path(sys.argv[1])
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text("Generated during the build and packed into game.mga.\n", encoding="utf-8")
