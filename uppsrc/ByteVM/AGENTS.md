# ByteVM Package AGENTS Guide

## Scope
- Applies to `uppsrc/ByteVM` and its entire sub-tree of U++ packages.

## Purpose
- Virtual machine for executing compiled bytecode
- Provides sandboxed execution environment for scripts and applications
- Supports multiple script languages and runtime environments
- Integrates with other U++ packages for virtualized execution

## Key Areas
- **VM Core**: Central virtual machine execution engine
- **Bytecode Execution**: Interpreter for compiled bytecode instructions
- **Runtime Environment**: Memory management and resource allocation
- **Language Support**: Multiple script language compilers and interpreters
- **Sandboxing**: Security boundaries and isolation mechanisms
- **Garbage Collection**: Memory management and object lifecycle

## Extension Points
- Add new VM instructions in the interpreter cores (`Python/PyVM.cpp`, `EcmaScript` JsVM)
- Implement custom language compilers for new script languages
- Extend memory management with custom allocators
- Add new sandboxing mechanisms for enhanced security
- Integrate with external virtual machine implementations

## Conventions
- Follow `/CODESTYLE.md` for code style and documentation
- Use proper U++ naming conventions (PascalCase for classes, camelCase for methods)
- Include `AGENTS.md` as first file in `.upp` manifest
- Document all public APIs with Topic++ (`.tpp`) style comments
- Use `Separate` and `ReadOnly` options in `.upp` for logical grouping
- Keep package header (`AGENTS.md`) up to date with any structural changes
- Use proper acronym casing for shared components
- Prefer shared implementations over duplicated code patterns
- Follow existing patterns in the codebase

- Follow repo-wide naming rules: `ai/rules/naming-conventions.md`

## Dependencies
- Core: Basic U++ functionality
- Common: Shared utilities and base classes
- TheIDE: Integration with the IDE application

## Where To Start
- Review `ByteVM.h` — the umbrella that includes `Python/Python.h`
- Examine the `Python/` subpackage (`PyCompiler`, `PyCode.h`, `PyIR.h`, `PyVM.cpp`, `PolicyKit`)
- Examine the `EcmaScript/` subpackage (`JsCompiler`, `JsCode.h`, `JsBindings.h`, `DomBindings`)
- Check `PyBindings.h` for Python C-API bindings
- Use `RecyclerPool` for high-frequency allocations (see `ai/rules/memory-pooling.md`)

## Design Principles
- Secure sandboxed execution for untrusted code
- Efficient bytecode interpretation with minimal overhead
- Extensible architecture for new language support
- Consistent API design for VM operations
- Comprehensive error handling and reporting

