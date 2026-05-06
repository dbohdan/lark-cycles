# Cycles Porting Log

## Overview
Port of Rico Mariana's 1988 Amiga game "Cycles" to Python 3, Tkinter, and
the Starlark Python API described in `starlark.md`.

## Architecture
- **Single-process**: Removed Amiga Exec message ports and separate player
  processes. The server and all AIs run in one Python process.
- **Tkinter display**: Replaced Amiga Intuition/Graphics with a Tkinter Canvas.
  Arena is 40×23 cells drawn at 12×12 pixels each.
- **Timer**: Replaced `timer.device` with Tkinter `after(TICK_MS)`.
- **Starlark strategies**: Each AI player is a `.star` file loaded with
  `starlark.exec_file`. Predeclared builtins inject the player's current state.

## Preserved Logic (from cycles.c / dobikes.c / cycles.h)
- Grid size 40×23, max 5 players/colours.
- Movement order: snapshot `na = num_alive` at tick start; rotate cycle list
  each tick; process cycles sequentially.
- Collision: a cycle crashes if its target cell is occupied (wall, trail, or
  another cycle that already moved this tick). This means move order matters.
- Winner declared only when `na == 1` at tick start. If multiple cycles crash
  leaving zero alive, it is a tie.
- Trails persist for the entire round and are cleared on restart.
- Initial spawn positions are random within `rand()%(X_SIZE-10)+5` and
  `rand()%(Y_SIZE-10)+5`.

## Starlark Adaptations
- `GetInfo(&x,&y,&dir)` became `get_info()` returning a `(x, y, dir)` tuple.
- `Look(dir)` / `Inquire(x,y)` return Starlark `int` (0 or 1). They were
  initially exposed as `bool`, but Starlark forbids `int + bool` arithmetic,
  so the bridge now explicitly casts results with `int()`.
- `TURN_LEFT` / `TURN_RIGHT` macros became predeclared functions
  `turn_left(d)` / `turn_right(d)`.
- **No recursion in Starlark**: Players 6 and 7 originally used recursive DFS
  to evaluate open space. They were rewritten with manually unrolled nested
  `for` loops (depth 2 and 3) to stay within Starlark's "no recursion/no
  while" constraints.
- **No `while` loops**: The iterative DFS was replaced by fixed-depth nested
  `for` loops, which is valid Starlark.

## Assumptions
- The Starlark implementation does **not** allow calling a `StarlarkFunction`
  directly from Python. The bridge uses `starlark.eval.evaluator.call()` and
  must wrap the call in `with_thread(self.module.thread)` and
  `with_mutability(self.module.mutability)` context managers.
- Human player (original player8 / joystick) is implemented natively in Python
  as `HumanPlayer` bound to Tkinter key events (arrows + WASD).

## Colour Mapping
Amiga RGB4 pens mapped to Tkinter hex:
- Player 0 (Red):    `#ff0000`
- Player 1 (Blue):   `#0000ff`
- Player 2 (White):  `#ffffff`
- Player 3 (Green):  `#00ff00`
- Player 4 (Yellow): `#ffff00`

## Debugging and Fixes

### StarlarkFunction not callable
Directly invoking `module.globals["strategy"]()` from Python raised
`'StarlarkFunction' object is not callable`. Fixed by importing `call` from
`starlark.eval.evaluator` and using `call(self.strategy_fn, [], {},
self.module.thread)` within `with_thread` and `with_mutability` context
managers.

### int + bool unsupported
Player strategies that accumulate inquiry results (e.g. `score += inquire(...)`)
crashed with "unsupported binary operation: int + bool". The bridge's
`_look()` and `_inquire()` methods were returning Python `bool` values.
Explicitly wrapping the return values with `int()` resolves the issue because
Starlark treats the result as plain `int`.

### Player label overlap and short names
The scoreboard initially showed truncated/default names like "Playe W0"
because `StarlarkPlayer.name` fell back to the filename stem. Added a
`short_name` global (max 5 characters, e.g. `short_name = "Right"`) to each
`.star` file and updated the bridge to read it from `module.globals`. This
gives every AI a unique, human-readable label and prevents Tkinter text
overlap.

### Display scaling CLI option
Added `-s M` / `--scale M` to `cycles.py`. The `GameDisplay` multiplies
`BASE_CELL_SIZE` and scoreboard layout coordinates by `M` while keeping font
sizes unchanged. This lets users enlarge the arena to avoid label overlap on
high-DPI displays or small screens.

## Files Created
- `cycles.py` – Main entry point / Tkinter app loop.
- `engine.py` – Game state, movement, collision, scoring.
- `display.py` – Tkinter Canvas rendering and scoreboard.
- `starlark_bridge.py` – `StarlarkPlayer` and `HumanPlayer` classes.
- `players/player{1..7}.star` – Ported AI strategies.
