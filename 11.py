# This is the first competition where I use a programming language
# different from C++. Please, don't judge me!

import sys, fileinput, collections

startRoom = 'start'
exitRoom = 'exit'
modulo = 1000000007

factorials = [1]
inversefact = [1]

def inverse (a):
    '''Computes b such that a*b === 1 mod modulo'''
    
    return pow(a, modulo - 2, modulo)

def binomial (a, b):
    '''Computes (a! / (b! (a-b)!)) % modulo'''
    
    if a < b: return 0
    
    # Since we do not know the limits, we keep a list of all 
    # the needed factorials, modulo 'modulo'
    
    while len(factorials) <= a:
        factorials.append((factorials[-1] * (len(factorials))) % modulo)
        inversefact.append(inversefact[-1] * inverse((len(inversefact))) % modulo)
        
    return (factorials[a] * inversefact[b] * inversefact[a - b]) % modulo

scenarios = fileinput.input()

numScenarios = int(scenarios.next())

# We compute the answers of all the scenarios at the beginning
answers = []



for scenario in range(numScenarios):
    print scenario
    Stamina, Rooms = map(int, scenarios.next().split())
    
    # The graph is a dictionary that maps rooms to tuples
    graph = {}
    
    # We need to keep how many rooms there are before each one
    parents = collections.Counter()
    for room in range(Rooms):
        nameRoom, doors = scenarios.next().split()
        graph[nameRoom] = []
        for door in range(int(doors)):
            nextRoom, keys, stamina = scenarios.next().split()
            
            graph[nameRoom].append((nextRoom, int(keys), int(stamina)))
        
            # We will use this to check wether we alread know
            # all the information for a given room
            parents[nextRoom] += 1
        
    # We keep a list of rooms such that we already visited all the 
    # rooms with doors that lead to that room
    visitableRooms = [startRoom]
    
    # For each room we compute the number of ways that we have
    # to arrive there with any possible level of stamina
    numWays = collections.defaultdict(collections.Counter)
    
    # We compute the path that requieres most stamina
    longestPaths = {exitRoom : 0}

    def longestPath (a, g):
        if a == exitRoom: return 0
        if a in longestPaths: return longestPaths[a]
        ans = -1
        for nextRoom, keys, neededSt in graph[a]:
            b = longestPath (nextRoom, g)
            if b == -1: continue
            minions = 1 # The guardian of the door
            minions += max (0, keys - 1) # Keys to open the door
            
            ans = max(ans, max (0, b + neededSt - minions))
        longestPaths[a] = ans
        return ans
    longestPath(startRoom, graph)
    
    numWays[startRoom][Stamina] = 1
    itera = 0
    minst = Stamina
    while visitableRooms and visitableRooms[-1] != exitRoom:
        currentRoom = visitableRooms.pop()
        
        numDoors = len(graph[currentRoom])
        
        # We try to escape using any possible room
        for nextRoom, keys, neededSt in graph[currentRoom]:
            
            # Compute the result for any possible level of stamina
            for levelSt, ways in reversed(sorted(numWays[currentRoom].iteritems())):
                
                # The minions we need to kill depends on the number
                # of keys and the needed stamina
                minions = 1 # The guardian of the door
                minions += max (0, keys - 1) # Keys to open the door
                minions += max (0, neededSt - levelSt - minions) # extra stamina
                
                # We have to kill the guardian plus other m - 1
                waysKillingMinions = binomial (numDoors - 1, minions - 1)
                
                # The total number of ways also depends on the number of ways 
                # of reaching the room with the given level of stamina
                totalWays = (waysKillingMinions * ways) % modulo
                if totalWays > 0 and neededSt <= Stamina:
                    newStamina = min(Stamina, min(Stamina, levelSt + minions) - neededSt)
                    newStamina = min (newStamina, longestPaths[nextRoom])
                    numWays[nextRoom][newStamina] += totalWays
                    numWays[nextRoom][newStamina] %= modulo
                    
                # If we cannot pass with a given stamina we cannot pass with less either
                else:
                    break
                    
                    
            # Finally we update the next room
            parents[nextRoom] -= 1
            if parents[nextRoom] == 0:
                visitableRooms.append(nextRoom)
            
    # The total sum is the total number of ways in which
    # we can arrive at the exit with any possible stamina
    totalSum = 0
    for ways in numWays[exitRoom].values():
        totalSum += ways
        totalSum %= modulo
        
    answers.append(totalSum)
    
for line in sys.stdin.readlines():
    scenario = int(line)
    print "Scenario {}: {}".format(scenario, answers[scenario])
                    
        