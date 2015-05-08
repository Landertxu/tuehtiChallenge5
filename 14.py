# This is the first competition where I use a programming language
# different from C++. Please, don't judge me!

import sys, fileinput, collections

def dfs (vertex, graph, order, visited):
    '''First search for Tarjan's algorithm'''
    
    if visited[vertex]: return
    visited[vertex] = True
    for neighbour in graph[vertex]:
        dfs(neighbour, graph, order, visited)
    order.append(vertex)

def dfs2 (vertex, graph, connectedComponent, numCC):
    '''Second search for Tarjan's algorithm'''
    
    for neighbour in graph[vertex]:
        if connectedComponent[neighbour] == -1:
            connectedComponent[neighbour] = numCC
            dfs2(neighbour, graph, connectedComponent, numCC)

def dotProduct (a, b):
    '''Return the dot product of a and b'''
    
    return a[0] * b[0] + a[1] * b[1]

def inside (enemy, polygon):
    '''Return true if the coordenates of enemy lie
    inside the given polygon'''
    
    # A point is inside if its at the same side of
    # each of the edges
    atLeft = False
    atRight = False
    for i in range(len(polygon)):
        vertex = polygon[i]
        nextVertex = polygon[(i+1)%len(polygon)]
        
        # We need a vertex orthogonal to the side
        orthogonal = (vertex[1] - nextVertex[1], nextVertex[0] - vertex[0])
        
        # We need another vector pointing the enemy
        vector = (enemy[0] - vertex[0], enemy[1] - vertex[1])
        
        direction = dotProduct(orthogonal, vector)
        
        if direction < 0: atLeft = True
        if direction > 0: atRight = True
        if atLeft and atRight: return False
    
    return True

numEnemies = int(sys.stdin.readline())

coordenates = []
polygon = []

for i in range(numEnemies):
    coordenates.append(tuple(map(int, sys.stdin.readline().split())))
    vertexPolygon = int(sys.stdin.readline())
    vertices = map(int, sys.stdin.readline().split())
    polygon.append([tuple(vertices[2 * j: 2 * j + 2]) for j in range(vertexPolygon)])
    

# We keep a directed graph with the info of whether
# an enemy is inside the exploding shape of another one

graph = []


for i in range(numEnemies):
    graph.append([])
    for j in range(numEnemies):
        if i != j and inside(coordenates[j], polygon[i]):
            graph[i].append(j)
            
# We need to shoot as many times as strongly connected components
# without in edges. First, compute the strongly connected components

reversedGraph = []
for i in range(numEnemies): reversedGraph.append([])
for i in range(numEnemies):
    for j in graph[i]:
        reversedGraph[j].append(i)
    
order = []
visited = [False] * numEnemies
for i in range(numEnemies):
    dfs (i, graph, order, visited)
    
connectedComponent = [-1] * numEnemies
numCC = 0
while order:
    vertex = order.pop()
    if connectedComponent[vertex] == -1:
        connectedComponent[vertex] = numCC
        dfs2 (vertex, reversedGraph, connectedComponent, numCC)
        numCC += 1
        
# We keep the strongly connected components with in edges
initials = set()
for i in range(numEnemies):
    for j in graph[i]:
        if connectedComponent[i] != connectedComponent[j]:
            initials.add(connectedComponent[j])

# The answers is the difference with the total number
print numCC - len(initials)