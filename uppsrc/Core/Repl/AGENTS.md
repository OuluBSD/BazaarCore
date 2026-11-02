# Core/Repl Package

Generic, lightweight, headless line-oriented REPL (Read-Eval-Print Loop) framework for U++ Core applications.

## Key Features
- **Prompt & Line Reading**: Single-line and multiline user input reading.
- **Command History**: In-memory buffer with optional disk load/save.
- **Slash Commands**: Extensible command registry (`/h`, `/q`, `/history`, and custom callbacks like `/c`, `/y`, `/u`).
- **Signal Handling**: POSIX `SIGINT` (Ctrl+C) trap to allow interactive pausing/cancellation without terminating the process.
- **CLI Integration**: Built-in integration with U++ `CommandLineArguments`.

## File Map
- `Repl.upp`: Package definition (uses `Core`).
- `Repl.h`: Main umbrella header.
- `Repl.cpp`: Implementation of `ReplSession`.
