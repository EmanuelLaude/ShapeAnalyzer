#ifndef __ShapeAnalyzer__PetscHeatDiffusion__
#define __ShapeAnalyzer__PetscHeatDiffusion__

#include <slepceps.h>

#include <vtkPolyData.h>

#include "Shape.h"
#include "laplaceBeltrami/PetscFEMLaplaceBeltramiOperator.h"
#include "PetscHelper.h"
#include "HeatDiffusion.h"

using namespace laplaceBeltrami;

/// \brief Concrete implementation of the HeatDiffusion interface.
class PetscHeatDiffusion : public HeatDiffusion {
public:
    /// \brief Constructor
    /// @param Shape* The shape to which the heat diffusion process is applied.
    /// @param PetscLaplaceBeltramiOperator* The laplacian of the shape.
    /// @param vtkSmartPointer<vtkDoubleArray> The initial condition as an array of values each corresponding to the vertex with the same index
    PetscHeatDiffusion(shared_ptr<Shape> shape, shared_ptr<PetscLaplaceBeltramiOperator> laplacian, vtkSmartPointer<vtkDoubleArray> initialCondition);
    /// \brief Virtual destructor.
    virtual ~PetscHeatDiffusion();
    vtkSmartPointer<vtkDoubleArray> getHeat(double t);
private:
    shared_ptr<PetscLaplaceBeltramiOperator> laplacian_;

    /// \brief The Vector Phi^T * M * u0
    Vec PhiTMu0_;
};

#endif /* defined(__ShapeAnalyzer__PetscHeatDiffusion__) */
