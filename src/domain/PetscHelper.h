#ifndef __ShapeAnalyzer__PetscHelper__
#define __ShapeAnalyzer__PetscHelper__

#include <string>
#include <exception>
#include <stdexcept>

#include <slepceps.h>

#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>

/// \brief A Helper class containing a set of static functions for the creation and manipulation of PETSc vectors and matrices.
class PetscHelper {
public:
    /// \brief Sets block B in matrix A. Element A(i, j) will be set to B(0,0) and so on...
    static void setBlock(Mat& A, Mat& B, PetscInt i, PetscInt j);
    
    /// \brief set i-th row a_i in matrix A
    static void setRow(Mat& A, Vec& ai, PetscInt i);

    /// \brief Returns i-th row a_i in matrix A
    static void getRow(Vec& ai, Mat& A, PetscInt i);
    
    /// \brief Sets i-th column a_i in matrix A
    static void setColumn(Mat& A, Vec& ai, PetscInt i);
    
    /// \brief Reshapes n*m-dimensional vector b as matrix B with m rows and n columns. Elements are filled row wise into matrix B.
    static void reshape(Mat& B, Vec& b, PetscInt m, PetscInt n);
    
    /// \brief Converts a vtkDataArray to a Petsc vector
    static void vtkDataArrayToPetscVec(vtkSmartPointer<vtkDataArray> arr, Vec &vec);
    
    /// \brief Converts a PETSc vector to a vtkDoubleArray
    static vtkSmartPointer<vtkDoubleArray> petscVecToVtkDoubleArray(Vec& vec);
    
    /// \brief Converts a C++ array of type PetscScalar* to a vtkDoubleArray
    static vtkSmartPointer<vtkDoubleArray> petscScalarArrayToVtkDoubleArray(const PetscScalar *array, PetscInt size);
};

#endif /* defined(__ShapeAnalyzer__PetscHelper__) */
