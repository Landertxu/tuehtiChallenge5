#include <iostream>
#include <vector>

using namespace std;

int lim = 100000000;

int main()
{
    // First we perform a sieve of Eratosthenes to compute
    // all the prime numbers up to lim
    
    // Instead of recording whether the number is prime, we also rememember
    // one of its prime factors. If it is prime, lastDivisor[i] = -1
    vector<int> lastDivisor (lim + 1, -1);
    
    // If we have not found a divisor of i up to sqrt(lim) then i is not prime
    for (int i = 2; i * i <= lim; i++)
    {
        // I we cannot find a divisor up to i then i must be prime
        if (lastDivisor[i] == -1)
        {
            // All the multiples of i are now composite
            for (int j = i * 2; j <= lim; j += i)
                lastDivisor[j] = i;
        }
    }
    
    // Now we compute the number of almost primes in 0..i
    vector<int> almostPrimes (lim + 1, 0);
    for (int i = 1; i <= lim; i++)
    {
        bool isAlmostPrime = true;
        
        // if the number is prime then it is not almost prime
        if (lastDivisor[i] == -1) isAlmostPrime = false;
        else
        {
            // if after dividing by lastDivisor the result is not prime,
            // then the number is not almost prime
            if (lastDivisor[i / lastDivisor[i]] != -1)
                isAlmostPrime = false;
        }
        
        // the total count of almost primes is the count up to i - 1 plus
        // 1 if the number is almost prime
        almostPrimes[i] = almostPrimes[i - 1] + (isAlmostPrime?1:0);
    }
    
    int T;
    cin >> T;
    while (T--)
    {
        int A, B;
        cin >> A >> B;
        // the number of almost primes in [A..B] is the number of almost primes
        // in [0..B] minus then umber of almost primes in [0..A-1]
        cout << almostPrimes[B] - almostPrimes[A-1] << endl;
    }
}