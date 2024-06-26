#ifndef __ShapeAnalyzer__VisualCorrespondence__
#define __ShapeAnalyzer__VisualCorrespondence__

#include <vtkActor.h>
#include <vtkCellArray.h>
#include <vtkType.h>
#include <vtkLine.h>
#include <vtkLinearTransform.h>
#include <vtkLineSource.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTriangle.h>

#include "PointCorrespondence.h"
#include "FaceCorrespondence.h"

#include "../../util/HashMap.h"
#include "../Shape.h"


#include <vector>
#include <algorithm>

using namespace std;

namespace correspondences {
    /// \brief Selection visualization strategy. For each Correspondence type the visualization behavior is defined in the functions initActor() and getCoordinates()
    template<class T>
    struct SelectionVisualizationStrategy;
    
    /// \brief Selection visualization strategy for type PointCorrespondence.
    template<>
    struct SelectionVisualizationStrategy<PointCorrespondence> {
        /// \brief Initializes the actors representing the vertices of the Correspondence.
        static void initActor(vtkSmartPointer<vtkActor> actor, vtkSmartPointer<vtkPolyDataMapper> mapper, double color[3], Shape* shape, vtkIdType pointId) {
            vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
            vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
            
            double p[3];
            shape->getPolyData()->GetPoint(pointId, p);
            points->InsertNextPoint(p);
            
            polyData->SetPoints(points);
            
            vtkSmartPointer<vtkVertexGlyphFilter> glyphFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
            glyphFilter->SetInputData(polyData);
            glyphFilter->Update();
            
            mapper->SetInputData(glyphFilter->GetOutput());
            actor->SetMapper(mapper);
            actor->GetProperty()->SetPointSize(5);
            actor->GetProperty()->SetRepresentationToPoints();
            actor->GetProperty()->SetColor(color[0], color[1], color[2]);
            actor->SetUserTransform(shape->getTransformation());
        }
        
        /// \brief Returns the 3D coordinates of the selection vertex.
        static void getCoordinates(double point[3], Shape* shape, vtkIdType pointId) {
            shape->getPolyData()->GetPoints()->GetPoint(pointId, point);
        }
    };
    
    /// \brief Selection visualization strategy for type PointCorrespondence.
    template<>
    struct SelectionVisualizationStrategy<FaceCorrespondence> {
        /// \brief Initializes the actors representing the faces of the Correspondence.
        static void initActor(vtkSmartPointer<vtkActor> actor, vtkSmartPointer<vtkPolyDataMapper> mapper, double color[3], Shape* shape, vtkIdType faceId) {
            vtkSmartPointer<vtkTriangle> face = vtkTriangle::SafeDownCast(shape->getPolyData()->GetCell(faceId));
            vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
            vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
            vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
            
            double p[3];
            face->GetPoints()->GetPoint(0, p);
            points->InsertNextPoint(p);
            face->GetPoints()->GetPoint(1, p);
            points->InsertNextPoint(p);
            face->GetPoints()->GetPoint(2, p);
            points->InsertNextPoint(p);
            
            vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
            triangle->GetPointIds()->SetId(0, 0);
            triangle->GetPointIds()->SetId(1, 1);
            triangle->GetPointIds()->SetId(2, 2);
            
            polys->InsertNextCell(triangle);
            
            
            polyData->SetPoints(points);
            polyData->SetPolys(polys);
            
            
            mapper->SetInputData(polyData);
            actor->SetMapper(mapper);
            actor->GetProperty()->EdgeVisibilityOn();
            actor->GetProperty()->SetEdgeColor(color[0], color[1], color[2]);
            actor->GetProperty()->SetLineWidth(3);
            actor->SetUserTransform(shape->getTransformation());
        }
        
        /// \brief Returns the 3D coordinates of the center of the selected face.
        static void getCoordinates(double point[3], Shape* shape, vtkIdType faceId) {
            double p1[3];
            double p2[3];
            double p3[3];
            
            vtkSmartPointer<vtkTriangle> face = vtkTriangle::SafeDownCast(shape->getPolyData()->GetCell(faceId));
            face->GetPoints()->GetPoint(0, p1);
            face->GetPoints()->GetPoint(1, p2);
            face->GetPoints()->GetPoint(2, p3);
            
            vtkTriangle::TriangleCenter(p1, p2, p3, point);
        }
    };
    
