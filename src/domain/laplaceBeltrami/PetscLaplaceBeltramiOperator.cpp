#include "PetscLaplaceBeltramiOperator.h"

///////////////////////////////////////////////////////////////////////////////
void laplaceBeltrami::PetscLaplaceBeltramiOperator::getEigenfunctionMatrix(Mat *Phi) {
    PetscInt numberOfEigenfunctions;
    MatGetSize(*Phi, NULL, &numberOfEigenfunctions);
    if(numberOfEigenfunctions > numberOfEigenfunctions_) {
        throw invalid_argument(string("Cannot add more eigenfunctions than initialized before to eigenfunction matrix in ").append(__PRETTY_FUNCTION__));
    }
    
    for(PetscInt i = 0; i < numberOfEigenfunctions; i++) {
        Vec phi;
        getEigenfunction(i, &phi);
        
        PetscHelper::setColumn(*Phi, phi, i);
        
        VecDestroy(&phi);
    }
    
    MatAssemblyBegin(*Phi, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(*Phi, MAT_FINAL_ASSEMBLY);
}
