#ifndef __ShapeAnalyzer__Signature__
#define __ShapeAnalyzer__Signature__

#include "../Shape.h"

/// Namespace grouping all signature classes.
namespace signature {
    /// \brief Abstract class for computation of signatures.
    class Signature {
    public:
        /// \brief Signature constructor.
        /// @param Shape* The shape on which the signature is computed.
        /// @param int Number of components that the signature should have.
        Signature(shared_ptr<Shape> shape, int dimension) : shape_(shape), dimension_(dimension) {
            if (shape == nullptr) {
                throw invalid_argument(string("Null pointer input 'shape' in ").append(__PRETTY_FUNCTION__));
            }
            if (dimension_ <= 0) {
                throw invalid_argument(string("Input dimension not positive in ").append(__PRETTY_FUNCTION__));
            }
        }
        
        /// \brief Virtual destructor.
        virtual ~Signature() {}
        
        /// \brief Returns the number of components of the signature that should be computed.
        int getDimension() {
            return dimension_;
        }
        
        /// \brief Returns i-th component of the signature.
        /// @param int Index of the component
        virtual vtkSmartPointer<vtkDoubleArray> getComponent(int i) = 0;
        
        
    protected:
        /// Reference to the shape the signature is from
        shared_ptr<Shape> shape_;
        /// Number of components of the signature
        int dimension_;
    };
}

#endif /* defined(__ShapeAnalyzer__Signature__) */
