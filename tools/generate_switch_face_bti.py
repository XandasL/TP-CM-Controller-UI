from pathlib import Path
import struct

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
SOURCE_DIR = ROOT / "assets" / "switch" / "source"
OUTPUT_DIR = ROOT / "res" / "switch"
BUTTONS = ("a", "b", "x", "y")


def encode_rgba8_bti(image: Image.Image) -> bytes:
    image = image.convert("RGBA").resize((128, 128), Image.Resampling.LANCZOS)
    width, height = image.size

    header = bytearray(32)
    header[0] = 0x06  # GX_TF_RGBA8
    header[1] = 0x01  # alpha enabled
    struct.pack_into(">H", header, 2, width)
    struct.pack_into(">H", header, 4, height)
    header[6] = 0  # wrap S: clamp
    header[7] = 0  # wrap T: clamp
    header[8] = 0  # no palette
    header[9] = 0
    struct.pack_into(">H", header, 10, 0)
    struct.pack_into(">I", header, 12, 0)
    header[20] = 1  # min filter: linear
    header[21] = 1  # mag filter: linear
    header[22] = 0
    header[23] = 0
    header[24] = 1  # one image / no mip chain
    header[25] = 0
    struct.pack_into(">h", header, 26, 0)
    struct.pack_into(">I", header, 28, 32)

    pixels = image.load()
    data = bytearray()

    # GameCube/Wii RGBA8 stores every 4x4 block as two 32-byte planes:
    # first A/R pairs, then G/B pairs.
    for block_y in range(0, height, 4):
        for block_x in range(0, width, 4):
            for y in range(4):
                for x in range(4):
                    r, g, b, a = pixels[block_x + x, block_y + y]
                    data.extend((a, r))
            for y in range(4):
                for x in range(4):
                    r, g, b, a = pixels[block_x + x, block_y + y]
                    data.extend((g, b))

    return bytes(header) + bytes(data)


def main() -> None:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    for button in BUTTONS:
        source = SOURCE_DIR / f"{button}.png"
        output = OUTPUT_DIR / f"{button}.bti"

        if not source.exists():
            raise FileNotFoundError(source)

        with Image.open(source) as image:
            output.write_bytes(encode_rgba8_bti(image))

        print(f"Generated {output.relative_to(ROOT)} from {source.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
