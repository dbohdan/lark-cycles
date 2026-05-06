# Vertigo

short_name = "Verti"

def strategy():
    x, y, dir = get_info()
    if dir % 2 == 1:
        if not look(turn_right(dir)):
            return turn_right(dir)
        elif not look(turn_left(dir)):
            return turn_left(dir)
        else:
            return dir
    else:
        if not look(dir):
            return dir
        elif not look(turn_right(dir)):
            return turn_right(dir)
        else:
            return turn_left(dir)
