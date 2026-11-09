# TuiDialogs

TUI dialog and layout showcase (phase 12).
Demonstrates modal loops, dialog positioning, and layout metrics:
- `.lay`-defined dialog (`WithDialogLayout<TopWindow>`) proving `Zx`/`Zy` font metrics
- Nested modal dialogs (Parent -> Child modal -> Prompt)
- Built-in prompts (`PromptOK`, `PromptYesNo`, `PromptOKCancel`)
- Resizable modal dialog with responsive widget layout
- Oversized dialog (testing Phase 05 screen-clamping and boundary policies)
- `MenuBar`, `StatusBar`, and keyboard navigation

## Verification
Builds both as TUI and GUI:
- `./bin/build -j 12 -m GCC TuiDialogs` (TUI)
- `./bin/build -j 12 -m GCC -M 1 TuiDialogs` (GUI)
