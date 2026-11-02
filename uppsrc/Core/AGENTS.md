# Core Package AGENTS Guide

## Scope
- Applies to `uppsrc/Core` and its entire sub-tree of U++ packages.

## Purpose
- Fundamental U++ core library providing essential infrastructure
- Defines basic types, memory management, containers, and utilities
- Provides the foundation for all other U++ packages
- Used by virtually all U++ applications and packages

## Key Areas
- **Basic Types**: Fundamental data types (String, int64, double, etc.)
- **Memory Management**: Allocators, memory pools, and smart pointers
- **Containers**: Vector, Array, Map, and other container classes
- **Utilities**: String operations, formatting, file I/O, and helpers
- **Threading**: Thread management, synchronization, and concurrency
- **Serialization**: Data serialization and deserialization
- **Error Handling**: Exception handling and error reporting
- **Configuration**: Application configuration and settings management

## Extension Points
- Add new container types by following container conventions
- Implement custom memory allocators for specialized needs
- Extend string operations with new utilities
- Add new serialization formats
- Extend threading primitives with custom synchronization

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
- (None - this is the base package)
- All other U++ packages depend on Core
- **Subpackage Decoupling & Ecosystem Splitting Plan**: See [`ai/info/core-subpackage-decoupling.md`](../../ai/info/core-subpackage-decoupling.md) for the active migration plan decoupling `uppsrc/Core/*` subpackages into domain repositories under OuluBSD / UppHub.
- **Patched-Uppsrc & Single-Subpackage Consolidation**: See [`ai/info/bazaarcore-and-patched-uppsrc-architecture.md`](../../ai/info/bazaarcore-and-patched-uppsrc-architecture.md). To maximize BLITZ build performance, all loose extensions in `uppsrc/Core` are consolidated into `uppsrc/Core/Ext`, and base `Core.h` receives only surgical inclusion hooks.

## Where To Start
- Review `Core.h` — the umbrella aggregating all headers
- Study `String.h` for the string implementation
- Look at `Map.h` / `Index.h` / `InVector.h` for container implementations
- Check `CoWork.h` / `Mt.h` for threading primitives

## Design Principles
- Minimal dependencies and maximum portability
- High performance with low overhead
- Extensible architecture for specialized needs
- Consistent API design across all components
- Comprehensive error handling and reporting

