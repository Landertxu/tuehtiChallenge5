// Simplex method v2.01 (Feng Xie, xief@mcmaster.ca)
//
// simplex (vector<vector<double>> A, vector<double> B, vector<double> C,
//          vector<double> X, double obj);
//   Input: A - constraint matrix in standard form (equality);
//          B - right hand side, should be non-negative;
//          C - objective vector.
//   Output: X - optimal solution; obj - optimal value;
//   Returns: -1 - unbounded; 0 - infeasible; 1 - feasible & bounded.
//   Note: it solves a maximization problem.

#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <algorithm>    // std::count

using namespace std;

#define EPS 1E-9
#define DEBUG 0


// Check if column c of A is an indentity column.
// Returns: -1 if not; non-zero row id otherwise.
inline int identity_col (const vector <vector <double> > & A, int c) {
  int count = 0, row;
  for (int r=0; r<A.size(); r++) 
    if (A[r][c] > EPS) { count++; row = r; }  
  return (count == 1) ? row : -1;
}


// Convert a standard form of LP into canonical form:
// 1) The entries in the objective vector corresponding to basic variables are
//    0's.
// 2) The submatrix corresponding to basic variables is an identity matrix.
void canonicalize ( vector <vector <double> > & A,
		    vector <double> & B, 
		    vector <double> & C,
		    vector <int> & BasicVarR,   // basic variable of each row
		    double & obj                // objective value
		    )
{
  int m = A.size(), n = C.size();

  for (int r=0; r<m; r++) {
    int bc = BasicVarR[r];          // col. that the basic variable is in
    if ( fabs(A[r][bc] - 1.0) > EPS) {
      double p = A[r][bc];
      for (int c=0; c<n; c++) A[r][c] /= p;
      B[r] /= p;
    }

    if (fabs(C[bc]) > EPS) {
      double p = C[bc];
      for (int c=0; c<n; c++) C[c] -= A[r][c] * p;
      obj -= B[r] * p;
    }
  }
}


// Pivoting iterations.
// Returns true if the problem is unbounded, false otherwise.
bool pivoting ( vector <vector <double> > & A,
		vector <double> & B, 
		vector <double> & C,
		vector <int> & BasicVarR,   // basic variable of each row
		double & obj                // objective value
		)
{
  int m = A.size(), n = C.size();

  while (1) {
    // Find the entering variable (use the first one encountered).
    int ev = 0;                    // id of the entering variable    
    for (ev=0; ev<n; ev++) 
      if (C[ev] < -EPS) break;

    if (ev == n) break;           // optimum reached.
      
    // Find the leaving variable using ratio test.
    int lvr = -1;                  // leaving variable, id'ed by row
    double minRatio;
    for (int r=0; r<m; r++) {
      if (A[r][ev] > EPS) {
	if ( lvr < 0 || B[r]/A[r][ev] < minRatio ) {           
	  lvr = r; minRatio = B[r] / A[r][ev]; 
	}
      }
    }
    if (lvr < 0) return true;      // unbounded
    int lv = BasicVarR[lvr];       // leaving variable

    // Leaving & entering variables identified, do pivoting.
    BasicVarR[lvr] = ev;
    double p = A[lvr][ev];
    for (int c=0; c<n; c++) A[lvr][c] /= p; B[lvr] /= p;
    for (int r=0; r<m; r++) {
      if ( r != lvr && fabs (A[r][ev]) > EPS ) {
	double p2 = A[r][ev];
	for (int c=0; c<n; c++) A[r][c] -= p2 * A[lvr][c];
	B[r] -= p2 * B[lvr];
      }
    }
    if ( fabs (C[ev]) > EPS ) {
      double p2 = C[ev];
      for (int c=0; c<n; c++) C[c] -= p2 * A[lvr][c];
      obj -= p2 * B[lvr];
    }

    if (DEBUG) {
      for (int c=0; c<n; c++) cout << C[c] << "\t"; cout << obj << endl;
      for (int r=0; r<m; r++) {
	for (int c=0; c<n; c++) cout << A[r][c] << "\t";
	cout << B[r] << endl;
      }
      cout << endl;
    }
  }
  return false;
}


