#ifndef __ShapeAnalyzer__CorrespondenceColoring__
#define __ShapeAnalyzer__CorrespondenceColoring__

#include <vtkSmartPointer.h>
#include <vtkType.h>
#include <vtkUnsignedCharArray.h>

#include "../../domain/Shape.h"
#include "../../util/HashMap.h"

#include "../../domain/correspondences/FaceCorrespondence.h"
#include "../../domain/correspondences/PointCorrespondence.h"

using namespace correspondences;
using namespace std;
using namespace util;

namespace custom {
namespace tabs {


///
/// \brief Colors the given shapes to visualize the given correspondences.
/// \details The reference shape will be colored by coordinates using the CoordinateColoring class and
/// all points with a corresponding point on another shape will be colored
/// in the same way as the correspondence point on the reference shape.
///
/// Additionally it is possible to keep track of how many and which points are actually corresponding
/// and if they are corresponding to multiple points on the reference shape.
///
/// \author Emanuel Laude and Zorah Lähner
///
///
class CorrespondenceColoring {
public:
    /// \brief Constructor.
    /// @param shapes HashMap of all shapes that will be colored.
    /// @param pointCorrespondences HashMap of all PointCorrespondences that will be considered
    /// @param faceCorrespondences HashMap of all FaceCorrespondences that will be considered
    /// @param reference Optional. Pointer to shape that will be the reference. The first shape
    /// will be chosen if no reference is given.
    CorrespondenceColoring(const HashMap<vtkActor*, shared_ptr<Shape>>&              shapes,
                           const HashMap<shared_ptr<PointCorrespondence>, bool>& pointCorrespondences,
                           const HashMap<shared_ptr<FaceCorrespondence>, bool>&  faceCorrespondences,
                           shared_ptr<Shape> reference = nullptr);
    
    /// \brief Visualizes the point correspondences going out from the reference shape.
    /// \details The reference shape will be colored by coordinates using the CoordinateColoring class and
    /// all points with a corresponding point on another shape will be colored
    /// in the same way as the correspondence point on the reference shape. If a vertex is part of multiple
    /// correspondences, an arbitrary correspondence is visualized.
    /// The parameters can be used to get information of the density of the correspondence between two
    /// shapes and the percentage of doubly matched points.
    /// @param percentageMatched Optional. For every shape that is not the reference shape - identified
    /// by the shape id - the vector will contain the percentage of its vertices that are mapped (might be multiple times)
    /// to any vertex of the reference shape.
    /// @param percentageMultiple Optional. For every shape that is not the reference shape - identified
    /// by the shape id - the vector will contain the percentage of its vertices that are mapped to a vertex
    /// on the reference shape multiple times.
    void showPointCorrespondences(shared_ptr<vector<pair<vtkIdType, double>>> percentageMatched = nullptr,
                                  shared_ptr<vector<pair<vtkIdType, double>>> percentageMultiple = nullptr);
    /// \brief Visualizes the face correspondences going out from the reference shape.
    /// \details The reference shape will be colored by coordinates using the CoordinateColoring class and
    /// all points with a corresponding face on another shape will be colored
    /// in the same way as the correspondence face on the reference shape. If a face is part of multiple
    /// correspondences, an arbitrary correspondence is visualized.
    /// The parameters can be used to get information of the density of the correspondence between two
    /// shapes and the percentage of doubly matched points.
    /// @param percentageMatched Optional. For every shape that is not the reference shape - identified
    /// by the shape id - the vector will contain the percentage of its faces that are mapped (might be multiple times)
    /// to any face of the reference shape.
    /// @param percentageMultiple Optional. For every shape that is not the reference shape - identified
    /// by the shape id - the vector will contain the percentage of its faces that are mapped to a face
    /// on the reference shape multiple times.
    void showFaceCorrespondences(shared_ptr<vector<pair<vtkIdType, double>>> percentageMatched = nullptr,
                                 shared_ptr<vector<pair<vtkIdType, double>>> percentageMultiple = nullptr);
    
protected:
    const HashMap<vtkActor*, shared_ptr<Shape>>&                 shapes_;
    const HashMap<shared_ptr<PointCorrespondence>, bool>&    pointCorrespondences_;
    const HashMap<shared_ptr<FaceCorrespondence>, bool>&     faceCorrespondences_;
    
    shared_ptr<Shape> reference_;
    
    unordered_map<vtkIdType, vtkSmartPointer<vtkUnsignedCharArray>> pointAttributes_;
    unordered_map<vtkIdType, vtkSmartPointer<vtkUnsignedCharArray>> faceAttributes_;
    
};

}
}

#endif /* defined(__ShapeAnalyzer__CorrespondenceColoring__) */
