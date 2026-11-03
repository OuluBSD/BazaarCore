# TerminalCore

The GUI-free half of the [Terminal package](../Terminal/ReadMe.md): the VT cell
model, the page buffer and the escape-sequence parser.

| file | contents |
|------|----------|
| `Parser.h/.cpp` | `VTInStream` — a VT500-series lexical parser for DEC and ANSI escape sequences, after Paul Flo Williams' state diagram |
| `Cell.h/.cpp`   | `VTCell` — one cell: `Color ink`/`Color paper` plus the SGR attribute set, with a fluent setter API and selective `FillerFlags` fill |
| `Page.h/.cpp`   | `VTPage` / `VTLine` — the grid, with cursor, margins and scrollback |
| `Sixel.h/.cpp`  | `SixelRenderer` — sixel image decoding |

Include `<TerminalCore/TerminalCore.h>`; it fixes the dependency order in one
place.

## Why it was split out

`uppsrc/Terminal` was written as a terminal *emulator widget*, and its widget
half depends on CtrlLib. The four modules above never did: they carry no
reference to `TerminalCtrl` and were already grouped under their own separators
in `Terminal.upp`. Extracting them lets a second surface reuse VT semantics
rather than growing a parallel implementation of them — the TUI backend in
`uppsrc/CtrlCore` is the first such consumer, and treats its cell as a `VTCell`.

The split is a file move plus two `.upp` edits. No logic was changed, by design:
if a file had needed an edit to move, the boundary would have been wrong.

## Upstream

`uppsrc/Terminal` tracks an upstream U++ package, so this split is local and
makes future merges of those eight files land here instead. Keep it a pure
move: code changes belong behind the repo's feature flag convention, not in the
move itself.

Documentation topics (`Upp_VTInStream_en-us.tpp`, `Upp_SixelRenderer_en-us.tpp`)
deliberately stay in `uppsrc/Terminal/src.tpp` with the rest of the package's
reference, along with `Version`, `Copying` and the `.usc` file.
