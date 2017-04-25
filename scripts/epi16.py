for i in range(1<<8):
    solution = []
    lastbit = 0xFF
    for bit in range(8):
        if ((i & (1<<bit)) == 0):
            solution.append(2*bit)
            solution.append(2*bit+1)
            lastbit = bit
    while(len(solution) < 16):
        if (lastbit == 0xFF):
          solution.append(lastbit)
          solution.append(lastbit)
        else:
          solution.append(2*lastbit)
          solution.append(2*lastbit+1)
    s = ""
    for j in range(16):
        s+=hex(solution[j])+","
    print(s)
