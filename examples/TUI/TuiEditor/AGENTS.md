# TuiEditor

TUI text editor example (phase 12).
Demonstrates monospace text editing on a cell grid using `DocEdit`:
- `MenuBar` with File, Edit, View, and Help menus
- `ToolBar` with standard document actions
- `StatusBar` with dynamic line, column, character count, and file status
- `FileSel` for file open and save dialogs
- Monospace font and cell grid alignment
- Clipboard operations (Cut, Copy, Paste, Select All)
- Bracketed paste and cursor caret positioning
- Live theme switching (TUI Dark, TUI Light, TUI ANSI 16)

## Verification
Builds both as TUI and GUI:
- `./bin/build -j 12 -m GCC TuiEditor` (TUI)
- `./bin/build -j 12 -m GCC -M 1 TuiEditor` (GUI)