    ///
    /// \brief Abstract Template Class for VisualCorrespondences that are shown in the GUI.
    ///
    /// \details Contains a Correspondence Pointer and can visualize this data
    /// in the GUI with lines and highlighted faces or points.
    /// @tparam Correspondence Can either be PointCorrespondence or FaceCorrespondence.
    template<class T>
    class VisualCorrespondence {
        
    public:
        /// \brief Constructor
        /// @param vtkSmartPointer<vtkRenderer> renderer. The renderer to which the correspondence is added afterwards using the function addToRenderer().
        /// @param shared_ptr<T> a Correspondence object of type T.
        VisualCorrespondence(vtkSmartPointer<vtkRenderer> renderer, shared_ptr<T> correspondence) : correspondence_(correspondence), renderer_(renderer) {
            referencePoints_ = vtkSmartPointer<vtkPoints>::New();
            vtkSmartPointer<vtkPoints> linePoints = vtkSmartPointer<vtkPoints>::New();
            vtkSmartPointer<vtkCellArray> lineCells = vtkSmartPointer<vtkCellArray>::New();
            
            polyData_ = vtkSmartPointer<vtkPolyData>::New();
            //initialize poly data
            polyData_->SetPoints(linePoints);
            polyData_->SetLines(lineCells);
            
            
            //create mapper and actor. mapper maps the poly data to the line actor
            mapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
            mapper_->SetInputData(polyData_);
            actor_ = vtkSmartPointer<vtkActor>::New();
            actor_->SetMapper(mapper_);
            actor_->GetProperty()->SetLineWidth(1);
            actor_->GetProperty()->SetColor(0, 1, 0);
            
            // in case CorrespondenceData was not empty create actors.
            if(correspondence_->size() > 0) {
                // fill poly data with all points
                for (int i = 0; i < correspondence_->getShapes().size(); i++) {
                    // insert point
                    double point[3];
                    SelectionVisualizationStrategy<T>::getCoordinates(point, correspondence_->getShapes().at(i).get(), correspondence_->getCorrespondingIds().at(i));
                    referencePoints_->InsertNextPoint(point);
                    polyData_->GetPoints()->InsertNextPoint(point);
                    polyData_->Modified();
                    selectionActors_.push_back(vtkSmartPointer<vtkActor>::New());
                    
                    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                    double color[3] = { 0.0, 1.0, 0.0 };
                    SelectionVisualizationStrategy<T>::initActor(selectionActors_[selectionActors_.size()-1], mapper, color, correspondence_->getShapes().at(i).get(), correspondence_->getCorrespondingIds().at(i));
                    transform(correspondence_->getShapes().at(i).get());
                }
                
                // produce lines
                for (int i = 1; i < correspondence_->size(); i++) {
                    vtkSmartPointer<vtkIdList> line = vtkSmartPointer<vtkIdList>::New();
                    line->InsertId(0, i-1);
                    line->InsertId(1, i);
                    polyData_->InsertNextCell(VTK_LINE, line);
                    
                    polyData_->Modified();
                }
                
                // transform
                for (int i = 0; i < correspondence_->getShapes().size(); i++) {
                    transform(correspondence_->getShapes().at(i).get());
                }
            }
        }
        
        /// \brief Virtual destructor.
        virtual ~VisualCorrespondence() {
            removeFromRenderer();
            selectionActors_.clear();
        }
        
