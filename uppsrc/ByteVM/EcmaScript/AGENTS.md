# ByteVM EcmaScript AGENTS Guide

## Scope
- Applies to `uppsrc/ByteVM/EcmaScript` and its entire sub-tree of U++ packages.

## Purpose
This sub-package provides a self-contained EcmaScript (JavaScript) interpreter and compiler written in C++. It compiles JavaScript source to bytecode and executes it in a virtual machine, providing a scripting environment for U++ applications without requiring V8 or other external runtimes.

## Key Areas
- **JsValue**: JavaScript value representation (number, string, boolean, object, array, function, etc.)
- **JsVM**: Execution engine with stack-based virtual machine
- **JsCompiler**: JavaScript source code to bytecode compiler
- **JsScheduler**: Thread management and execution scheduling
- **PolicyKit**: Sandboxing and permission control
- **JsBindings**: C++ to JavaScript binding utilities

## Extension Points
- Add new built-in functions in `JsVM::RegisterBuiltins()`
- Extend value types by modifying `JsValue` class
- Implement new JavaScript features in `JsCompiler` or `JsVM`
- Add sandboxing rules in `PolicyKit`

## Conventions
- Use proper U++ types (`int64`, `double`, `String`) instead of native C++ primitives
- Use `RecyclerPool` for high-frequency value allocations (see `ai/rules/memory-pooling.md`)
- Document all public APIs with Topic++ (`.tpp`) style comments
- Keep JS parser error handling consistent with Python counterpart

## Dependencies
- Core: Basic U++ functionality
- Core/TextParsing: Text parsing utilities
- ByteVM: VM infrastructure

## Where To Start
- Read `uppsrc/ByteVM/EcmaScript/JsValue.cpp` to understand value operations
- Examine `uppsrc/ByteVM/EcmaScript/JsVM.cpp` for execution engine logic
- Study `uppsrc/ByteVM/EcmaScript/JsCompiler.cpp` for JavaScript compilation flow
- Look at `uppsrc/ByteVM/EcmaScript/PolicyKit.h` for sandboxing model