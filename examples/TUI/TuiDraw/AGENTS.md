# TuiDraw

TUI graphics and sub-cell rasterisation demo (phase 09 / phase 12).
Demonstrates `Draw` primitives rendered into terminal cells:
- Lines (horizontal, vertical, diagonal)
- Rectangles and borders
- Ellipses and circles
- Smooth color gradient image rasterisation
- Oversized styled text rendered with sub-cell blocks
- Live sub-cell block mode switcher:
  - L0: 1x1 flat cells
  - L1: 2x1 half blocks
  - L2: 2x2 quadrants (default)
  - L3: 2x3 sextants
  - L4: 2x4 octants
  - Braille: 2x4 dots
- Live color policy and theme switching

## Verification
Builds both as TUI and GUI:
- `./bin/build -j 12 -m GCC TuiDraw` (TUI)
- `./bin/build -j 12 -m GCC -M 1 TuiDraw` (GUI)
