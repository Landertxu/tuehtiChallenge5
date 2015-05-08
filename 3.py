# This is the first competition where I use a programming language
# different from C++. Please, don't judge me!

import fileinput, collections, sys

def isPrime (n):
    '''Since the primes in this problem are small the sieve of Erathostenes
    would take the same amount of time as brute force'''
    
    i = 2
    while (i * i <= n):
        if n % i == 0: return False
        i += 1
    
    return True

# First we compute the first 25 prime numbers
primeList = []
numPrimes = 25

candidate = 2
while len(primeList) < numPrimes:
    if isPrime(candidate):
        primeList.append(candidate)
    candidate += 1
# For each line l and each prime p we keep a counter of the appearance of
# the prime p in lines [0..l]
sumPrimes = [collections.Counter()]

for line in fileinput.input():
    number = int(line)
    
    # We compute a counter of the appeareance of each prime
    factors = collections.Counter()
    
    for p in primeList:
        while number % p == 0:
            factors[p] += 1
            number /= p
    
    # We add the current factors to the sum that we have so far
    sumPrimes.append(factors)
    sumPrimes[-1].update(sumPrimes[-2])

# Skip the first line
sys.stdin.readline()

for line in sys.stdin.readlines():
    A, B = [int(x) for x in line.split()]
    
    # We compute the number of appearances for each prime
    primes = {}
    for p in primeList:
        # We index 1-based, so the total sum in [A,B) will be
        # the sum in [0,B] minus the sum in [0,A]
        primes[p] = sumPrimes[B][p] - sumPrimes[A][p]
    
    maxValue = max(primes.values())
    
    print maxValue, ' '.join([str(p) for p in primeList if primes[p] == maxValue])
    