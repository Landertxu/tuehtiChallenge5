# This is the first competition where I use a programming language
# different from C++. Please, don't judge me!

import sys, fileinput, collections, itertools

def backtracking (elements, transformations, values):
    '''Compute the maximum possible value of elements after
    perform some of the given transformations'''
    
    # First we compute the basic value
    ans = sum([values[e] for e in elements])
    
    # We will need this later
    counterElem = collections.Counter(elements)
    
    # Now we try to combine elements. There are at most 10
    for num in range(2, 11):
        for perm in itertools.combinations(elements, num):
            # Try to combine elements in perm
            for newElement in transformations[tuple(perm)]:
                # Compute the new elements
                newElements = list((counterElem - collections.Counter(perm)).elements())
                newElements.append(newElement)
                newAns = backtracking(sorted(newElements), transformations, values)
                ans = max(ans, newAns)
    
    return ans


# We keep the transformations into a dictionary of tuples of elements
transformations = collections.defaultdict(list)
values = {}
for line in fileinput.input():
    spline = line.split()
    name = spline[0]
    gold = int(spline[1])
    elements = spline[2:]
    values[name] = gold
    if elements:
        transformations[tuple(sorted(elements))].append(name)


Tests = int(sys.stdin.readline())
for i in range(Tests):
    elements = sorted(sys.stdin.readline().split())
    # We try al the possibilities
    print backtracking(elements, transformations, values)