# BazaarCore

BazaarCore is the foundation assembly and core infrastructure tier for the Ultimate++ (U++) ecosystem.

## Dependency Hierarchy

Part of the **OuluBSD U++ Modularization Architecture**:
```
UppBootstrap  <--  AdvancedBuilder (UppBuilder)  <--  BazaarCore  <--  IDE (DevEnv)
```
1. **`UppBootstrap`**: Zero-dependency toolchain bootstrap (`umk` compiler & build methods).
2. **`AdvancedBuilder` (`UppBuilder`)**: Meta-builder with 3-pillar bridge, multi-assembly chaining, deterministic config keys.
3. **`BazaarCore`** (this repo): Foundation assembly (`patched-uppsrc`), single amalgamated `Core/Ext`, and Tier 3 packages (`Docking`, `Terminal`, `Manager`...).
4. **`IDE` (`DevEnv`)**: Modular developer environment application built on top of `BazaarCore` via `UppBuilder`.

## Overview

BazaarCore establishes a clean separation between pristine upstream U++ (`ultimatepp`) and ecosystem extensions:
- **`uppsrc/Core/Ext`**: Single consolidated extension subpackage amalgamating all core algorithms (TOON, YAML, CritBitIndex, AppRegistry, CommandRegistry, RecyclerPool, Process, CmdArgs, etc.) into a single-pass BLITZ compile unit (`$blitz.cpp`).
- **13 Subpackage Shims**: Backward-compatible forwarding shims for `Core/{MetaTraits, DataStructures, ProcessUtil, MathNumeric, CompatExt, ColorGeom, CryptoExt, Cuboid, TextParsing, MluiScript, Repl, AiCommon, VfsValueLegacy}`.
- **Foundation Infrastructure (Tier 3)**:
  - `uppsrc/Terminal`: Embedded VT100 / XTerm terminal widget (libvterm-backed).
  - `uppsrc/PtyProcess`: Cross-platform pseudoterminal process execution.
  - `uppsrc/DropGrid`: Multi-column popup grid dropdown control with key-value binding, search filtering, and multibutton frame.
  - `uppsrc/PlotCtrl` & `uppsrc/PlotLib`: Advanced 2D graphing and plotting controls with headless PNG export.
  - `uppsrc/Ftp` & `uppsrc/FtpBrowser`: Modular headless FTP client/server engine (`Ftp`) and GUI browser component (`FtpBrowser`).
  - `uppsrc/QtfReader`: Text extraction engine for U++ QTF formatted documents.
  - `uppsrc/DebuggersCore` & `uppsrc/dbg`: Headless GDB, LLDB, and Visual Studio debugger runner and session abstractions.
  - `uppsrc/TuiDesktop`, `uppsrc/VirtualGui3D`, `uppsrc/NetDpy`: Terminal UI desktop, virtual framebuffers, and networked display servers.
  - `uppsrc/ByteVM`: Fast register-based bytecode virtual machine.
  - `uppsrc/Docking`: Advanced multi-docking window framework.
  - `uppsrc/Manager`: High-speed local project, phase, and task tracker.
  - `uppsrc/Skylark`: Native U++ web application server.
  - `uppsrc/AMP`: Asynchronous message-passing network engine.
  - `uppsrc/Turtle` & `uppsrc/VirtualGui`: Virtual GUI framebuffers and headless backends.
- **Surgical Patch Suite**:
  - `patches/Core.patch`: Minimal 26-line surgical diff injecting `#include <Core/Ext/Ext.h>` into upstream `Core.h` and `uses Core/Ext;` into `Core.upp`.

## Assembly Materialization

BazaarCore uses **`uppbuilder`** (`OuluBSD/UppBuilder`) to materialize the staged assemblies from upstream:
```bash
# Materialize core patched-uppsrc assembly:
uppbuilder --apply-patches -v

# Materialize all upstream assemblies (uppsrc, reference, tutorial, examples, upptst, autotest):
uppbuilder materialize all

# Build upstream examples or tutorials directly:
uppbuilder examples/Bombs
uppbuilder reference/ArrayCtrl
uppbuilder tutorial/Gui01
uppbuilder autotest/Bits
```

### Staged Upstream Assemblies (`applied-*`)
- **`patched-uppsrc/`** / **`applied-patches/`**: Pristine upstream `uppsrc` with surgical hooks and restructured `Ctrl/Core` & `Ctrl/Lib`.
- **`applied-reference/`**: 200+ modernized upstream reference packages (`reference/*`).
- **`applied-tutorial/`**: 55+ modernized upstream tutorial packages (`tutorial/*`).
- **`applied-examples/`**: 40+ modernized upstream demonstration applications (`examples/*`).
- **`applied-upptst/`**: 190+ modernized upstream interactive/regression test suites (`upptst/*`).
- **`applied-autotest/`**: 200+ modernized upstream automated unit test suites (`autotest/*`).

### Global Configuration (`~/.config/uppbuilder/config.json`)
```json
{
  "upstream": "/path/to/ultimatepp/uppsrc",
  "patched_uppsrc": "/path/to/BazaarCore/patched-uppsrc"
}
```

## License
BSD 2-Clause License. See [LICENSE.md](LICENSE.md).
