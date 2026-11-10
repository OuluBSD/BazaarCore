# DropGridExample Package Guide

**Parent Assembly**: `OuluBSD/BazaarCore`  
**Namespace**: `Upp`  
**Dependencies**: `CtrlLib`, `GridCtrl`, `DropGrid`

---

## 1. Package Purpose

`DropGridExample` is a runnable GUI application demonstrating the integration of `DropGrid` widgets within a `TopWindow`.
It showcases:
- Adding tabular columns (ID, Description, Category, Quantity).
- Adding selection buttons (`AddClear()`, `AddSelect()`).
- Data population and change event hooks (`WhenAction`).
- `--test` command line flag for non-interactive smoke testing in CI pipelines.
