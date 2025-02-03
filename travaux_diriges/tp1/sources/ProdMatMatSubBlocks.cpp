#include <algorithm>
#include <cassert>
#include <iostream>
#include <thread>
#if defined(_OPENMP)
#include <omp.h>
#endif
#include "ProdMatMatSubBlocks.hpp"

namespace {
void prodSubBlocks(int iRowBlkA, int iColBlkB, int iColBlkA, int szBlock,
                   const Matrix& A, const Matrix& B, Matrix& C) {
  
  /* ordre origine */

  /*
  for (int i = iRowBlkA; i < std::min(A.nbRows, iRowBlkA + szBlock); ++i)
    for (int k = iColBlkA; k < std::min(A.nbCols, iColBlkA + szBlock); k++)
      for (int j = iColBlkB; j < std::min(B.nbCols, iColBlkB + szBlock); j++)
        C(i, j) += A(i, k) * B(k, j);
  */

  int i,j,k;
  #pragma omp parallel for private(j,k,i) // on protège les indices contre des accès simultanés
  for (int j = iColBlkB; j < std::min(B.nbCols, iColBlkB + szBlock); j++)
    for (int k = iColBlkA; k < std::min(A.nbCols, iColBlkA + szBlock); k++)
      for (int i = iRowBlkA; i < std::min(A.nbRows, iRowBlkA + szBlock); ++i)
        C(i, j) += A(i, k) * B(k, j);
}
const int szBlock = 512;
}  // namespace

Matrix operator*(const Matrix& A, const Matrix& B) {
  Matrix C(A.nbRows, B.nbCols, 0.0);

  /* Produit matrice-matrice par blocs */
  for (int J=0; J < B.nbCols/szBlock; J++){
    for (int K=0; K < A.nbCols/szBlock; K++){
      for (int I=0; I < A.nbRows/szBlock; I++){
        prodSubBlocks(I*szBlock, J*szBlock, K*szBlock, szBlock, A, B, C);
      }
    }
  }
  return C;
}