// Solver for Ax = b, where A is a nonsingular square matrix.
// LU decomposition (Doolittle's method) is used.
void LU_solver ( vector <vector <double> > & A, // matrix A
		 vector <double> & B,           // b
		 vector <double> & X            // x
		 )
{
  int n = A.size();
  
  if (X.size() != n)
    X.resize (n);

  vector <vector <double> > L ( n, vector<double> (n) );
  vector <vector <double> > U ( n, vector<double> (n) );

  // LU decomposition (Dolittle's method).
  for (int i=0; i<n; i++) 
    L[i][i] = 1.0; // diagonals of L are 1's
  
  // The first row of U is the same as the first row of A.
  copy ( A[0].begin(), A[0].end(), U[0].begin() );
  
  for (int k=0; k<n-1; k++) {
    for (int i=k+1; i<n; i++) { // compute the k'th column of L
      double t = A[i][k];
      for (int j=0; j<k; j++)
	t -= ( L[i][j] * U[j][k] );
      L[i][k] = t / U[k][k];
    }
    for (int j=k+1; j<n; j++) { // compute the (k+1)'s row of U
      double t = A[k+1][j];
      for (int i=0; i<k+1; i++) 
	t -= ( L[k+1][i] * U[i][j] );
      U[k+1][j] = t;
    }
  }

  // Forward substitution.
  for (int k=0; k<n; k++) {
    X[k] = B[k];
    for (int j=0; j<k; j++) 
      X[k] -= ( X[j] * L[k][j] );
  }

  // Backward substitution. (The new B is stored in X.)
  for (int k=n-1; k>=0; k--) {
    for (int j=k+1; j<n; j++)
      X[k] -= ( X[j] * U[k][j] );
    X[k] /= U[k][k];
  }
}


// Preprocessing:
//   1) Make sure that the r.h.s. (B) is non-negative.
//   2) Remove redundant constraints detected by linear dependency.
//   3) Solve the system if it is determined or over-determined, and check
//      if it is consistent.
// Returns:
//   -1 : if the system is inconsistent.
//   -2 : if the system is determined or over-determined and has unique
//        non-negative solution, which is stored in X.
//  >=0 : otherwise indicate the # of redundancies removed.
int preprocess ( vector <vector <double> > & A,     // constraint matrix
		 vector <double> & B,               // right hand side
		 vector <double> & X                // unknowns
		 )
{
  int m = A.size ();                 // # of constraints
  int n = A[0].size ();              // # of variables
  vector <bool> IsRedundant (m, false);  // flags for redundant constraint

  // Remove all-zero rows.
  for (int r=0; r<m; r++) {
    bool allZero = true;
    for (int c=0; c<n; c++) 
      if (fabs(A[r][c]) > EPS) { allZero = false; break; }
    if (allZero) {
      if (fabs(B[r]) > EPS) return -1;
      else IsRedundant[r] = true;
    }
  }

  for (int i=0; i<m; i++) if (!IsRedundant[i]) {
    for (int j=i+1; j<m; j++) if (!IsRedundant[j]) {
      int c;
      double ratio = 0.0;
	  
      for (c=0; c<n; c++) {
	if ( fabs(A[i][c]) < EPS && fabs(A[j][c]) < EPS )       // both are 0 
	  continue;
	else if ( fabs(A[i][c]) < EPS && fabs(A[j][c]) > EPS || // one is 0
	     fabs(A[i][c]) > EPS && fabs(A[j][c]) < EPS )
	  break;
	else {                                            // both are nonzero
	  if ( fabs(ratio) < EPS )
	    ratio = A[i][c] / A[j][c];
	  else {
	    if ( fabs (A[i][c]/A[j][c] - ratio) > EPS ) 
	      break;
	  }
	}
      }

      if (c == n) {
	if ( fabs(B[i]) < EPS && fabs(B[j]) < EPS || 
	     fabs(B[j]) > EPS && fabs (B[i]/B[j] - ratio) < EPS )
	  IsRedundant[j] = true;
	else return -1;              // inconsistency detected
      }
    }
  }

  int numRedundancies = count (IsRedundant.begin(), IsRedundant.end(), true);
  
  // Remove the redundant constraints.
  if (numRedundancies > 0) {
    int ir = 0;                      // 1 position to the right of the new A
    for (int i=0; i<m; i++) { 
      if (!IsRedundant[i]) {
	if (ir < i) {                // overiding
	  copy (A[i].begin(), A[i].end(), A[ir].begin());
	  B[ir] = B[i];
	}
	ir++;
      }
    }
    for (int i=0; i<numRedundancies; i++) { 
      A.erase (A.end()-1);
      B.erase (B.end()-1);
    }
  }

  m -= numRedundancies;


  if (m >= n) {                      // determined or overdetermined system
    // A0, B0, X0 : for LU solver.
    vector <vector <double> > A0 (n, vector<double> (n));
    vector <double> B0 (n);

    for (int r=0; r<n; r++) {
      copy (A[r].begin(), A[r].end(), A0[r].begin());
      B0[r] = B[r];
    }

    LU_solver (A0, B0, X);

    // Check if the solutions are non-negative.
    bool nonNegative = true;
    for (int c=0; c<n; c++)
      if (X[c] < 0) { nonNegative = false; break; }
    if (!nonNegative) 
      return -1;

    // Check if X satisfies the other constraints.
    bool consistent = true;
    for (int r=n; r<m; r++) {
      double lhs = 0.0;
      for (int c=0; c<n; c++) 
	lhs += A[r][c] * X[c];
      if ( fabs (lhs - B[r]) > EPS ) { // constraint c not satisfied
	consistent = false;
	break;
      }
    }
    return (consistent ? -2 : -1);
  } 
      
  return numRedundancies;
}


