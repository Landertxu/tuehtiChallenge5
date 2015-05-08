#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

/*
 * Returns the sum of the rectangle [xini...xend] x [yini...yend],
 * where sums contains the sums of all the rectangles [0..i] x [0..j]
 */
long long sum(int yini, int yend, int xini, int xend, vector<vector<long long> >& sums)
{
    // The sum is obtained by summing and substracting four rectangles
    // with top-left corner in (0,0)
    long long ans = 0;
    ans = sums[yend + 1][xend + 1];
    ans -= sums[yini][xend + 1] + sums[yend + 1][xini];
    ans += sums[yini][xini];
    return ans;
}

int main()
{
    // Read the matrix
    ifstream fin("sheet.data");
    int M, N;
    fin >> M >> N;
    
    vector<vector<int> > matrix (M, vector<int> (N));
    
    for (int i = 0; i < M; i++)
        for (int j = 0; j < N; j++)
                fin >> matrix[i][j];
    
    // sumMatrix[i][j] keeps the sum of the rectangle [0..i-1]x[0..j-1]    
    vector<vector<long long> > sumMatrix (M + 1, vector<long long> (N + 1, 0));
    
    // Each value is obtained by summing and substracting the neighbour rectangles
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < N; j++)
        {
            sumMatrix[i + 1][j + 1] = matrix[i][j];
            sumMatrix[i + 1][j + 1] += sumMatrix[i][j + 1] + sumMatrix[i + 1][j];
            sumMatrix[i + 1][j + 1] -= sumMatrix[i][j];
        }
    }
    
    int Tests;
    cin >> Tests;
    for (int nTest = 1; nTest <= Tests; nTest++)
    {
        int y0, x0, y1, x1, K;
        cin >> y0 >> x0 >> y1 >> x1 >> K;
        
        // First we compute the rectangle where the hole may be placed
        int left = x0 + K;
        int right = x1 - K;
        int top = y0 + K;
        int down = y1 - K;
        
        long long ans = 0;
        
        // Now we conjecture every possible value of the hole.
        // Note that this value determines the airscrew
        for (int i = left; i <= right; i++)
        {
            for (int j = top; j <= down; j++)
            {
                long long topSquare = sum(j - K, j - 1, i - K, i - 1, sumMatrix);
                long long downSquare = sum(j + 1, j + K, i + 1, i + K, sumMatrix);
                ans = max (ans, topSquare + downSquare);
            }
        }
        
        cout << "Case " << nTest << ": " << ans << endl;
    }
}