#!/usr/bin/env python3
"""
Lark Cycles - Python/Tkinter/Starlark port of Rico Mariana's 1988 Amiga game
Warrior Cycles.
"""

import argparse
import tkinter as tk
import os

from engine import (
    GameEngine,
    STATUS_NEED_NEW,
    STATUS_GOT_MOVE,
    STATUS_AWAITING,
    STATUS_LOSER,
    STATUS_WINNER,
)
from display import GameDisplay
from starlark_bridge import StarlarkPlayer, HumanPlayer

TICK_MS = 100


class CyclesApp:
    def __init__(self, root, player_files, scale=1):
        self.root = root
        self.root.title("Lark Cycles")

        self.engine = GameEngine()
        self.display = GameDisplay(root, self.engine, scale=scale)

        self.players = []
        for filepath in player_files:
            if filepath == "human":
                p = HumanPlayer("Human")
            else:
                p = StarlarkPlayer(self.engine, filepath)
            self.players.append(p)
            self.engine.add_player(p)

        self.root.bind("<Key>", self._on_key)

        self._start_game()

    def _on_key(self, event):
        for p in self.players:
            if isinstance(p, HumanPlayer):
                p.on_key(event.keysym)

    def _start_game(self):
        self.engine.restart_round()
        self.display.draw_scores()
        self._tick()

    def _tick(self):
        if not self.engine.should_continue():
            return

        for p in self.engine.alive_players():
            if p.status in (STATUS_NEED_NEW, STATUS_GOT_MOVE):
                new_dir = p.get_direction()
                p.dir = new_dir
                p.status = STATUS_GOT_MOVE

        self.engine.move_cycles()
        self.display.draw_trails()

        if self.engine.tie_this_tick:
            self.display.flash_colour(0)
            self.display.draw_scores()
        elif self.engine.win_this_tick:
            self.display.flash_colour(self.engine.win_colour)
            self.display.draw_scores()

        if self.engine.round_over():
            for p in self.engine.cycles:
                if p.status == STATUS_LOSER:
                    p.losses += 1
                elif p.status == STATUS_WINNER:
                    p.wins += 1
                p.status = STATUS_AWAITING

            self.display.draw_scores()
            self.root.after(500, self._restart_round)
            return

        self.root.after(TICK_MS, self._tick)

    def _restart_round(self):
        self.engine.restart_round()
        self.display.clear_arena()
        self.display.draw_scores()
        self._tick()


def main():
    parser = argparse.ArgumentParser(
        description="Lark Cycles - Python/Tkinter/Starlark port "
        "of Rico Mariana's 1988 Amiga game Warrior Cycles."
    )
    parser.add_argument(
        "-s",
        "--scale",
        type=int,
        default=1,
        metavar="M",
        help="Draw pixels M times the size (default: 1)",
    )
    args = parser.parse_args()

    if args.scale < 1:
        parser.error("scale must be at least 1")

    player_dir = os.path.join(os.path.dirname(__file__), "players")
    player_files = []
    for i in range(1, 8):
        path = os.path.join(player_dir, "player{}.star".format(i))
        if os.path.exists(path):
            player_files.append(path)

    if len(player_files) < 5:
        player_files.append("human")

    root = tk.Tk()
    app = CyclesApp(root, player_files, scale=args.scale)
    root.mainloop()


if __name__ == "__main__":
    main()
