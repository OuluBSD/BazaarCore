# ByteVM Python AGENTS Guide

## Scope
- Applies to `uppsrc/ByteVM/Python` and its entire sub-tree of U++ packages.

## Purpose
This sub-package provides a self-contained Python interpreter and compiler written in C++. It compiles Python source to bytecode and executes it in a virtual machine, providing a scripting environment for U++ applications without requiring CPython.

## Key Areas
- **PyValue**: Value representation (int, float, str, list, dict, tuple, set, function, lambda, iterator, etc.)
- **PyVM**: Execution engine with stack-based virtual machine
- **PyCompiler**: Python source code to bytecode compiler
- **PyScheduler**: Thread management and execution scheduling
- **PolicyKit**: Sandboxing and permission control
- **PyBindings**: C++ to Python binding utilities
- **Typing Support**: Type annotations parsing, typed PyIR, runtime typing module

## Extension Points
- Add new built-in functions in `PyVM::RegisterBuiltins()`
- Extend value types by modifying `PyValue` class
- Implement new Python features in `PyCompiler` or `PyVM`
- Add sandboxing rules in `PolicyKit`

## Conventions
- Use proper U++ types (`int64`, `double`, `String`) instead of native C++ primitives
- Use `RecyclerPool` for high-frequency value allocations (see `ai/rules/memory-pooling.md`)
- Document all public APIs with Topic++ (`.tpp`) style comments
- Keep test files in this directory (`.py` files for interpreter testing)

## Dependencies
- Core: Basic U++ functionality
- Core/TextParsing: Text parsing utilities
- ByteVM: VM infrastructure

## Where To Start
- Read `uppsrc/ByteVM/Python/PyValue.cpp` to understand value operations
- Examine `uppsrc/ByteVM/Python/PyVM.cpp` for execution engine logic
- Study `uppsrc/ByteVM/Python/PyCompiler.cpp` for Python compilation flow
- Look at `uppsrc/ByteVM/Python/PolicyKit.h` for sandboxing model

## Typing Support (Python 3.5+ type hints)

### Parser (`PyCompiler.cpp`)
- Function parameter annotations: `def f(x: int, y: str) -> bool:`
- Return type annotations: `def g() -> List[int]:`
- Variable annotations: `x: int = 5`, `y: List[str]`
- Annotations parsed in enclosing scope, attached to function objects via `PY_ANNOTATE_FUNC` opcode
- Variable annotations stored in scope `__annotations__` dict via `PY_ANNOTATE_SCOPE` opcode

### PyIR Opcodes (`PyCode.h`)
- `PY_ANNOTATE_SCOPE = 206`: Store variable annotation in current scope's `__annotations__`
- `PY_ANNOTATE_FUNC = 207`: Attach annotation to function object's `PyLambda.annotations`

### VM Runtime (`PyVM.cpp`)
- `PY_ANNOTATE_SCOPE` handler: writes to module/frame `__annotations__` dict
- `PY_ANNOTATE_FUNC` handler: writes to `PyLambda.annotations` VectorMap
- `PY_LOAD_ATTR` on functions: returns `__annotations__` dict
- `PY_BINARY_SUBSCR` on typing forms: enables `typing.List[int]` → `typing.List`
- `InitBuiltins()`: registers `typing` module with forms (Any, Optional, Union, List, Dict, Tuple, Set, FrozenSet, Callable, TypeVar, Generic, Annotated, ClassVar, Final, NoReturn, Text, AnyStr)

### PyValue Extensions (`PyValue.h`)
- `PyLambda.annotations`: `VectorMap<String, PyValue>` storing function annotations by name

### Testing
- `test_typing.py`: covers all annotation forms, typing module, introspection