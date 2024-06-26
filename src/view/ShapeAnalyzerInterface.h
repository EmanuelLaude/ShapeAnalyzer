#ifndef ShapeAnalyzer_ShapeAnalyzerInterface_h
#define ShapeAnalyzer_ShapeAnalyzerInterface_h

#include <vector>


#include "../domain/correspondences/FaceCorrespondence.h"
#include "../domain/correspondences/PointCorrespondence.h"
#include "../domain/Shape.h"

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>

using namespace correspondences;

/// \brief Interface declaring all public functions accessable by CustomTab or CustomContextMenuItem objects. All of them rerender the vtkWidget.
/// \details Implemented by ShapeAnalyzer. Read access to all shapes and all correspondences is
/// provided via the protected read-only (const) class members shapes_, pointCorrespondences_ and
/// faceCorrespondences_ in class CustomTab. Therefore only concrete CustomTab classes can have
/// access to all correspondences and all shapes at the same time.
/// \author Emanuel Laude and Zorah Lähner
///
class ShapeAnalyzerInterface {
public:
    /// \brief Creates a PointCorrespondence out of a vector of shape plus vertex ID pairs and adds it to the internal data structures of ShapeAnalyzer.
    /// @param const vector<Shape*, vtkIdType>& vector of Shape plus point ID pairs that are part of the PointCorrespondence.
    /// @return shared_ptr<PointCorrespondence> The newly created PointCorrespondence.
    virtual shared_ptr<PointCorrespondence> addPointCorrespondence(const vector<pair<shared_ptr<Shape>, vtkIdType>>& correspondence) = 0;

    /// \brief Creates a FaceCorrespondence out of a vector of shape plus vertex ID pairs and adds it to the internal data structures of ShapeAnalyzer.
    /// @param const vector<Shape*, vtkIdType>& vector of Shape plus face ID pairs that are part of the FaceCorrespondence.
    /// @return shared_ptr<PointCorrespondence> The newly created FaceCorrespondence.
    virtual shared_ptr<FaceCorrespondence> addFaceCorrespondence(const vector<pair<shared_ptr<Shape>, vtkIdType>>& correspondence) = 0;
    
    /// \brief Adds and visualizes a new shape.
    /// \details Creates a new Shape object given a name and the mesh data and returns the newly created Shape object.
    /// @param string name. The name of the shape.
    /// @param vtkSmartPointer<vtkPolyData> polyData. The mesh data.
    virtual shared_ptr<Shape> addShape(string name, vtkSmartPointer<vtkPolyData> polyData) = 0;
    
    /// \brief Rerenders the vtkWidget scene.
    /// \details Any changes made for example by CustomTab classes to Correspondences or Shapes are not instantly visible.
    /// Use this function after all changes are made to update the visualisation when you are not using
    /// any functions of ShapeAnalyzer to make the changes. (i.e. deleteCorrespondence will rerender by itself)
    virtual void render() = 0;
};

#endif
