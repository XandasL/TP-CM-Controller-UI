#!/usr/bin/env python3
from pathlib import Path
from PIL import Image

src = Path("res/banner.jpg")
dst = Path("res/banner.png")

if not src.is_file():
    raise SystemExit(f"Missing source banner: {src}")

with Image.open(src) as image:
    image.seek(0)
    image.convert("RGB").save(dst, format="PNG", optimize=True)

print(f"Converted {src} -> {dst}")
