#ifndef ShapeAnalyzer_LaplaceBeltramiOperator_h
#define ShapeAnalyzer_LaplaceBeltramiOperator_h

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkDoubleArray.h>


#include "../Shape.h"

/// \brief Namespace grouping all Laplace-Beltrami operator classes.
namespace laplaceBeltrami {

    /// \brief Abstract class for the implementation of different discretizations of the Laplace-Beltrami operator.
    /// \details Implementing classes provide functionality for the computation of eigenvectors and eigenvalues for a given shape. This abstract class is independent of the underlying framework. Laplace-Beltrami discretizations that utilize the PETSc and SLEPc framework should be derived from the subclass PetscLaplaceBeltramiOperator
    class LaplaceBeltramiOperator {
    public:
        /// \brief Constructor
        /// @param Shape* The shape which serves as the domain to which the Laplace-Beltrami operator is applied
        /// @param int The number of eigenpairs that should be computed
        LaplaceBeltramiOperator(shared_ptr<Shape> shape, int numberOfEigenfunctions) : shape_(shape), numberOfEigenfunctions_(numberOfEigenfunctions) {
            if (shape == nullptr) {
                throw invalid_argument(string("Null pointer input 'shape' in ").append(__PRETTY_FUNCTION__));
            }
        }
        
        /// \brief Virtual destructor.
        virtual ~LaplaceBeltramiOperator() {
        }
        
        /// \brief Returns i-th eigenfunction.
        /// @param int The index of the eigenfunction that should be returned.
        virtual vtkSmartPointer<vtkDoubleArray> getEigenfunction(int i) = 0;
        
        
        /// \brief Returns i-th eigenvalue.
        /// @param int The index of the eigenvalue that should be returned.
        virtual double getEigenvalue(int i) = 0;
        
        
        /// \brief Returns number of eigenpairs that are computed.
        int getNumberOfEigenfunctions() {
            return numberOfEigenfunctions_;
        }
    protected:
        /// Pointer to the shape which serves as the domain to which the Laplace-Beltrami operator is applied
        shared_ptr<Shape> shape_;
        
        /// Number of eigenpairs that are computed.
        int numberOfEigenfunctions_;
    };
    
}

#endif
