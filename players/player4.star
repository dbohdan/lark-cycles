# Round the Clock

short_name = "Clock"

xdir = (0, 1, 0, -1)
ydir = (-1, 0, 1, 0)

def strategy():
    x, y, dir = get_info()
    best = 1000
    for i in range(4):
        if look(i):
            continue
        score = 0
        for j in range(2, 4):
            score += inquire(x + j * xdir[i], y + j * ydir[i])
        if score < best:
            best = score
            dir = i
    return dir
