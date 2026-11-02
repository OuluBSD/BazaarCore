# AGENTS - Manager Package

**For**: AI agents working on `uppsrc/Manager/`

A CLI task & phase tracker. Built into `bin/<method>/Manager`. Stores data
as JSON files under the **caller's CWD** at `./mgr/**/*.json`. This package
is local-only; concepts are designed to scale toward GitHub-style
issues/PRs/orgs/users later.

## Build

```
bin/build -m CLANG Manager
```

Binary lands at `bin/CLANG/Manager`.

## File Map

| File | Role |
|------|------|
| `Manager.upp` | Package manifest (`uses Core` only) |
| `Manager.h`   | Main header — wraps everything in `NAMESPACE_UPP` |
| `Task.h/.cpp` | `Project`, `Phase`, `Task` domain structs + `Jsonize` |
| `Store.h/.cpp` | Project-scoped filesystem persistence under `./mgr/projects/` |
| `Commands.h/.cpp` | `add`, `remove`, `list`, `set` for `project`/`phase`/`task` |
| `GitCommands.h/.cpp` | `git commit`, `git pull`, `git push` wrappers |
| `main.cpp` | `CONSOLE_APP_MAIN` at global scope, arg parsing |

## CLI Grammar

Hierarchy: `project → phase → task` (all required).

```
Manager add    project <title> [field value ...]
Manager add    phase   <project> <title> [field value ...]
Manager add    task    <project> <phase> <title> [field value ...]
Manager remove project <slug|title>
Manager remove phase   <project> <idx|title>
Manager remove task    <project> <phase> <idx|title>
Manager list   project
Manager list   phase   <project>
Manager list   task    <project> [<phase>]
Manager set    project <slug|title>            <field> <value>
Manager set    phase   <project> <idx|title>   <field> <value>
Manager set    task    <project> <phase> <idx|title> <field> <value>
Manager print  project <slug|title>
Manager print  phase   <project> <idx|title>
Manager print  task    <project> <phase> <idx|title>
Manager next   task    <project>          # first 'open' task, exit 2 when none
Manager git    {commit,pull,push}
```

`print` shows every field of a single item (unset values print as `(empty)`,
`link-next-task` as `none`), with field names matching the set-grammar keys
so output greps 1:1. `list` is the overview and skips empty fields.

`next task` is the worker-side queue pop: the first task with status `open`
(phases in seq order, tasks in seq order inside each phase), printed in the
same full format as `print task`. Exit codes: 0 = a task was printed,
2 = the queue is empty (scriptable branch), 1 = usage error.

Project fields: `title`, `description`, `status`.
Phase fields:   `title`, `description`, `status`.
Task fields:    `title`, `description`, `details`, `instructions`, `result`,
                `status`, `link-next-task`, `phase`.

`instructions` is the planner → worker direction (what to do); `result` is
the worker → planner direction (outcome + verification evidence). Together
with `status` (`open` / `in-progress` / `done` / `blocked`) they form the
planner-worker channel — see `ai/skills/manager-planner-worker-loop.md`.

## Conventions

- Projects, phases, tasks each have **independent sequences per project**
  (projects use slug-as-identity, no numeric seq).
- Sequence numbers are encoded as the file-name prefix `NNN-slug.json`.
- Default `link-next-task` = `seq + 1` (within the same project);
  overridable via `set ... link-next-task`.
- `./mgr/` is created on demand in the **current working directory** — never
  in the repo where the Manager binary itself lives unless invoked there.
- `Manager git commit` stages **only** `./mgr/` and auto-generates the
  commit message from the diff summary.
- Removing a phase is refused while tasks still belong to it (the
  `still has tasks` guard).
- Removing a project recursively deletes its whole subtree.
- The project slug (derived from the title at creation) is the directory
  name and the stable identity. Renaming a project's title does not rename
  the slug/directory.

## Out Of Scope (for now)

- GitHub/GitLab remote sync, issues API, pull requests, organizations,
  users, comments, labels, milestones.
- Any GUI (per `headless-gui-dual-purpose.md`, if a GUI is ever added it
  must be a thin twin over the same domain model — the CLI stays primary).

## Generic Rules

See `ai/rules/` (top of repo) — especially `header-include-policy.md`,
`naming-conventions.md`, `logging-rules.md`,
`assertion-and-fix-it-now.md`. Generic rules are not duplicated here.
