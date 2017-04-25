for i in range(1<<4):
    solution = []
    lastbit = 0xFF
    for bit in range(4):
        if ((i & (1<<bit)) == 0):
            solution.append(4*bit)
            solution.append(4*bit+1)
            solution.append(4*bit+2)
            solution.append(4*bit+3)
            lastbit = bit
    while(len(solution) < 16):
        if(lastbit == 0xFF):
          solution.append(lastbit)
          solution.append(lastbit)
          solution.append(lastbit)
          solution.append(lastbit)
        else :
          solution.append(4*lastbit)
          solution.append(4*lastbit+1)
          solution.append(4*lastbit+2)
          solution.append(4*lastbit+3)
    s = ""
    for j in range(16):
        s+=hex(solution[j])+","
    print(s)
