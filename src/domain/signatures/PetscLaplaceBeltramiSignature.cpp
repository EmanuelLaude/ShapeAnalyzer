#include "PetscLaplaceBeltramiSignature.h"

///////////////////////////////////////////////////////////////////////////////
signature::PetscLaplaceBeltramiSignature::PetscLaplaceBeltramiSignature(shared_ptr<Shape> shape, int dimension, shared_ptr<PetscLaplaceBeltramiOperator> laplacian, int numberOfEigenfunctions) : Signature(shape, dimension), laplacian_(laplacian), numberOfEigenfunctions_(numberOfEigenfunctions) {
    // argument check
    if (laplacian_ == nullptr) {
        throw invalid_argument(string("Null pointer input 'laplacian' in ").append(__PRETTY_FUNCTION__));
    }
    MatCreateSeqDense(MPI_COMM_SELF, dimension_, shape_->getPolyData()->GetNumberOfPoints(), NULL, &signature_);
}


///////////////////////////////////////////////////////////////////////////////
signature::PetscLaplaceBeltramiSignature::~PetscLaplaceBeltramiSignature() {
    MatDestroy(&signature_);
}


///////////////////////////////////////////////////////////////////////////////
vtkSmartPointer<vtkDoubleArray> signature::PetscLaplaceBeltramiSignature::getComponent(int i) {
    if(i >= dimension_) {
        throw invalid_argument(string("i = ") + to_string(i) + " exceeds dimension of signature in " + __PRETTY_FUNCTION__);
    }
    
    const PetscScalar* row;
    MatGetRow(signature_, i, NULL, NULL, &row);
    vtkSmartPointer<vtkDoubleArray> component = vtkSmartPointer<vtkDoubleArray>::New();
    component->SetNumberOfValues(shape_->getPolyData()->GetNumberOfPoints());
    for(vtkIdType i = 0; i < shape_->getPolyData()->GetNumberOfPoints(); i++) {
        component->SetValue(i, row[i]);
    }
    MatRestoreRow(signature_, i, NULL, NULL, &row);
    
    return component;
}


///////////////////////////////////////////////////////////////////////////////
void signature::PetscLaplaceBeltramiSignature::getComponent(int i, Vec *component) {
    if(i >= dimension_) {
        throw invalid_argument(string("i = ") + to_string(i) + " exceeds dimension of signature in " + __PRETTY_FUNCTION__);
    }
    
    PetscHelper::getRow(*component, signature_, i);
}