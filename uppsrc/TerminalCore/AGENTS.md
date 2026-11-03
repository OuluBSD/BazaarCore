# TerminalCore Package AGENTS Guide

## Scope
- Applies to `uppsrc/TerminalCore/` and its components.
- Listed as the first entry in `TerminalCore.upp`.

## Purpose
`TerminalCore` provides the GUI-free core data structures and parsers for virtual terminal handling:
- **Cell Model (`VTCell`)**: Representation of an individual character cell, including UTF-32 character, foreground ink color, background paper color, and SGR attribute flags (bold, faint, italic, underline, strikeout, inverse).
- **Page Buffer (`VTPage`, `VTLine`)**: Grid management, cursor tracking, scrollback history, and line-level manipulation.
- **Escape Parser (`VTInStream`)**: A robust lexical state-machine parser for ANSI and DEC VT500-series control sequences, based on Paul Flo Williams' state diagram.
- **Sixel Graphics (`SixelRenderer`)**: Sixel image decoding engine.

## Rationale for the Split from `uppsrc/Terminal`
Historically, `VTInStream`, `VTCell`, `VTPage`, and `SixelRenderer` lived inside `uppsrc/Terminal` alongside `TerminalCtrl`.

`TerminalCtrl` is a GUI widget and inherently depends on `CtrlLib`. However:
1. The cell model, parser, and page buffer do **not** require `CtrlLib`.
2. The native TUI backend in `uppsrc/CtrlCore` needs VT cell structures and ANSI sequence parsing.
3. If these modules remained in `uppsrc/Terminal`, `CtrlCore` would have to depend on `Terminal`, causing a cyclic package dependency (`CtrlCore` -> `Terminal` -> `CtrlLib` -> `CtrlCore`).

Extracting the GUI-free half into `TerminalCore` allows `CtrlCore` (and any other headless or console tool) to consume VT semantics with only `Core` and `Draw` dependencies.

## Key Components

| File | Primary Classes / Functions | Responsibility |
|---|---|---|
| `Cell.h` / `Cell.cpp` | `VTCell`, `FillerFlags` | Single-cell attributes, ink/paper colors, fluent setters |
| `Page.h` / `Page.cpp` | `VTPage`, `VTLine` | Multi-line grid buffer, scrolling, cursor tracking |
| `Parser.h` / `Parser.cpp` | `VTInStream` | State machine decoding CSI, OSC, DCS, and ESC sequences |
| `Sixel.h` / `Sixel.cpp` | `SixelRenderer` | Parsing and rasterizing sixel image payloads |

## Upstream Synchronization
`uppsrc/Terminal` tracks an upstream U++ package. When merging upstream changes:
- Changes to `Cell.*`, `Page.*`, `Parser.*`, and `Sixel.*` belong in `TerminalCore`.
- Upstream changes to `TerminalCtrl`, `Terminal.cpp`, and GUI event handlers belong in `uppsrc/Terminal`.
- Preserve the clean boundary: **never** introduce `CtrlLib` or `CtrlCore` dependencies into `TerminalCore`.
