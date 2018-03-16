#ifndef MATRIXMULT_HPP
#define MATRIXMULT_HPP
#include <iostream>
#include <vector>

using namespace std;

//template <class T>
class MatrixOperations {
public:
  vector<vector<int> > matrixA;
  vector<vector<int> > matrixB;
  MatrixOperations(const vector<vector<int> > &a, const vector<vector<int> > &b)  {
   matrixA = a;
   matrixB = b;
  }

  ~MatrixOperations(){
    
        for(vector<int> v : matrixA){
            v.clear();
        }
        matrixA.clear();
        
        for(vector<int> v : matrixB){
            v.clear();
        }

        matrixB.clear();
  }


  vector<vector<int> > matrixMultiply()
  {
    const int dim1 = matrixA.size();     // a rows
    const int dim2 = matrixA[0].size();  // a cols
    const int dim3 = matrixB[0].size();  // b cols

    vector<vector<int> > prodMatrix(dim1,vector<int>(dim3, 0));
    for (int itop = 0; itop < dim1; itop+=32)
    {
        for (int jtop = 0; jtop < dim3; jtop+=32)
        {
            for (int ktop = 0; ktop < dim2; ktop+=32)
            {
		for (int i = itop; i<((itop+32)>dim1?dim1:(itop+32));i++){

		  for (int j = jtop; j<((jtop+32)>dim3?dim3:(jtop+32));j++) {

		    for (int k = ktop; k<((ktop+32)>dim2?dim2:(ktop+32));k++) {

                	prodMatrix[i][j] += matrixA[i][k] * matrixB[k][j];
		    }
		  }
		}
            }
        }
    }
    return prodMatrix;
  }


};

#endif
