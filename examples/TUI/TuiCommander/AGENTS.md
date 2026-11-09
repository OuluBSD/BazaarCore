# TuiCommander

Two-pane file manager example (phase 12).
Demonstrates comprehensive widget integration and keyboard-driven workflows:
- Two-pane layout with `Splitter` and `ArrayCtrl` / `FileList`
- Active pane toggling via Tab
- Directory traversal with Enter / Backspace
- Function key actions (F3 View, F5 Copy, F7 Mkdir, F8 Delete)
- `MenuBar`, `ToolBar`, and `StatusBar`
- Icon substitution for files and directories (`CtrlImg::Dir()`, `CtrlImg::File()`)
- Modal dialogs for prompts, confirmations, and file viewing
- Live theme switching

## Verification
Builds both as TUI and GUI:
- `./bin/build -j 12 -m GCC TuiCommander` (TUI)
- `./bin/build -j 12 -m GCC -M 1 TuiCommander` (GUI)
