#ifndef OptimT_STUPIDLU_HPP
#define OptimT_STUPIDLU_HPP
#include "../OptimTemplates/Global"
#include <type_traits>
#include <assert.h>
#include "MatrixDynamicSize.hpp"
#include "MatrixFixedSize.hpp"
#include "MatrixMap.hpp"
namespace OptimT {

/**
 * @brief A simple LU implementation. 
 *      This function is written only to enable NSGA3 to run without Eigen. 
 *      For high-performance inverse matrix, please use Eigen.
 * 
 * @tparam Mat_t Type of matrix
 * @param src source of matrix
 * @param dst result
 */

template<typename Scalar_t,size_t N>
using SquareMat_t = typename std::conditional<N==Dynamic,MatrixDynamicSize<Scalar_t>,MatrixFixedSize<Scalar_t,N,N>>::type;

template<typename Scalar_t,size_t Size=Dynamic>
void MatrixProduct(const SquareMat_t<Scalar_t,Size> & A,
    const SquareMat_t<Scalar_t,Size> & B,
    SquareMat_t<Scalar_t,Size> * res) {

const size_t N=A->rows();
if constexpr (Size==Dynamic) {
res->resize(N,N);
}

for(size_t r=0;r<N;r++) {
    for(size_t c=0;c<N;c++) {
        Scalar_t sum=0;
        for(size_t k=0;k<N;k++) {
            sum+=A(r,k)*B(k,c);
        }
        res->operator()(r,c)=sum;
    }
}

}


template<typename Scalar_t,size_t Size=Dynamic>
void InverseMatrix_LU(const SquareMat_t<Scalar_t,Size> & A,SquareMat_t<Scalar_t,Size> * invA) {

assert(A.rows()==A.cols());

SquareMat_t<Scalar_t,Size> L,U,iL,iU;
const size_t N=A.rows();
if constexpr (Size==Dynamic) {
L.resize(N,N);
U.resize(N,N);
iL.resize(N,N);
iU.resize(N,N);
}

L.fill(0);
U.fill(0);

for(size_t i=0;i<N;i++) {
    L(i,i)=1;
}


///compute L and U
for(size_t k=0;i<N;k++) {
    for(size_t j=k;j<N;j++) {
        Scalar_t temp=0;
        for(size_t di=0;di<k-1;di++) {
            temp+=L(k,di)*U(di,j);
        }
        U(k,j)=A(k,j)-temp;
    }

    for(size_t i=k;i<N;i++) {
        Scalar_t temp=0;
        for(size_t di=0;di<k-1;di++) {
            temp+=L(i,di)*U(di,k);
        }
        L(i,k)=(A(i,k)-temp)/U(k,k);
    }
}

iL.fill(0);
iU.fill(0);
///compute iL and iU
for(size_t j=0;j<N;j++) {
    for(size_t i=0;i<j;i++) {
        Scalar_t temp=0;
        for(size_t k=i+1;k<=j;k++) {
            temp+=iL(j,k)*L(k,i);
        }
        iL(j,i)=-iL(i,i)*temp;
    }
    iL(j,j)=1/L(j,j);
}


for(size_t j=0;j<N;j++) {
        iU(j,j)=1/U(j,j);
    for(size_t i=j+1;i<N;i++) {
        Scalar_t temp=0;
        for(size_t k=j+1;k<i;k++) {
            temp+=U(j,k)*iU(k,i);
        }
        iU(j,i)=-1/U(j,j)*temp;
    }
}

MatrixProduct(iU,iL,invA);

}

}   //  namespace OptimT

#endif  //  OptimT_STUPIDLU_HPP