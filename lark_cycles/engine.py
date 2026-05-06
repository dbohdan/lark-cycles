"""
Game engine ported from cycles.c, dobikes.c, and cycles.h.
"""

import random

# Constants from cycles.h
STATUS_NEED_NEW = 0
STATUS_GOT_MOVE = 1
STATUS_DEAD = 100
STATUS_LOSER = 101
STATUS_WINNER = 102
STATUS_AWAITING = 103
STATUS_REMOVED = 104

ORD_DIRECTION = 0
ORD_AWAIT = 1
ORD_RETIRE = 2

DIR_UP = 0
DIR_RIGHT = 1
DIR_DOWN = 2
DIR_LEFT = 3

MAX_COLOURS = 5
X_SIZE = 40
Y_SIZE = 23

xdir = [0, 1, 0, -1]
ydir = [-1, 0, 1, 0]


class GameEngine:
    def __init__(self, players=None):
        self.cycles = []
        self.scores = [0] * MAX_COLOURS
        self.ties = 0
        self.num_players = 0
        self.num_alive = 0
        self.num_waiting = 0
        self.occupied = set()
        self.trails = []
        self.new_trails = []
        self.tie_this_tick = False
        self.win_this_tick = False
        self.win_colour = 0

        if players:
            for p in players:
                self.add_player(p)

    def add_player(self, player):
        if len(self.cycles) >= MAX_COLOURS:
            return False
        used = {c.colour for c in self.cycles}
        colour = 0
        while colour < MAX_COLOURS and colour in used:
            colour += 1
        if colour >= MAX_COLOURS:
            return False
        player.colour = colour
        player.x = 0
        player.y = 0
        player.dir = DIR_RIGHT
        player.status = STATUS_LOSER
        self.cycles.append(player)
        self.num_players = len(self.cycles)
        self.scores[colour] = 0
        return True

    def remove_player(self, player):
        if player in self.cycles:
            if player.status < STATUS_DEAD:
                self.num_alive -= 1
            self.cycles.remove(player)
            self.num_players = len(self.cycles)

    def restart_round(self):
        self.occupied.clear()
        self.trails = []
        self.new_trails = []
        self.tie_this_tick = False
        self.win_this_tick = False
        self.num_waiting = 0
        self.num_alive = 0
        for c in self.cycles:
            if c.status == STATUS_REMOVED:
                continue
            c.x = random.randint(5, X_SIZE - 6)
            c.y = random.randint(5, Y_SIZE - 6)
            c.dir = DIR_RIGHT
            c.status = STATUS_NEED_NEW
            self.num_alive += 1
            self._occupy(c.x, c.y)

    def _occupy(self, x, y):
        self.occupied.add((x, y))

    def _is_occupied(self, x, y):
        if x < 0 or y < 0 or x >= X_SIZE or y >= Y_SIZE:
            return True
        return (x, y) in self.occupied

    def alive_players(self):
        return [c for c in self.cycles if c.status < STATUS_DEAD]

    def move_cycles(self):
        self.new_trails = []
        self.tie_this_tick = False
        self.win_this_tick = False
        na = self.num_alive
        if not self.cycles:
            return

        # Rotate list: move first to end (as in original dobikes.c)
        first = self.cycles.pop(0)
        self.cycles.append(first)

        for c in self.cycles:
            if c.status not in (STATUS_GOT_MOVE, STATUS_NEED_NEW):
                continue

            old_x, old_y = c.x, c.y
            c.x += xdir[c.dir]
            c.y += ydir[c.dir]

            if self._is_occupied(c.x, c.y):
                c.status = STATUS_LOSER
                self.num_alive -= 1
                if self.num_alive == 0:
                    self.ties += 1
                    self.tie_this_tick = True
            else:
                if na > 1:
                    c.status = STATUS_NEED_NEW
                else:
                    self.scores[c.colour] += 1
                    c.status = STATUS_WINNER
                    self.num_alive -= 1
                    self.win_this_tick = True
                    self.win_colour = c.colour
                self._occupy(c.x, c.y)
                self.trails.append((old_x, old_y, c.x, c.y, c.colour))
                self.new_trails.append((old_x, old_y, c.x, c.y, c.colour))

    def round_over(self):
        return self.num_alive == 0

    def should_continue(self):
        return self.num_players >= 2

    def get_cycle(self, colour):
        for c in self.cycles:
            if c.colour == colour:
                return c
        return None

    def inquire(self, x, y):
        return self._is_occupied(x, y)

    def look(self, colour, direction):
        c = self.get_cycle(colour)
        if not c:
            return True
        nx = c.x + xdir[direction]
        ny = c.y + ydir[direction]
        return self._is_occupied(nx, ny)
