#ifndef __ShapeAnalyzer__FunctionalMaps__
#define __ShapeAnalyzer__FunctionalMaps__

#include "Shape.h"

#include <vtkDoubleArray.h>

#include <vector>

using namespace std;

/// \brief Abstract class for the transfer of a function from one shape to another using the functional maps approach.
class FunctionalMaps {
public:
    /// \brief Constructor
    /// @param Shape* Source shape.
    /// @param Shape* Target shape.
    /// @param vector<vtkSmartPointer<vtkDoubleArray>>& Vector of constraints (Vertex-to-Double maps) defined on the source shape.
    /// @param vector<vtkSmartPointer<vtkDoubleArray>>& Vector of constraints (Vertex-to-Double maps) defined on the target shape.
    /// @param int Number of eigenfunctions that are used for the representation of the functions.
    FunctionalMaps(shared_ptr<Shape> shape1,
                   shared_ptr<Shape> shape2,
                   vector<vtkSmartPointer<vtkDoubleArray>>& c1,
                   vector<vtkSmartPointer<vtkDoubleArray>>& c2,
                   int numberOfEigenfunctions
                   ) :  shape1_(shape1),
                        shape2_(shape2),
                        c1_(c1),
                        c2_(c2),
                        numberOfEigenfunctions_(numberOfEigenfunctions)
    {
        
        ;
        
        if (shape1_ == nullptr) {
            throw invalid_argument(string("Null pointer input 'shape1' in ").append(__PRETTY_FUNCTION__));
        }
        if (shape2_ == nullptr) {
            throw invalid_argument(string("Null pointer input 'shape2' in ").append(__PRETTY_FUNCTION__));
        }
        
        numberOfConstraints_ = c1_.size();
        if(c1_.size() != c2_.size()) {
            throw invalid_argument(string("Number of constraints of the given shapes c1.size() and c2.size() do not coincide in ").append(__PRETTY_FUNCTION__));
        }
        
        for(int i = 0; i < c1_.size(); i++) {
            if(c1_.at(i)->GetNumberOfTuples() != shape1_->getPolyData()->GetNumberOfPoints()) {
                throw invalid_argument(string("Number of tuples in c1[" + to_string(i) + "] and number of points of shape1 do not coincide in ").append(__PRETTY_FUNCTION__));
            }

            if(c2_.at(i)->GetNumberOfTuples() != shape2_->getPolyData()->GetNumberOfPoints()) {
                throw invalid_argument(string("Number of tuples in c2[" + to_string(i) + "] and number of points of shape2 do not coincide in ").append(__PRETTY_FUNCTION__));
            }
            
            if(c2_.at(i)->GetNumberOfTuples() != shape2_->getPolyData()->GetNumberOfPoints()) {
                throw invalid_argument(string("Number of components of contraint c1[" + to_string(i) + "] is not equal to 1 in ").append(__PRETTY_FUNCTION__));
            }
            
            if(c2_.at(i)->GetNumberOfComponents() != 1) {
                throw invalid_argument(string("Number of components of contraint c2[" + to_string(i) + "] is not equal to 1 in ").append(__PRETTY_FUNCTION__));
            }
            
            if(c1_.at(i)->GetNumberOfComponents() != 1) {
                throw invalid_argument(string("Number of components of contraint c1[" + to_string(i) + "] is not equal to 1 in ").append(__PRETTY_FUNCTION__));
            }

            if(c2_.at(i)->GetNumberOfComponents() != 1) {
                throw invalid_argument(string("Number of components of contraint c2[" + to_string(i) + "] is not equal to 1 in ").append(__PRETTY_FUNCTION__));
            }
        }
        
    }
    
    /// \brief Computes correspondence in functional maps representation.
    virtual void computeCorrespondence() = 0;
    
    /// \brief Transfers and returns a scalar valued function f from the source to the target shape.
    virtual vtkSmartPointer<vtkDoubleArray> transferFunction(vtkSmartPointer<vtkDataArray> f) = 0;
    
    virtual ~FunctionalMaps() {}

protected:
    shared_ptr<Shape> shape1_;
    shared_ptr<Shape> shape2_;
    vector<vtkSmartPointer<vtkDoubleArray>>& c1_;
    vector<vtkSmartPointer<vtkDoubleArray>>& c2_;
    int numberOfEigenfunctions_;
    int numberOfConstraints_;
};


#endif /* defined(__ShapeAnalyzer__FunctionalMaps__) */

