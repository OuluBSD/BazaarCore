# AGENTS - BazaarCore Guide

**Scope**: Applies to the root of `OuluBSD/BazaarCore` and all subpackages within.

## Architecture Tenets
1. **Pristine Upstream Untouched**: Upstream `ultimatepp/uppsrc` is strictly read-only.
2. **Single Subpackage Rule**: All extensions to `Core` belong in `uppsrc/Core/Ext` for single-pass BLITZ compilation. Do not create granular subpackages under `Core/`.
3. **Surgical Patches Only**: Upstream modifications live in `patches/` and must remain minimal hooks.
4. **Build Driver**: Always build using `uppbuilder` (`bin/uppbuilder`).

## Subpackage Map
- `uppsrc/Core/Ext`: The unified home for Core extras.
- `uppsrc/Terminal`, `PtyProcess`: Developer console infrastructure.
- `uppsrc/ByteVM`: Dialect VM (Python / JS / Bytecode).
- `uppsrc/Docking`: Desktop docking layout.
- `uppsrc/Manager`: Project tracking engine.
- `uppsrc/Skylark`, `AMP`, `Turtle`, `VirtualGui`: Headless services and virtual GUI.
