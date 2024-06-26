#ifndef __ShapeAnalyzer__Correspondence__
#define __ShapeAnalyzer__Correspondence__

#include <vtkType.h>

#include <vector>
#include <iostream>

#include "../Shape.h"

using namespace std;

/// \brief Namespace grouping all correspondence related classes.
namespace correspondences {
    
    
    ///
    /// \brief Abstract class to store correspondence information.
    ///
    /// \details Correspondence matches data ids of an arbitrary number of shapes
    /// to each other. Each Correspondence has a unique id and stores a vector
    /// of shapes and a vector of data ids with the same size. The ids on the same
    /// position of each vector belong to each other.
    /// It is possible to add more data after initialization and to delete it.
    ///
    /// \author Emanuel Laude and Zorah Lähner
    ///
    class Correspondence {
        
    public:
        /// \brief Adds a new shape and data id to the object.
        void addShape(shared_ptr<Shape> shape, vtkIdType correspondingId);
        
        /// \brief Removes a shape plus corresponding data id pair from the object.
        /// @return Returns index i of the deleted shape. If shape is not part of correspondence it returns -1.
        /// @throws invalid_argument if the function is called when the correspondence is binary (2 shapes).
        int removeShape(Shape* shape);
        
        /// \brief Clears the Correspondence without deleting the object.
        void clear();
        
        /// \brief Returns a pointer to the vector of shape ids that are in the
        /// object.
        const vector<shared_ptr<Shape>>& getShapes() const {
            return shapes_;
        }
        
        /// \brief Returns a pointer to the vector of data ids that are in the
        /// object.
        const vector<vtkIdType>& getCorrespondingIds() const {
            return correspondingIds_;
        }
        
        /// \brief Returns the number of shapes that have corresponding data.
        int size() const {
            return shapes_.size();
        }
        
        
        /// Returns the id of this object.
        vtkIdType getId() const {
            return id_;
        }
        
        /// Returns a string with all information of this CorrespondenceData.
        /// @return The resulting string has the form: \n
        /// Type: _T_ \n
        /// ID: _ID_ \n
        /// S _SID_ : _DID_ -> ... -> S _SID_ : _DID_ \n
        /// where _T_ is the type of the object (has to be implemented in the subclass)
        /// _ID_ is the id of the object, _SID_ is the id of a shape in this object,
        /// _DID_ is the id of the data in the shape with _SID_.
        virtual string toString() = 0;
        
        /// Empty Destructor.
        virtual ~Correspondence() {
        }
    protected:
        /// Protected Constructor.
        /// @param id Will be the Id of this object. It should be unique, but no further checking is
        /// made.
        Correspondence(vtkIdType id) : id_(id) {}
        
        
        
        /// \brief Vector of the ids of the corresponding shapes.
        vector<shared_ptr<Shape>> shapes_;
        /// \brief Vector of the ids of the corresponding data.
        /// \details The id at position i refers to data in the shape with
        /// the shape id found at shapeIds_[i].
        vector<vtkIdType> correspondingIds_; // either face or point ids depending on the implementing class
        /// \brief correspondence id.
        vtkIdType id_;
    };
}
#endif /* defined(__ShapeAnalyzer__Correspondence__) */
