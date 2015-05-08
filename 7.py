# This is the first competition where I use a programming language
# different from C++. Please, don't judge me!

import sys, collections

N, M = map(int, sys.stdin.readline().split())

# We keep the answers in a dictionary of lists
answers = {}
for i in range(N):
    line = sys.stdin.readline().split()
    answers[line[0]] = line[1:]
    
# The graph is a dictionary of sets
graph = collections.defaultdict(set)
for i in range(M):
    line = set(sys.stdin.readline().split())
    for j in line:
        # Add all the other people in the line as a friend
        graph[j] |= line - set([j])

# We keep the points of each person with a Counter
points = collections.Counter()

# Question 1: 7 points is they answer yes
for p, a in answers.iteritems():
    if a[0] == 'Y': points[p] += 7
    
# Question 2: 3 points for each friend that answers yes
for p, lf in graph.iteritems():
    for f in lf:
        if answers[f][1] == 'Y': points[p] += 3
        

# Question 3: 6 points for each friend at distance 2 that answers yes
for p, lf in graph.iteritems():
    # Friends of friends
    fof = set()
    for f in lf:
        fof |= graph[f]
    
    # Remove friends at distance 0 or 1
    for f in fof - lf - set([p]):
        if answers[f][2] == 'Y': points[p] += 6        


# Question 4: 4 points if there is a frient who answers yes
# but no other friends of them answer yes
# It can be done faster, but this is no slower than Q3,
# so the total time will be similar
for p, lf in graph.iteritems():
    # If someFriend = True then p wins 4 points
    someFriend = False
    for f in lf:
        if answers[f][3] == 'Y':
            # Check if all the friends satisfy the rule
            candidate = True
            for g in graph[f] - set([p]):
                if answers[g][3] == 'Y': candidate = False
                
            if candidate: someFriend = True
    if someFriend: points[p] += 4
    

# Before answering question 5 we need to know the connected components

# We store the connected components in a dictionary
connectedComponent = {}
# Number of connected components
numCC = 0
for p in answers.keys():
    if p not in connectedComponent:
        numCC += 1
        connectedComponent[p] = numCC
        # People in the new connected component
        pinCC = [p]
        while pinCC:
            q = pinCC.pop()
            for r in graph[q]:
                if r not in connectedComponent:
                    connectedComponent[r] = numCC
                    pinCC.append(r)
                    
# Now we compute the total number of people that answer yes,
# and the same for any connected component
numShopping = 0
numSbyCC = collections.Counter()
for p, c in connectedComponent.iteritems():
    if answers[p][4] == 'Y':
        numShopping += 1
        numSbyCC[c] += 1
        
# Finally, we add 5 for each person that answers yes
# and it is not in the same connected component
for p,c in connectedComponent.iteritems():
    points[p] += 5 * (numShopping - numSbyCC[c])


print max(points.values())