// Simplex method (primal).
// Returns: -1 - unbounded; 0 - infeasible; 1 - feasible & bounded.
int simplex ( const vector <vector <double> > & A,  // constraint matrix 
	      const vector <double> & B,            // right hand side
	      const vector <double> & C,            // objective vector
	      vector <double> & X,                  // unknowns
	      double & obj                          // objective value
	      )
{
  int m = A.size();                  // # of inequalities
  int n = A[0].size();               // # of variables

  // Sanity check.
  if (!m || m != B.size() || n != C.size()) { 
    cout << "Wrong inputs!\n"; while(1);//exit(1); 
  }

  // Adjust size of X if necessary.
  if (X.size() != n) X.resize(n);
  fill (X.begin(), X.end(), 0);

  // A0, B0 : for preprocessing
  vector <vector <double> > A0 ( m, vector<double>(n) );
  vector <double> B0 (m);

  for (int r=0; r<m; r++) 
    copy (A[r].begin(), A[r].end(), A0[r].begin() );
  copy ( B.begin(), B.end(), B0.begin() );

  int ret_val = preprocess (A0, B0, X);
  int numRedundancies;

  if (ret_val == -1)                 // inconsistent system
    return -1;
  else if (ret_val == -2)            // solved
    return 1;
  else                               // need to run Simplex
    numRedundancies = ret_val;   

  m = A0.size ();                    // size changes after redundancy removal

  vector <bool> IsBasic (n, false);  // bit flag for basic variables
  vector <int> BasicVarR (m, -1);    // basic variable of each row

  // Find the initial basic variables.
  int numBasicVar = 0;
  for (int c=0; c<n; c++) {
    int r = identity_col (A, c);
    if (r >= 0 && BasicVarR[r] < 0) {
      IsBasic[c] = true;
      BasicVarR[r] = c;
      numBasicVar++;
    }
  }

  // A2, B2, C2 : for Phase II
  vector <vector <double> > A2 ( m, vector<double>(n) );
  vector <double> B2 (m);
  vector <double> C2 (n);
  
  for (int r=0; r<m; r++) 
    copy ( A0[r].begin(), A0[r].end(), A2[r].begin() );
  copy ( B0.begin(), B0.end(), B2.begin() );
  for (int c=0; c<n; c++) 
    C2[c] = -C[c];                   // obj. vector should be negated
  obj = 0;

  if (numBasicVar < m) {
    // No initial b.f.s is found, go into Phase I.
    int n1 = n;                      // Phase I need extra dummy variables
    
    // A1, B1, C1 : for Phase I.
    vector <vector <double> > A1 (m, vector<double>(n) ); 
    vector <double> B1 (m);          
    vector <double> C1 (n, 0);       // new objective vector for phase I

    for (int r=0; r<m; r++)
      copy ( A0[r].begin(), A0[r].end(), A1[r].begin() );
    copy ( B0.begin(), B0.end(), B1.begin() );  // r.h.s. is the same

    for (int i=0; i<m; i++) {
      if (BasicVarR[i] < 0) {
	for (int r=0; r<m; r++) {
	  if (r == i) A1[r].push_back (1);
	  else A1[r].push_back (0);
	}
	C1.push_back (1);            
	BasicVarR[i] = n1;
	n1++;
      }
    }
    
    C1.resize (n1, 1);                // Adjust sizes of objective vector

    canonicalize (A1, B1, C1, BasicVarR, obj);  // convert to canonical form  

    bool unbounded = pivoting (A1, B1, C1, BasicVarR, obj);  // pivoting

    if (unbounded) {
      cout << "Unbounded Phase I!" << endl;
      while(1);//exit (1);
    }

    bool feasible = (fabs(obj) < EPS) ? true : false;
    if (!feasible) return 0;

    // Initial b.f.s. found, prepare for Phase II.
    for (int r=0; r<m; r++) {
      for (int c=0; c<n; c++)
	A2[r][c] = A1[r][c];
      B2[r] = B1[r];
    }
  }

  // Phase II.
  canonicalize (A2, B2, C2, BasicVarR, obj);

  bool unbounded = pivoting (A2, B2, C2, BasicVarR, obj);

  for (int r=0; r<m; r++)              // r.h.s. is the basic solution 
    X[BasicVarR[r]] = B2[r];

  return ( unbounded ? -1 : 1 );
}

