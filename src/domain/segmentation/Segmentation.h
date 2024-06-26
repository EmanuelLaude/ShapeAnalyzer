#ifndef __ShapeAnalyzer__Segmentation__
#define __ShapeAnalyzer__Segmentation__

#include "vtkIntArray.h"
#include "vtkSmartPointer.h"

#include "../Shape.h"

/// \brief Namespace grouping all segmentation classes.
namespace segmentation {
    /// \brief Abstract class for computation of segments on a shape.
    class Segmentation {
    public:
        
        /// \brief Constructor.
        /// @param Shape* The shape on which a segmentation should be computed.
        Segmentation(shared_ptr<Shape> shape) : shape_(shape) {
            if (shape == nullptr) {
                throw invalid_argument(string("Null pointer input 'shape' in ").append(__PRETTY_FUNCTION__));
            }
        }
        
        /// \brief Virtual destructor.
        virtual ~Segmentation() {}
        
        /// \brief Returns vertex-to-segment-ID mapping.
        virtual vtkSmartPointer<vtkIntArray> getSegments() = 0;
        
    protected:
        /// Reference to the shape the segmentation is calculated on.
        shared_ptr<Shape> shape_;
    };
    
}

#endif /* defined(__ShapeAnalyzer__Segmentation__) */
