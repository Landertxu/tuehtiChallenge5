# This is the first competition where I use a programming language
# different from C++. Please, don't judge me!

import fileinput

# Skip the first line
first = True

for line in fileinput.input():
    
    if first:
        first = False
        continue
    
    N = int(line)
    
    # The optimal solution consists in using only the odd urinals
    
    print (N + 1) / 2
    