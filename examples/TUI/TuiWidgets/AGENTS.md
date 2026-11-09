# TuiWidgets

TUI widget catalogue and test surface (phase 08 / phase 12).
Demonstrates and exercises common `CtrlLib` widgets:
- `MenuBar`, `ToolBar`, `StatusBar`
- `Button`, `Option`, `Switch`, `MultiButton`
- `EditField`, `DocEdit`, `DropList`, `DropChoice`, `SuggestCtrl`
- `ArrayCtrl` with `HeaderCtrl`, in-place editing, and sorting
- `TreeCtrl` with collapsible branches and standard icons
- `Splitter`, `TabCtrl`, `LabelBox`
- `SliderCtrl`, `ProgressIndicator`
- Live theme switching (TUI Dark, TUI Light, TUI ANSI 16)
- Live border policy switching (None / Graphic)
- Live block resolution modes (L0-L4, Braille)
- Live Nerd Font symbol toggle

## Verification
Builds both as TUI and GUI:
- `./bin/build -j 12 -m GCC TuiWidgets` (TUI)
- `./bin/build -j 12 -m GCC -M 1 TuiWidgets` (GUI)
