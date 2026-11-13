# Core/Ext Package Guide

## Purpose
This package contains independent helpers and utility modules that were historically mixed into base Core. It remains the single `Core/Ext` package so BazaarCore can provide a patched foundation assembly while keeping Core changes explicit and reviewable.

## Structure
- `Ext.h` is the package umbrella and includes Core before its extension headers.
- Package headers are listed in `Ext.upp` and aggregated through `Ext.h`; avoid redundant includes in subheaders unless standalone inclusion is required.
- Package implementation files include `Ext.h` first and keep exported APIs in `namespace Upp`.
- Keep the package BLITZ-compatible and guard platform-specific behavior.

## Core Boundary
- Prefer free functions and standalone classes here over adding more members or vtable slots to upstream Core classes.
- Retain Core patches only for class-private state, Core lifecycle integration, or target-platform configuration that cannot be expressed in an extension.
- Check upstream Core and the sibling extension headers before adding an API to avoid duplicate definitions.
- Tracked patches under `patches/` are the source for generated `applied-patches` and `patched-uppsrc`. Do not hand-edit generated trees as source.

## Validation
- Build representative consumers through UppBuilder; compiling this subpackage alone is not sufficient.
- Validate helper extraction with targeted runtime tests where possible.
- Keep Task 29 and architecture notes aligned with the actual patch inventory.