// My code starts here!

int main()
{
    int T;
    cin >> T;
    for (int testCase = 1; testCase <= T; testCase++)
    {
        int T, S, P;
        cin >> T >> S >> P;
        
        // Minutes per person
        vector<double> totalMin (P);
        
        // Minutes to chop a tree
        vector<vector<double> > minTree (P, vector<double> (T));
        for (int i = 0; i < P; i++)
        {
            cin >> totalMin[i];
            for (int j = 0; j < T; j++)
                cin >> minTree[i][j];
        }
        // Number of constraints
        int nConst = P + T;
        
        // Number of variables
        int nVar = P * T + nConst;
        
        // Vectors for the linear programming problem
        vector <vector <double> > A (nConst, vector<double>(nVar));
        vector <double> B (nConst), C(nVar, 0);
        
        // Function to maximize:
        for (int i = 0; i < T * P; i++)
            C[i] = -1;
        
        // Constraints of maximum number of minutes
        for (int i = 0; i < P; i++)
        {
            for (int j = 0; j < T; j++)
                A[i][i * T + j] = 1;
            
            // slack variable
            A[i][T * P + i] = 1;
            
            B[i] = totalMin[i];
        }
        
        // Constraints for the trees
        for (int i = 0; i < T; i++)
        {
            for (int j = 0; j < P; j++)
                A[i + P][j * T + i] = 1. / minTree[j][i];
            // slack variable
            A[i + P][T * P + P + i] = -1;
            
            B[i + P] = 1;
        }
        
  
        // Solution
        vector <double> X;
        
        double obj;

        int result = simplex (A, B, C, X, obj);
        
        cout.setf(ios::fixed);
        cout.precision(2);
        
        cout << "Test case #" << testCase << ": ";
        // Infeasible
        if (result == 0)
            cout << "IMPOSSIBLE" << endl;
        else
        {
            // Since we are in a minimization problem, we change the sign
            double numMin = -obj;
            if (numMin <= S)
                cout << "RIGHT" << endl;
            else
                cout << numMin - S << endl;
        }

    }
}
