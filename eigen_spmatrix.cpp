#include <Eigen/Sparse>
#include <vector>
#include <iostream>
using namespace std;
typedef Eigen::SparseMatrix<double> SpMat; // declares a column-major sparse matrix type of double
typedef Eigen::Triplet<double> T;

int main(int argc, char** argv)
{
  if(argc!=1) {
    cerr << "Error: expected one and only one argument.\n";
    return -1;
  }
  
  int n = 3;
  // Assembly:
  std::vector<T> coeffs;            // list of non-zeros coefficients
  coeffs.push_back(T(0,0,2));
  coeffs.push_back(T(0,1,1));
  coeffs.push_back(T(1,0,3));
  coeffs.push_back(T(1,1,1));
  coeffs.push_back(T(1,2,2));
  coeffs.push_back(T(2,2,2));
  coeffs.push_back(T(2,1,3));
  Eigen::VectorXd b(n);                   // the right hand side-vector resulting from the constraints
  b(0)=1; b(1)=0; b(2)=4;
  SpMat A(n, n);
  A.setFromTriplets(coeffs.begin(), coeffs.end());

  // Solving:
  Eigen::SimplicialCholesky<SpMat> chol(A);  // performs a Cholesky factorization of A
  Eigen::VectorXd x = chol.solve(b);         // use the factorization to solve for the given right hand side
  cout<<A<<endl;
  cout<<"=====solver========="<<endl;
  cout<<x<<endl;
  cout<<"============\n";
  cout<<b<<endl;
  return 0;
}

