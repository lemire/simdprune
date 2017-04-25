for i in range(256):
    l = []
    lastval = 0
    for j in range(8):
        if(i & ( 1<< j) == 0):
            l.append(j)
            lastval = j
    while(len(l) < 8): l.append(lastval)
    for z in l:
        print(str(z)+",",end="")
    print()