        /// \brief Transforms this correspondence in case the shape provided as a parameter is part of it.
        /// \details To do so it applies the current user transformation of the shape to the original coordinates of the respective end point of the line representing the correspondence. Afterwards it updates the line with the transformed coordinates and sets the current user transformation of the actor representing the point or face of the transformed shape to the new one.
        /// @param Shape* shape. The shape that was transformed by the user.
        void transform(Shape* shape) {
            for(int i = 0; i < correspondence_->getShapes().size(); i++) {
                if(correspondence_->getShapes().at(i).get() == shape) {
                    vtkLinearTransform* t = shape->getTransformation();
                    if(t != nullptr) {
                        double point[3];
                        referencePoints_->GetPoint(i, point);
                        polyData_->GetPoints()->SetPoint(i, t->TransformPoint(point));
                        polyData_->Modified();
                        selectionActors_[i]->SetUserTransform(t);
                    }
                }
            }
        }
        
        
        /// \brief Highlights this correspondence in case it was selected.
        /// @param bool selected. Value is indicating whether the correspondence should be highlighted or not.
        void setSelected(bool selected) {
            if(selected) {
                actor_->GetProperty()->SetColor(1, 0, 0);
                actor_->GetProperty()->SetEdgeColor(1, 0, 0);
                
                actor_->Modified();
                for(int i = 0; i < selectionActors_.size(); i++) {
                    selectionActors_.at(i)->GetProperty()->SetColor(1, 0, 0);
                    selectionActors_.at(i)->GetProperty()->SetEdgeColor(1, 0, 0);
                    renderer_->AddActor(selectionActors_.at(i));
                }
            } else {
                actor_->GetProperty()->SetColor(0, 1, 0);
                actor_->GetProperty()->SetEdgeColor(0, 1, 0);
                actor_->Modified();
                for(int i = 0; i < selectionActors_.size(); i++) {
                    renderer_->RemoveActor(selectionActors_.at(i));
                }
            }
        }
        
        /// \brief Adds another shape plus face ID (or vertex ID) pair to this correspondence. Manages the Correspondence object.
        /// \details Returns 1 if shape plus data id combination has successfully been added (shape has not been selected twice). Returns 0 if the shape equals the last added shape and updates the  coordinates of the lines end point with the new coordinates. Returns -1 if the shape is identical to another shape that has already been added before and the shape is not equals to the last added shape.
        int addShape(shared_ptr<Shape> shape, vtkIdType id) {
            if (shape == nullptr) {
                throw invalid_argument(string("Null pointer input 'shape' in ").append(__PRETTY_FUNCTION__));
            }
            
            if(correspondence_->getShapes().size() > 0 && correspondence_->getShapes()[correspondence_->getShapes().size()-1] == shape) {
                // in case shape is equal to last added shape replace point
                //get coordinates of line target
                double point[3];
                SelectionVisualizationStrategy<T>::getCoordinates(point, shape.get(), id);
                
                // shape is equal to the last inserted shape. Replace id and shape
                referencePoints_->SetPoint(referencePoints_->GetNumberOfPoints()-1, point);
                polyData_->GetPoints()->SetPoint(polyData_->GetPoints()->GetNumberOfPoints()-1, point);
                polyData_->Modified();
                
                vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
                double color[3] = { 0.0, 1.0, 0.0 };
                SelectionVisualizationStrategy<T>::initActor(selectionActors_[selectionActors_.size()-1], mapper, color, shape.get(), id);
                transform(shape.get());
                return 0;
            }
            
            if(find(correspondence_->getShapes().begin(), correspondence_->getShapes().end(), shape) != correspondence_->getShapes().end()) {
                //shape is already part of correspondence and not equal to the last added
                return -1;
            }
            correspondence_->addShape(shape, id);
            
            //shape has not yet been added
            double point[3];
            SelectionVisualizationStrategy<T>::getCoordinates(point, shape.get(), id);
            referencePoints_->InsertNextPoint(point);
            polyData_->GetPoints()->InsertNextPoint(point);
            polyData_->Modified();
            selectionActors_.push_back(vtkSmartPointer<vtkActor>::New());
            
            vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
            double color[3] = { 0.0, 1.0, 0.0 };
            SelectionVisualizationStrategy<T>::initActor(selectionActors_[selectionActors_.size()-1], mapper, color, shape.get(), id);
            transform(shape.get());
            
            if(correspondence_->size() > 1) {
                //Visualize in vtk
                // create new line cell out of the two points inserted last
                vtkSmartPointer<vtkIdList> line = vtkSmartPointer<vtkIdList>::New();
                line->InsertId(0, polyData_->GetPoints()->GetNumberOfPoints()-2);
                line->InsertId(1, polyData_->GetPoints()->GetNumberOfPoints()-1);
                polyData_->InsertNextCell(VTK_LINE, line);
                
                polyData_->Modified();
            }
            
            return 1;
        }
        
        
        /// \brief Removes a shape plus corresponding data id pair from the object. Manages the Correspondence object.
        /// @return Returns index i of the deleted shape. If shape is not part of correspondence it returns -1.
        /// @throws invalid_argument if the function is called when the correspondence is binary (2 shapes).
        int removeShape(Shape* shape) {
            int size = correspondence_->size();
            int i = correspondence_->removeShape(shape);
            if(i == -1) {
                return -1;
            }
            renderer_->RemoveActor(selectionActors_.at(i));
            selectionActors_.erase(selectionActors_.begin() + i);
            
            
            vtkSmartPointer<vtkPoints> referencePoints = vtkSmartPointer<vtkPoints>::New();
            vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
            vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
            vtkSmartPointer<vtkCellArray> lineCells = vtkSmartPointer<vtkCellArray>::New();
            polyData->SetPoints(points);
            polyData->SetLines(lineCells);
            
            
            for(int j = 0; j < size; j++) {
                if(j != i) {
                    referencePoints->InsertNextPoint(referencePoints_->GetPoint(j));
                    points->InsertNextPoint(polyData_->GetPoint(j));
                }
            }
            
            for(int j = 1; j < correspondence_->size(); j++) {
                vtkSmartPointer<vtkIdList> line = vtkSmartPointer<vtkIdList>::New();
                line->InsertId(0, j - 1);
                line->InsertId(1, j);
                polyData->InsertNextCell(VTK_LINE, line);
            }
            
            polyData_->DeepCopy(polyData);
            referencePoints_->ShallowCopy(referencePoints);
            
            return i;
        }
        
