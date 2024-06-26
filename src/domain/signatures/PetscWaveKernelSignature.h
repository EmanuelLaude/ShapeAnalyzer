#ifndef __ShapeAnalyzer__PetscWaveKernelSignature__
#define __ShapeAnalyzer__PetscWaveKernelSignature__

#include "PetscLaplaceBeltramiSignature.h"
#include "../Shape.h"
#include "../laplaceBeltrami/PetscLaplaceBeltramiOperator.h"
#include "../../custom/Factory.h"

#include <vtkPolyData.h>

#include <cmath>

using namespace laplaceBeltrami;
using namespace signature;

namespace signature {
    
    /// \brief Concrete class for computation of the Wave Kernel signature. Implementation adapted from MATLAB code taken from http://www.di.ens.fr/~aubry/wks.html
    
    class PetscWaveKernelSignature : public PetscLaplaceBeltramiSignature {
    public:
        /// \brief Constructor.
        /// @param Shape* The shape on which the signature is computed.
        /// @param int Number of components that the signature should have.
        /// @param PetscLaplaceBeltramiOperator* A reference to an instance of a PETSc based Laplace-Beltrami operator
        /// @param double Optional paramter controlling the variance.
        PetscWaveKernelSignature(shared_ptr<Shape> shape, int dimension, shared_ptr<PetscLaplaceBeltramiOperator> laplacian, int numberOfEigenfunctions, double wksVariance = 6.0);
        
        virtual ~PetscWaveKernelSignature() {}
    private:
        double wksVariance_;
    };
    
}

#endif /* defined(__ShapeAnalyzer__PetscWaveKernelSignature__) */
