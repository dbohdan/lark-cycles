# Not so Dizzy

short_name = "NotSo"

xdir = (0, 1, 0, -1)
ydir = (-1, 0, 1, 0)

def value(x, y, max_depth):
    if inquire(x, y):
        return 0
    best = 0
    for i in range(4):
        x1 = x + xdir[i]
        y1 = y + ydir[i]
        if inquire(x1, y1):
            continue
        sub = 0
        for j in range(4):
            x2 = x1 + xdir[j]
            y2 = y1 + ydir[j]
            if (x2, y2) == (x, y):
                continue
            if inquire(x2, y2):
                continue
            sub = 1
            break
        score = 1 + sub
        if score > best:
            best = score
    return 1 + best

def strategy():
    x, y, dir = get_info()
    best = 0
    d = turn_right(dir)
    for i in range(4):
        score = value(x + xdir[d], y + ydir[d], 2)
        if score > best:
            dir = d
            best = score
        d = turn_right(d)
    return dir
