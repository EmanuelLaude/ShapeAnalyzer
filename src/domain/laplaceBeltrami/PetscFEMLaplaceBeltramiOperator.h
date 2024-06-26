#ifndef ShapeAnalyzer_PetscFEMLaplaceBeltramiOperator_h
#define ShapeAnalyzer_PetscFEMLaplaceBeltramiOperator_h

#include "PetscLaplaceBeltramiOperator.h"

#include <slepceps.h>

#include <vtkTriangle.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <set>
#include <iostream>

using namespace std;

namespace laplaceBeltrami {

    /// \brief Concrete finite element based Laplace-Beltrami operator implementation.
    /// \details Implementation is based on PETSc and SLEPc framework.
    class PetscFEMLaplaceBeltramiOperator : public PetscLaplaceBeltramiOperator {
    public:
        /// \brief Constructor
        /// @param Shape* The shape which serves as the domain to which the Laplace-Beltrami operator is applied
        /// @param int The number of eigenpairs that should be computed
        PetscFEMLaplaceBeltramiOperator(shared_ptr<Shape> shape, int numberOfEigenfunctions);
        
        virtual double getEigenvalue(int i);
        
        virtual vtkSmartPointer<vtkDoubleArray> getEigenfunction(int i);
        
        
        virtual void getEigenfunction(PetscInt i, Vec* phi);
        
        virtual void getEigenpair(PetscInt i, Vec* phi, PetscScalar* lambda);
        
        //returns a reference to mass matrix. LaplaceOperator object itself is responsible for deleting the Mat object.
        virtual Mat* getMassMatrix();
        
        Mat* getStiffnessMatrix();
        
        virtual ~PetscFEMLaplaceBeltramiOperator();
        
    private:
        /// Computes stiffness and mass matrices
        void setupMatrices();
        
        /// \brief Returns number of non-zero elements per row
        /// \details needed for efficient allocation of sparse matrices
        inline void getNnz(PetscInt *nnz, vtkIdType numberOfPoints, vtkIdType numberOfFaces);
        
        /// \brief Returns mass of triangle a--b--c
        /// @param double* 3D coordinates of point a.
        /// @param double* 3D coordinates of point b.
        /// @param double* 3D coordinates of point c.
        inline PetscScalar getMass(double* a, double* b, double* c);
        
        /// \brief Returns stiffness of triangle a--b--c
        /// @param double* 3D coordinates of point a.
        /// @param double* 3D coordinates of point b.
        /// @param double* 3D coordinates of point c.
        inline PetscScalar getStiffness(double* a, double* b, double* c);
        
        /// \brief Stiffness matrix.
        Mat L_;
        
        /// \brief Mass martix.
        Mat M_;
        
        /// \brief Eigenproblem solver context
        EPS eps_;
        
        ST st_;
    };
}

#endif
