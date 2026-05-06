# Mr. Right

short_name = "Right"

def strategy():
    x, y, dir = get_info()
    if not look(dir):
        return dir
    if not look(turn_right(dir)):
        return turn_right(dir)
    return turn_left(dir)
