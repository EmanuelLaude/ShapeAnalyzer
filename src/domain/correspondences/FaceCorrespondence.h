#ifndef __ShapeAnalyzer__FaceCorrespondence__
#define __ShapeAnalyzer__FaceCorrespondence__

#include "Correspondence.h"

namespace correspondences {
    ///
    /// \brief Stores multi correspondences between faces of an arbitrary number of shapes.
    /// \details The main implementation can be seen in Correspondence, this class
    /// purely insures that point and face correspondences can not be mixed up in some
    /// data structures (especially faceCorrespondences_ in ShapeAnalyzer).
    ///
    class FaceCorrespondence : public Correspondence {
    public:
        /// \brief Basic Constructor.
        /// @param id The final id of this correspondence data. It should be unique
        /// among all correspondence data, but this is not enforced.
        FaceCorrespondence(vtkIdType id) : Correspondence(id) {
        }
        
        virtual string toString();
    };
}
#endif /* defined(__ShapeAnalyzer__FaceCorrespondence__) */
