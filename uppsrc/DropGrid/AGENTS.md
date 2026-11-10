# DropGrid Package Guide

**Parent Assembly**: `OuluBSD/BazaarCore`  
**Namespace**: `Upp`  
**Dependencies**: `CtrlLib`, `GridCtrl`

---

## 1. Package Purpose

`DropGrid` provides a rich dropdown control that displays a tabular popup grid (`GridCtrl`) instead of a simple linear list. It combines the editing, searching, key-mapping, and multi-column presentation capabilities of `GridCtrl` with standard dropdown and multibutton input paradigms.

---

## 2. File Map

| File | Purpose |
| :--- | :--- |
| `DropGrid.h` | Class declarations (`DropGrid`, nested `PopUpGrid`), events, and API macros |
| `DropGrid.cpp` | Dropdown popup management, keyboard handling, column indexing, search filtering, and paint routines |
| `DropGrid.usc` | TheIDE layout designer visual preview and property sheet declaration |

---

## 3. Key Concepts & Architecture

1. **`PopUpGrid`**:
   - Derived from `GridCtrl`.
   - Manages popup lifecycle (`PopUp()`, `Deactivate()`, `CloseData()`, `CloseNoData()`).
   - Hooks into `WhenLeftClick`, `WhenEnter`, `WhenEscape`, and `WhenClose`.

2. **Column Semantics**:
   - `AddColumn(name, width)`: Defines visible tabular columns in the dropdown.
   - `AddIndex(id)`: Defines internal non-rendered index/key values.
   - `SetKeyColumn(n)`: Sets which column provides the primary key Value returned by `GetData()`.
   - `SetValueColumn(n)` / `AddValueColumn(n)`: Determines which column(s) are used for display formatting and text rendering.
   - `SetFindColumn(n)`: Controls which column is matched against incremental type-ahead searches.

3. **MultiButton Integration**:
   - Embeds a `MultiButtonFrame` (`drop`).
   - Provides helper methods `AddSelect`, `AddPlus`, `AddEdit`, and `AddClear`.
