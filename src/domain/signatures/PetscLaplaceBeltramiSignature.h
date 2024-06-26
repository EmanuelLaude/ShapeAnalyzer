#ifndef __ShapeAnalyzer__PetscLaplaceBeltramiSignature__
#define __ShapeAnalyzer__PetscLaplaceBeltramiSignature__

#include "Signature.h"

#include "../laplaceBeltrami/PetscLaplaceBeltramiOperator.h"

using namespace laplaceBeltrami;

namespace signature {
    /// \brief Abstract class for Laplace-Beltrami based signatures that use the PETSc framework.
    class PetscLaplaceBeltramiSignature : public Signature {
    public:
        /// \brief Constructor.
        /// @param Shape* The shape on which the signature is computed.
        /// @param int Number of components that the signature should have.
        /// @param PetscLaplaceBeltramiOperator* A reference to an instance of a PETSc based Laplace-Beltrami operator
        PetscLaplaceBeltramiSignature(shared_ptr<Shape> shape, int dimension, shared_ptr<PetscLaplaceBeltramiOperator> laplacian, int numberOfEigenfunctions);
        
        /// \brief Virtual destructor
        virtual ~PetscLaplaceBeltramiSignature();
        
        void getComponent(int i, Vec* component);
        virtual vtkSmartPointer<vtkDoubleArray> getComponent(int i);
    protected:
        /// Petsc matrix of the signature
        Mat signature_;
        /// Reference to the LPO of the shape.
        shared_ptr<PetscLaplaceBeltramiOperator> laplacian_;
        
        int numberOfEigenfunctions_;
    };
    
}

#endif /* defined(__ShapeAnalyzer__PetscLaplaceBeltramiSignature__) */
