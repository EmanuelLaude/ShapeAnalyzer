#ifndef __ShapeAnalyzer__ShapeSampling__
#define __ShapeAnalyzer__ShapeSampling__

#include <iostream>

#include <vtkIdList.h>
#include <vtkSmartPointer.h>

#include "../Shape.h"

using namespace std;

/// \brief Namespace grouping all sampling classes.
namespace sampling {
    
    ///
    /// \brief Abstract class for sub sampling points on shapes.
    ///
    class Sampling {
        
    public:
        /// \brief Basic constructor.
        Sampling(shared_ptr<Shape> shape) : shape_(shape) {
            if (shape == nullptr) {
                throw invalid_argument(string("Null pointer input 'shape' in ").append(__PRETTY_FUNCTION__));
            }
        }
        
        /// Virtual empty destructor.
        virtual ~Sampling() {}
        
        /// \brief Returns a list of the ids of the sampled points.
        virtual vtkSmartPointer<vtkIdList> getPoints() = 0;
        
        /// \brief Returns a pointer to the shape.
        shared_ptr<Shape>  getShape()      { return shape_; }
        
    protected:
        /// \brief The shape that is sampled.
        shared_ptr<Shape> shape_;
    };
    
}

#endif /* defined(__ShapeAnalyzer__ShapeSampling__) */
