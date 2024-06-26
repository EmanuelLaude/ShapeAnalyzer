#ifndef __ShapeAnalyzer__PointCorrespondence__
#define __ShapeAnalyzer__PointCorrespondence__

#include "Correspondence.h"

namespace correspondences {
    ///
    /// \brief Stores multi correspondences between points of an arbitrary number of shapes.
    /// \details The main implementation can be seen in Correspondence, this class
    /// purely insures that point and face correspondences can not be mixed up in some
    /// data structures (especially pointCorrespondence_ in ShapeAnalyzer).
    ///
    class PointCorrespondence : public Correspondence {
    public:
        /// \brief Basic Constructor.
        /// @param id The final id of this correspondence data. It should be unique
        /// among all correspondence data, but this is not enforced.
        PointCorrespondence(vtkIdType id) : Correspondence(id) {
        }
        
        virtual string toString();
    };
}
#endif /* defined(__ShapeAnalyzer__PointCorrespondence__) */
