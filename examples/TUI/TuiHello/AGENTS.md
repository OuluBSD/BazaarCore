# TuiHello

Minimal TUI smoke test application.
Demonstrates the most basic `CtrlLib` application: `TopWindow` with `Label`, `Button`, `MenuBar`, and `StatusBar`.

## Verification
Builds and runs both as TUI and GUI:
- `./bin/build -j 12 -m GCC TuiHello` (TUI)
- `./bin/build -j 12 -m GCC -M 1 TuiHello` (GUI)
