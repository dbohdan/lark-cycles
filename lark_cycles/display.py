"""
Tkinter display ported from graphic.c and manager.c.
"""

import tkinter as tk

COLOURS = [
    "#ff0000",  # Red    (player 0)
    "#0000ff",  # Blue   (player 1)
    "#ffffff",  # White  (player 2)
    "#00ff00",  # Green  (player 3)
    "#ffff00",  # Yellow (player 4)
]

BASE_CELL_SIZE = 12
BASE_SCOREBOARD_HEIGHT = 30


class GameDisplay:
    def __init__(self, root, engine, scale=1):
        self.root = root
        self.engine = engine
        self.scale = scale
        self.trail_items = []
        self.flash_item = None

        self.cell_size = BASE_CELL_SIZE * scale
        self.scoreboard_height = BASE_SCOREBOARD_HEIGHT * scale
        self.arena_width = 40 * self.cell_size
        self.arena_height = 23 * self.cell_size

        self.canvas = tk.Canvas(
            root,
            width=self.arena_width,
            height=self.arena_height + self.scoreboard_height,
            bg="black",
        )
        self.canvas.pack()

        self._draw_scoreboard()

    def _draw_scoreboard(self):
        self.canvas.create_rectangle(
            0, 0, self.arena_width, self.scoreboard_height, fill="black", outline=""
        )
        self.canvas.create_text(
            10 * self.scale,
            5 * self.scale,
            anchor="nw",
            text="Cycles",
            fill="#888888",
            font=("Courier", 12),
        )

    def draw_scores(self):
        self.canvas.delete("score")
        y = 5 * self.scale
        x = 60 * self.scale
        self.canvas.create_text(
            x,
            y,
            anchor="nw",
            text="T:{}".format(self.engine.ties),
            fill="white",
            font=("Courier", 10),
            tag="score",
        )
        x += 45 * self.scale
        for i in range(5):
            c = self.engine.get_cycle(i)
            if c:
                text = "{} W{} L{}".format(c.name[:5], c.wins, c.losses)
            else:
                text = "---"
            self.canvas.create_text(
                x,
                y,
                anchor="nw",
                text=text,
                fill=COLOURS[i],
                font=("Courier", 10),
                tag="score",
            )
            x += 75 * self.scale

    def draw_trails(self):
        for x1, y1, x2, y2, colour in self.engine.new_trails:
            sx1 = x1 * self.cell_size + self.cell_size // 2
            sy1 = y1 * self.cell_size + self.cell_size // 2 + self.scoreboard_height
            sx2 = x2 * self.cell_size + self.cell_size // 2
            sy2 = y2 * self.cell_size + self.cell_size // 2 + self.scoreboard_height
            item = self.canvas.create_line(
                sx1, sy1, sx2, sy2, fill=COLOURS[colour], width=2
            )
            self.trail_items.append(item)

    def flash_colour(self, colour):
        if self.flash_item:
            self.canvas.delete(self.flash_item)
        self.flash_item = self.canvas.create_rectangle(
            0,
            self.scoreboard_height,
            self.arena_width,
            self.arena_height + self.scoreboard_height,
            fill=COLOURS[colour],
            stipple="gray50",
        )
        self.root.after(200, self._clear_flash)

    def _clear_flash(self):
        if self.flash_item:
            self.canvas.delete(self.flash_item)
            self.flash_item = None

    def clear_arena(self):
        for item in self.trail_items:
            self.canvas.delete(item)
        self.trail_items = []
        if self.flash_item:
            self.canvas.delete(self.flash_item)
            self.flash_item = None
