# Look B4 U Leap

short_name = "LB4UL"

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
        sub_best = 0
        for j in range(4):
            x2 = x1 + xdir[j]
            y2 = y1 + ydir[j]
            if (x2, y2) == (x, y):
                continue
            if inquire(x2, y2):
                continue
            sub_sub = 0
            for k in range(4):
                x3 = x2 + xdir[k]
                y3 = y2 + ydir[k]
                if (x3, y3) == (x, y) or (x3, y3) == (x1, y1):
                    continue
                if inquire(x3, y3):
                    continue
                sub_sub = 1
                break
            score2 = 1 + sub_sub
            if score2 > sub_best:
                sub_best = score2
        score = 1 + sub_best
        if score > best:
            best = score
    return 1 + best

def strategy():
    x, y, dir = get_info()
    best = 0
    d = turn_right(dir)
    for i in range(4):
        score = value(x + xdir[d], y + ydir[d], 3)
        if score > best:
            dir = d
            best = score
        d = turn_right(d)
    return dir
