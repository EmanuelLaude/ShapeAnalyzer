#ifndef __ShapeAnalyzer__HeatDiffusion__
#define __ShapeAnalyzer__HeatDiffusion__


#include <vtkPolyData.h>

#include "Shape.h"

using namespace laplaceBeltrami;

/// \brief Abstract class for implementation of the heat diffusion process on a shape.
/// \details Implementing classes provide functionality for the computation of a distribution
/// of heat after time t given an initial distribution of heat at time t=0.
class HeatDiffusion {
public:
    /// \brief Constructor
    /// @param Shape* The shape to which the heat diffusion process is applied.
    /// @param vtkSmartPointer<vtkDoubleArray> The initial condition as an array of values each corresponding to the vertex with the same index
    HeatDiffusion(shared_ptr<Shape> shape, vtkSmartPointer<vtkDoubleArray> initialCondition) : shape_(shape), initialCondition_(initialCondition) {}
    
    /// \brief Virtual destructor.
    virtual ~HeatDiffusion() {}
    
    /// \brief Returns the distribution of heat after time t.
    /// @param double The time t at which the heat distribution shall be computed.
    virtual vtkSmartPointer<vtkDoubleArray> getHeat(double t) = 0;
protected:
    shared_ptr<Shape> shape_;
    vtkSmartPointer<vtkDoubleArray> initialCondition_;
};

#endif /* defined(__ShapeAnalyzer__HeatDiffusion__) */
