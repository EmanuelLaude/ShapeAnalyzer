#ifndef __ShapeAnalyzer__Shape__
#define __ShapeAnalyzer__Shape__

#include <vtkActor.h>
#include <vtkBoxWidget2.h>
#include <vtkBoxRepresentation.h>
#include <vtkCell.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCharArray.h>
#include <vtkCommand.h>
#include <vtkDataArray.h>
#include <vtkGlyph3D.h>
#include <vtkIdList.h>
#include <vtkLookupTable.h>
#include <vtkLinearTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectBase.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkTriangle.h>
#include <vtkType.h>
#include <vtkVertexGlyphFilter.h>

#include <memory>
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>

#include "PetscHelper.h"

using namespace std;

///
/// \brief The Shape contains all geometric information as well as VTK-related objects and basic information about the shape.
/// \details Every shape has a unique and static id and a not necessarily unique name which can be changed.
/// \author Emanuel Laude and Zorah Lähner
/// \note Documentation not finished.
///
class Shape {
public:
    /// \brief Enum that contains several types of a visual representation of a shape.
    /// \details The current visual representation of the shape can be set via the method setVisualRepresentation().
    enum class VisualRepresentation {
        /// \brief Stands for a (triangulated) mesh surface representation (default). Filled faces are visualized.
        MeshSurface,
        /// \brief Stands for a smooth representation using the shapes normal vectors.
        InterpolatedNormals,
        /// \brief Stands for a point cloud representation of the shapes. Only the vertices of the shape a visualized.
        PointCloud,
        /// \brief Stands for a triangulated mesh representation. Only the edges of the faces are visualized.
        Mesh
    };
    
    /// \brief Coloring struct. Contains a type and the coloring data as vtkSmartPointer<vtkDataArray>.
    /// \details The values of PointRgb, FaceRgb, PointScalar and FaceScalar have to be in an vtkDoubleArray.
    /// Additionally the values of *Rgb have to be between 0 and 1 (otherwise the visualisation will probably
    /// not look like intended). The values of PointSegmentation and FaceSegmentation have to be in an
    /// vtkCharArray.
    struct Coloring {
        /// \brief Type enum.
        enum class Type {
            /// \brief Stands for a segmentation that is represented as a discrete vertex map. Each vertex is mapped to an int that stands for its segment ID.
            PointSegmentation,
            /// \brief Stands for a segmentation that is represented as a discrete face map. Each face is mapped to an int that stands for its segment ID.
            FaceSegmentation,
            /// \brief Stands for a 3D RGB point map.
            PointRgb,
            /// \brief Stands for a 3D RGB face map.
            FaceRgb,
            /// \brief Stands for a scalar valued point map.
            PointScalar,
            /// \brief Stands for a scalar valued face map.
            FaceScalar
        };
        
        /// \brief Color data. It can be either point or face data. It is either a scalar (1D) or a RGB (3D) map.
        vtkSmartPointer<vtkDataArray> values;
        
        /// \brief Type of the coloring. It is either PointSegmentation, FaceSegmentation, PointRgb, FaceRgb, PointScalar or FaceScalar
        Type type;
    };
    
    /// \brief Constructor.
    /// @param vtkIdType The unique shape ID.
    /// @param string The name of the shape.
    /// @param vtkSmartPointer<vtkPolyData> The faces and triangles of the shape.
    /// @param vtkSmartPointer<vtkRenderer> The renderer object which is responsible for rendering the shape.
    Shape(vtkIdType id, string name, vtkSmartPointer<vtkPolyData> polyData, vtkSmartPointer<vtkRenderer> renderer);
    
    /// \brief Constructor with predefined transform.
    /// @param vtkIdType The unique shape ID.
    /// @param string The name of the shape.
    /// @param vtkSmartPointer<vtkPolyData> The faces and triangles of the shape.
    /// @param vtkSmartPointer<vtkRenderer> The renderer object which is responsible for rendering the shape.
    Shape(vtkIdType id, string name, vtkSmartPointer<vtkPolyData> polyData, vtkSmartPointer<vtkRenderer> renderer, vtkSmartPointer<vtkMatrix4x4>);
    
    /// Virtual destructor.
    virtual ~Shape() {
        // remove shape from renderer.
        removeFromRenderer();
    }
    
