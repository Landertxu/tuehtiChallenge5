# This is the first competition where I use a programming language
# different from C++. Please, don't judge me!

import sys, collections

finalIsland = 'Raftel'

# this is more than the gold that you can collect by pillaging
inf = 1000000000

def backtracking (i, g, turn, remainingTurns, path, info):
    '''Computes the maximum amount of gold that can be obtained
    starting at island i, with gold g, in less than remainingTurns,
    having already visited the islands given in path,
    where info contains the routes between islands,
    the cost of reaching an island and
    the costs of colliding with another ship'''
    
    if i == finalIsland: return g
    if remainingTurns == 0: return -inf
    
    # First we try to pillage
    ans = backtracking (i, g + 10, turn + 1, remainingTurns - 1, path, info)
    
    # If we have gold we can try to move
    if g > 0:
        path.append('')
        # info[0] contains the graph
        for (island, cost) in info[0][i]:
            newTurn = turn + 1
            # Cost of traversing the route
            newGold = max(0, g - cost)
            # Cost of arriving in the island
            newGold = max(0, newGold - info[1][island])
            # Cost of colliding with another ship
            newGold = max(0, newGold - info[2][newTurn][island])
            path[-1] = island
            cand = backtracking(island, newGold, newTurn, remainingTurns - 1, path, info)
            ans = max (ans, cand)
        path.pop()
        
    return ans

numIslands = int(sys.stdin.readline())

# We keep a dictionary with the cost of each island and the order of the input
costIsland = {}
orderInput = {}
for i in range(numIslands):
    name, cost = sys.stdin.readline().split()
    costIsland[name] = int(cost)
    orderInput[name] = i
    
numRoutes = int(sys.stdin.readline())

# Our implementation of the graph is a dictionary of 
# lists of routes, each of them of the type (destination, cost)

graph = collections.defaultdict(list)
for i in range(numRoutes):
    origin, destination, cost = sys.stdin.readline().split()
    graph[origin].append((destination, int(cost)))

# Now we compute the next Island for each even and odd ship
# Position 0 is for even ships and position 1 for odd ships
nextIsland = [{}, {}]
for i in costIsland.keys():
    if graph[i]:
        nextIsland[0][i] = max([(c, -orderInput[d], d) for (d, c) in graph[i]])[2]
        nextIsland[1][i] = min([(c, orderInput[d], d) for (d, c) in graph[i]])[2]
    
numShips = int(sys.stdin.readline())
idShip, nameShip, gold, iniIsland = sys.stdin.readline().split()

# For every ship we only need the time when they arrive in each island

# collisions [t][i] saves the extra cost of arriving in island i at time t
collisions = [collections.Counter()]

# maxTurns computes the number of turns that we have to reach the last island
maxTurns = numIslands + 1

for i in range(1, numShips):
    idShip, nameShip, costCollision, islandShip = sys.stdin.readline().split()
    cost = int(costCollision)
    time = 0
    parity = int(idShip) % 2
    
    # Simulate the route of the ship and add the collisions
    while islandShip != finalIsland and nextIsland[parity][islandShip]:
        time += 1
        islandShip = nextIsland[parity][islandShip]
        while len(collisions) <= time:
            collisions.append(collections.Counter())
        if islandShip == finalIsland:
            maxTurns = min(maxTurns, time)
        else:
            collisions[time][islandShip] += cost

print backtracking (iniIsland, int(gold), 0, maxTurns, [], (graph, costIsland, collisions))
    