        /// Returns the actor representing the line
        vtkActor* getActor() {
            return actor_;
        }
        
        /// Removes the correspondence from the renderer provided as a constructor argument.
        void removeFromRenderer() {
            for(int i = 0; i < selectionActors_.size(); i++) {
                renderer_->RemoveActor(selectionActors_[i]);
            }
            
            renderer_->RemoveActor(actor_);
        }
        
        /// Adds the correspondence to the renderer provided as a contructor argument.
        void addToRenderer() {
            setSelected(false);
            
            renderer_->AddActor(actor_);
        }
        
        /// \brief Returns managed Correspondence object of type T.
        shared_ptr<T> getCorrespondence() {
            return correspondence_;
        }
        
        /// \brief Removes all shape plus data id pairs from the correspondence. Deletes the VTK ojects.
        void clear() {
            for(int i = 0; i < selectionActors_.size(); i++) {
                renderer_->RemoveActor(selectionActors_[i]);
            }
            
            correspondence_->clear();
            selectionActors_.clear();
            
            vtkSmartPointer<vtkPoints> referencePoints = vtkSmartPointer<vtkPoints>::New();
            referencePoints_->ShallowCopy(referencePoints);
            
            vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
            polyData_->DeepCopy(polyData);
            
            renderer_->Render();
        }
        
    private:
        /// \brief The managed Correspondence object.
        shared_ptr<T> correspondence_;
        
        
        ///@{ @name VTK ojects
        /// \brief Renderer.
        vtkSmartPointer<vtkRenderer>    renderer_;
        
        /// \brief Actors highlighting the data (vertices of faces) on the corresponding shapes
        /// \details The order correspondes to the order of the shapes in the Correspondenes
        /// data_ and shapes_.
        vector<vtkSmartPointer<vtkActor>>  selectionActors_;
        
        /// \brief Reference coordinates for the lines.
        /// \details The coordinates are used to transfrom the lines if the shape is transformed
        /// independently of the scene.
        vtkSmartPointer<vtkPoints>          referencePoints_;
        
        /// \brief PolyData containing the end points of the lines.
        /// \details The points are ordered in the same way as the shapes in data_ and shapes_, i.e.
        /// the point with id i in this object correspondes to the shape on position i in shapes_.
        vtkSmartPointer<vtkPolyData>        polyData_;
        /// \brief Mapper of the lines and highlightings.
        vtkSmartPointer<vtkPolyDataMapper>  mapper_;
        /// \brief Actors of the lines.
        vtkSmartPointer<vtkActor>           actor_;
        
        ///@}
    };
}
#endif /* defined(__ShapeAnalyzer__VisualCorrespondence__) */
