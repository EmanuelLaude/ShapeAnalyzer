#ifndef __ShapeAnalyzer__PetscLaplaceBeltramiOperator__
#define __ShapeAnalyzer__PetscLaplaceBeltramiOperator__

#include <slepceps.h>

#include "../PetscHelper.h"

#include "LaplaceBeltramiOperator.h"
#include "LaplaceBeltramiError.h"


namespace laplaceBeltrami {
    
    /// \brief Abstract class for all Laplace-Beltrami operators that are based on the PETSc and SLEPc framework.
    class PetscLaplaceBeltramiOperator : public LaplaceBeltramiOperator {
    public:
        /// \brief Constructor
        /// @param Shape* The shape which serves as the domain to which the Laplace-Beltrami operator is applied
        /// @param int The number of eigenpairs that should be computed
        PetscLaplaceBeltramiOperator(shared_ptr<Shape> shape, int numberOfEigenfunctions) : LaplaceBeltramiOperator(shape, numberOfEigenfunctions) {}
        
        // Tell the compiler that both getEigenfunction methods (the one from the Base class and the one declared here) are used since they are different
        using LaplaceBeltramiOperator::getEigenfunction;
        
        /// \brief Returns i-th eigenfunction as PETSc vector.
        /// @param PetscInt index of the eigenfunction that should be returned
        /// @param Vec* return parameter.
        virtual void getEigenfunction(PetscInt i, Vec* phi) = 0;
        
        /// \brief Returns i-th eigenpair
        /// @param PetscInt index of the eigenpair that should be returned.
        /// @param Vec* Return parameter for the i-th eigenfunction.
        /// @param PetscScalar* Return parameter for the i-th eigenvalue.
        virtual void getEigenpair(PetscInt i, Vec* phi, PetscScalar* lambda) = 0;
        
        /// \brief Returns matrix Phi of dimension 'numberOfEigenfunctions x numberOfPoints' containing the eigenfunctions as columns.
        /// @param Mat* Return parameter of type Petscmatrix that contains the eigenfunctions.
        virtual void getEigenfunctionMatrix(Mat* Phi);
        
        /// \brief Returns a reference to the mass matrix used internally.
        /// \details Laplace-Beltrami operator object itself is responsible for deleting the Mat object.
        /// @return Mat* Mass matrix as Petscmatrix.
        virtual Mat* getMassMatrix() = 0;
    };
    
}
#endif /* defined(__ShapeAnalyzer__PetscLaplaceBeltramiOperator__) */
