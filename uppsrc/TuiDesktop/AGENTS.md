# TuiDesktop

Package: `uppsrc/TuiDesktop`
Layer: 2 (VirtualGui host) / Layer 3 (Desktop shell) integration

## Overview

`TuiDesktop` implements `TuiVirtualGui`, a `VirtualGui` host that runs over the terminal layer (`TheTuiTerminal()`, `TuiDraw`, `TheTuiScreen()`).
It allows running a multi-window desktop inside a single terminal, with:
- Multiple overlapping `TopWindow`s hosted simultaneously
- Window decorations and borders obeying Chameleon border policies (`CHBORDER_GRAPHIC`, `CHBORDER_PAPER`, `CHBORDER_NONE`)
- Active window distinction (highlighted title and graphic border on foreground window, paper border on background windows)
- DesktopSuite `TaskBar` integration with 1-row taskbar, window focus activation on click, and `Alt+Tab` window cycling
- Native event routing between VT escape sequences and `VirtualGui`'s `Ctrl::DoMouseFB` and `Ctrl::DoKeyFB`

## Key Types & Functions

- `class TuiVirtualGui : public VirtualGui`: The concrete VirtualGui host driving terminal I/O.
- `void RunTuiVirtualGui(TuiVirtualGui& gui, Event<> app_main)`: Runs a VirtualGui desktop session in the terminal.

## Dual-Build Selection

When compiling with flag `TUI` and flag `VIRTUALGUI`:
- `CtrlCore.h` selects `<VirtualGui/VirtualGui.h>` as `GUIPLATFORM_INCLUDE`, enabling `VirtualGui`'s window manager and compositor.
- `TuiDesktop` connects `VirtualGui` to `TheTuiTerminal()`.
- Plain TUI applications (`flagTUI` without `flagVIRTUALGUI`) continue using `TuiGui.h` without VirtualGui overhead.
