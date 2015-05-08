#include <iostream>
#include <cmath>
#include <vector>

#define epsilon 1e-9

using namespace std;

/*
 * Computes the normalized cross correlation of waves
 * starting at position ini0 and ini1, with a given length.
 * We don't need the actual values of the waves, but instead
 * some parameters like mean, deviation and scalarProduct
 */
double crosscor(int ini0, int ini1, int length,
                vector<double> sumValues[],
                vector<vector<double> > mean[],
                vector<vector<double> > deviation[],
                vector<vector<double> > scalarProduct[])
{
    // Since the correlation is obtained as
    // length sum (f(x)-mean(f))(g(x)-mean(g)) / devf devg,
    // we develop the product into 4 terms, sum each of them
    // individually, and finally multiply by length / devf devg
    
    // First we need to compute sum f(x), sum g(x), mean(f) and mean(g)
    
    double sumf = sumValues[0][ini0 + length] - sumValues[0][ini0];
    double sumg = sumValues[1][ini1 + length] - sumValues[1][ini1];
    double meanf = mean[0][0][mean[0][0].size()-1];
    double meang = mean[1][ini1][ini1 + length - 1];
    
    // Now we compute the sum of the four addends
    
    double correlation = 0;
    
    // f(x)g(x)
    
    // This is the scalar product in a given interval,
    // that can be obtained as the difference of two scalar products
    // of intervals that start at the beginning
    if (ini0 < ini1)
    {
        correlation += scalarProduct[0][ini1 - ini0][ini0 + length];
        correlation -= scalarProduct[0][ini1 - ini0][ini0];
    }
    else
    {
        correlation += scalarProduct[1][ini0 - ini1][ini1 + length];
        correlation -= scalarProduct[1][ini0 - ini1][ini1];
    }
    
    // - f(x) * mean(g)
    
    correlation -= sumf * meang;
    
    // - g(x) * mean(f)
    
    correlation -= sumg * meanf;
    
    // mean(g) * mean(f)
    
    correlation += length * meanf * meang;
    
    // Finally we multiply by the constant factor
    
    double deviationf = deviation[0][0][deviation[0][0].size()-1];
    double deviationg = deviation[1][ini1][ini1 + length - 1];
    
    
    if (fabs(deviationf * deviationg) < epsilon) return 0;
    
    return correlation * length / deviationf / deviationg;
}

int main()
{
    int sizes[2]; // size of the pattern and the wave respectively
    cin >> sizes[0] >> sizes[1];
    
    // We keep the patterns in an array of two vectors
    vector<double> values [2];
    for (int i = 0; i < 2; i++)
    {
        values[i] = vector<double> (sizes[i]);
        for (int j = 0; j < sizes[i]; j++)
            cin >> values[i][j];
    }
    
    // Precomputations: for each interval we compute the mean and the variance
    // First we compute the sums of values and squares of values
    
    vector<double> sumValues[2];
    vector<double> sumSquares[2];
    for (int i = 0; i < 2; i++)
    {
        sumValues[i] = sumSquares[i] = vector<double> (sizes[i] + 1, 0);
        for (int j = 0; j < sizes[i]; j++)
        {
            sumValues[i][j + 1] = sumValues[i][j] + values[i][j];
            sumSquares[i][j + 1] = sumSquares[i][j] + values[i][j] * values[i][j];
        }
    }
    
    // Now we use them to compute the mean and deviation of each subinterval
    vector<vector<double> > mean[2];
    vector<vector<double> > deviation[2];
    for (int i = 0; i < 2; i++)
    {
        mean[i] = deviation[i] = vector<vector<double> > (sizes[i], vector<double> (sizes[i]));
        for (int j = 0; j < sizes[i]; j++)
        {
            for (int k = j; k < sizes[i]; k++)
            {
                // Compute the mean and deviation of interval [j..k]
                mean[i][j][k] = (sumValues[i][k + 1] - sumValues[i][j]) / (k - j + 1);
                
                // The deviation is computed as the square root of the variance
                
                // The variance is sum (f(x) - mean) ^ 2, so we develop the square
                // into 3 terms and sum each of them individually
                
                // f(x) ^ 2
                double variance = sumSquares[i][k + 1] - sumSquares[i][j];
                
                // mean ^ 2
                variance += (k - j + 1) * mean[i][j][k] * mean[i][j][k];
                
                // - 2 * f(x) * mean
                variance -= 2 * mean[i][j][k] * (sumValues[i][k + 1] - sumValues[i][j]);
                
                deviation[i][j][k] = sqrt(variance);
            }
        }
    }
    
    // Finally, we need to precompute the scalar products of any possible subinterval.
    // We have enough computing them for every possible subinterval starting at the
    // beginning of each wave, since we can compute any other as a difference
    
    // scalarProduct[i][j][k] is the scalar product between wave i [0..k-1]
    // and the other wave [j..j+k-1]
    vector<vector<double> > scalarProduct[2];
    for (int i = 0; i < 2; i++)
    {
        int noti = (i + 1) % 2; //the other wave
        int maxSize = min(sizes[0], sizes[1]); //max size of the subintervals
        scalarProduct[i] = vector<vector<double> > (sizes[noti], vector<double> (maxSize + 1, 0));
        
        // beginning of the interval in the other wave
        for (int j = 0; j < sizes[noti]; j++)
        {
            // size of the interval
            for (int k = 1; k <= sizes[i] and j + k <= sizes[noti]; k++)
            {
                // Since we already obtained the scalar product of length k-1,
                // we compute the new scalar product by adding the product
                // of the last elements in the interval
                scalarProduct[i][j][k] = scalarProduct[i][j][k - 1];
                scalarProduct[i][j][k] += values[i][k - 1] * values[noti][j + k - 1];
                
            }
        }
    }
    double best = 0;
    
    // We conjecture the length, starting by the largest possible
    int length = min (sizes[0], sizes[1]);
    
    // Since the value of an interval is never larger than its size
    // (by the inequality of scalar product and product of norms),
    // we stop when the size is smaller than our best match
    while (length > best and length >= 2)
    {
        // best of a given length
        double localBest = 0;
        // Conjecture the beginning of each wave
        for (int i = 0; i + length <= sizes[0]; i++)
        {
            for (int j = 0; j + length <= sizes[1]; j++)
            {
                double c = crosscor(i, j, length, sumValues, mean, deviation, scalarProduct);
                localBest = max(c, localBest);
            }
        }
        
        // This is coherent with the observation, but it might not be correct
        if (localBest < best * 0.9) break;
        
        best = max (best, localBest);
        length --;
    }
    cout.setf(ios::fixed);
    cout.precision(4);
    cout << best << endl;
}