    /// \brief Returns the area of the shape.
    /// \details Calculates the area of every cell in the poly data and adds them up. Assumes that all cells are
    /// triangles, the results will be faulty for non-triangulated meshes.
    double getArea();
    /// \brief Returns the index of an random point on the shape.
    vtkIdType getRandomPoint();
    
    // vtk functions
    
    /// \brief Will remove the shape from the renderer. The shape will not be shown after the next rerender.
    void removeFromRenderer();
    /// \brief Removes any Scalar information that is used to color the shape and resets all properties of the mapper to default.
    void clearColoring();
    
    /// @name Getters
    ///@{
    /// \brief Returns the actor of the shape.
    vtkSmartPointer<vtkActor> getActor() {
        return actor_;
    }

    /// \brief Returns the mesh data.
    vtkSmartPointer<vtkPolyData> getPolyData() {
        return polyData_;
    }
    
    /// \brief Returns the current user transformation.
    vtkLinearTransform* getTransformation() {
        return actor_->GetUserTransform();
    }
    
    
    /// \brief Returns the lookup table that is associated with the mapper.
    vtkScalarsToColors* getLookupTable() {
        return mapper_->GetLookupTable();
    }
    
    /// \brief Returns the mapper for this shape.
    vtkSmartPointer<vtkMapper> getMapper() {
        return mapper_;
    }
    
    ///@}
    
    /// \brief This function has to be called after polyData was modified.
    /// \details It rerenders the shape and updates the bounding box of the BoxWidget.
    void modified();
    /// \brief Adds an observer to the boxWidget. If the shape is transformed independently from the scene via the boxWidget the observers are executed.
    void addObserver(vtkSmartPointer<vtkCommand> callback) {
        boxWidget_->AddObserver(vtkCommand::InteractionEvent, callback);
    }
    
    /// \brief Transforms the shape
    /// @param vtkSmartPointer<vtkTransform> The transformation object.
    void transform(vtkSmartPointer<vtkTransform> t) {
        actor_->SetUserTransform(t);
    }
    
    /// \brief Sets the type of visual representation of the shape.
    void setVisualRepresentation(VisualRepresentation representation);
    
    
    /// \brief Toggles the bounding box widget.
    void setShowBoxWidget(bool showBoxWidget) {
        if(showBoxWidget) {
            boxWidget_->On();
        } else {
            boxWidget_->Off();
        }
    }
    
    /// \brief Returns the unique id of the shape.
    vtkIdType getId() {
        return id_;
    }
    
    /// \brief Returns the name of the shape.
    string getName() {
        return name_;
    }
    
    /// \brief Resets the name of the shape.
    void setName(string name) {
        name_ = name;
    }

    /// \brief Colors points or faces with any kind of Coloring.
    /// \details Depending on the type of the coloring different coloring schemas are applied.
    /// @param . Contains the color data for the points or faces and the type of the coloring.
    void setColoring(vtkSmartPointer<vtkDataArray> values, Coloring::Type type);

    Coloring const* getColoring() const {
        return coloring_.get();
    }

    /// \brief Colors the shape using the coordinates of the vertices as indicators for the rgb value.
    /// \details The values are the coordinates of each vertex but normalized to values between 0 and 255.
    void colorPointsCoordinates();

    /// \brief Colors the shape using the coordinates of the faces as indicators for the rgb value.
    /// \details The values are the interpolated values of the vertices of each face
    /// but normalized to values between 0 and 255. The class inherits from CoordinateColoring
    /// the only overwritten function is the calculation of the color for each face.
    void colorFacesCoordinates();

protected:
    /// \brief Unique id of the shape
    /// \details Although the ShapeAnalyzer only assigns unique ids, the uniqueness is not
    /// forced unconditionally
    vtkIdType id_;
    /// Non-unique name of the shape
    string name_;
    
    /// @name VTK objects
    ///@{
    vtkSmartPointer<vtkActor> actor_;
    vtkSmartPointer<vtkPolyDataMapper> mapper_;
    vtkSmartPointer<vtkBoxWidget2> boxWidget_;
    vtkSmartPointer<vtkPolyData> polyData_;
    vtkSmartPointer<vtkPolyDataNormals> polyDataNormals_;
    
    vtkSmartPointer<vtkRenderer> renderer_;
    ///@}
    
    /// \brief Current Coloring of the shape, if there is any
    unique_ptr<Coloring> coloring_;
};

#endif
