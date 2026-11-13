# Core/Ext

`Core/Ext` is BazaarCore's consolidated extension package for utilities that are independent of upstream Core internals. It stays a single subpackage for the current assembly layout; BazaarCore remains a patched foundation SDK rather than a normal consumer nest.

## Package Map

- `Ext.h` is included by the patched Core umbrella and aggregates extension headers.
- `Ext.upp` groups headers and BLITZ sources. Preserve current include order and namespace scope when editing `Ext.h`.
- `AppExt` provides command-line, share/local-file, MLUI runtime, and data-directory wrappers.
- `ThreadPool`, `ConvertExt`, `FormatExt`, `GeomExt`, and `PathExt` contain extracted helper APIs.
- YAML and TOON support live here and must not require a `Value` YAML vtable hook in base Core.
- `ExtUtil` is older code with overlapping utility names; check it before adding helpers.

## Patch Relationship

Tracked `patches/*.patch` files are the source used to materialize `applied-patches` and `patched-uppsrc`. The generated trees are outputs, not edit targets. `Core.upp` and `Core.h` intentionally connect base Core to this package; the remaining platform and compatibility changes must stay explicit in the surgical patch set.

## Validation

Build consumers through UppBuilder after materialization, including one headless downstream package. Test moved API behavior directly, especially YAML Value/container round trips and helper behavior that depends on platform flags. Keep Task 29 and the architecture note aligned with the exact retained patch inventory.
