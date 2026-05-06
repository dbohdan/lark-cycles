"""
Bridge to load and run Starlark player strategies.
"""

import os
import starlark

from starlark.eval import BuiltinFunction
from starlark.eval.builtins import with_mutability, with_thread
from starlark.eval.evaluator import call

from .engine import (
    DIR_UP,
    DIR_RIGHT,
    DIR_DOWN,
    DIR_LEFT,
    xdir,
    ydir,
)


class StarlarkPlayer:
    def __init__(self, engine, filepath):
        self.engine = engine
        self.filepath = filepath
        self.name = os.path.splitext(os.path.basename(filepath))[0]
        self.colour = -1
        self.x = 0
        self.y = 0
        self.dir = DIR_RIGHT
        self.status = 0
        self.wins = 0
        self.losses = 0

        with open(filepath, "r") as f:
            source = f.read()

        predeclared = self._make_predeclared()
        self.module = starlark.exec_file(
            source,
            filename=filepath,
            predeclared=predeclared,
            max_steps=1_000_000,
        )
        if "short_name" in self.module.globals:
            self.name = self.module.globals["short_name"]
        self.strategy_fn = self.module.globals["strategy"]

    def _make_predeclared(self):
        return {
            "get_info": BuiltinFunction(name="get_info", impl=self._get_info),
            "look": BuiltinFunction(name="look", impl=self._look),
            "inquire": BuiltinFunction(name="inquire", impl=self._inquire),
            "turn_left": BuiltinFunction(name="turn_left", impl=lambda d: (d - 1) & 3),
            "turn_right": BuiltinFunction(
                name="turn_right", impl=lambda d: (d + 1) & 3
            ),
            "DIR_UP": DIR_UP,
            "DIR_RIGHT": DIR_RIGHT,
            "DIR_DOWN": DIR_DOWN,
            "DIR_LEFT": DIR_LEFT,
            "xdir": tuple(xdir),
            "ydir": tuple(ydir),
        }

    def _get_info(self):
        return (self.x, self.y, self.dir)

    def _look(self, direction):
        nx = self.x + xdir[direction]
        ny = self.y + ydir[direction]
        return int(self.engine.inquire(nx, ny))

    def _inquire(self, x, y):
        return int(self.engine.inquire(x, y))

    def get_direction(self):
        try:
            with (
                with_mutability(self.module.mutability),
                with_thread(self.module.thread),
            ):
                result = call(self.strategy_fn, [], {}, self.module.thread)
        except Exception as e:
            print("Starlark error in {}: {}".format(self.name, e))
            return self.dir
        if result not in (DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT):
            return self.dir
        return result


class HumanPlayer:
    def __init__(self, name):
        self.name = name
        self.colour = -1
        self.x = 0
        self.y = 0
        self.dir = DIR_RIGHT
        self.status = 0
        self.wins = 0
        self.losses = 0
        self.pending_dir = None

    def on_key(self, key):
        mapping = {
            "Up": DIR_UP,
            "Down": DIR_DOWN,
            "Left": DIR_LEFT,
            "Right": DIR_RIGHT,
            "w": DIR_UP,
            "s": DIR_DOWN,
            "a": DIR_LEFT,
            "d": DIR_RIGHT,
        }
        if key in mapping:
            self.pending_dir = mapping[key]

    def get_direction(self):
        if self.pending_dir is not None:
            self.dir = self.pending_dir
