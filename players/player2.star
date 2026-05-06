# Dizzy

short_name = "Dizzy"

def strategy():
    x, y, dir = get_info()
    if not look(turn_left(dir)):
        return turn_left(dir)
    elif not look(turn_right(dir)):
        return turn_right(dir)
    else:
        return dir
