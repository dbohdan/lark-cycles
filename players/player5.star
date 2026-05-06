# Keep'm Flying

short_name = "Flyin"

xdir = (0, 1, 0, -1)
ydir = (-1, 0, 1, 0)

def strategy():
    x, y, dir = get_info()
    best = 8
    d = dir
    for i in range(4):
        if look(d):
            continue
        score = 0
        for j in range(2, 4):
            score += inquire(x + j * xdir[d], y + j * ydir[d])
        if score < best:
            best = score
            dir = d
        d = turn_right(d)
    return dir
