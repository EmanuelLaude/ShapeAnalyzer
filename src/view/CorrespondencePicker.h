#ifndef __ShapeAnalyzer__CorrespondencePicker__
#define __ShapeAnalyzer__CorrespondencePicker__


#include <vtkActor.h>
#include <vtkExtractSelection.h>
#include <vtkDataArray.h>
#include <vtkDataSetMapper.h>
#include <vtkUnstructuredGrid.h>
#include <vtkSelectionNode.h>
#include <vtkSelection.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPointPlacer.h>
#include <vtkSmartPointer.h>

#include <list>
#include <algorithm>

#include "../domain/Shape.h"
#include "../domain/correspondences/VisualCorrespondence.h"
#include "../domain/correspondences/Correspondence.h"
#include "../domain/correspondences/PointCorrespondence.h"
#include "../domain/correspondences/FaceCorrespondence.h"

using namespace correspondences;

/// \brief Responsible for creating new Correspondence objects.
/// \details TODO
/// @tparam Should be PointCorrespondence or FaceCorrespondence
/// @author Emanuel Laude and Zorah Lähner
template<class T>
class CorrespondencePicker {
public:
    //protected contructor: abstract class
    CorrespondencePicker(vtkSmartPointer<vtkRenderer> renderer, function<shared_ptr<T>()> factory) : renderer_(renderer), counter_(0), factory_(factory) {
        selectionActor_ = vtkSmartPointer<vtkActor>::New();
        selectionMapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
        
        lineActor_ = vtkSmartPointer<vtkActor>::New();
        lineMapper_ = vtkSmartPointer<vtkPolyDataMapper>::New();
    }
    
    
    virtual ~CorrespondencePicker() {
        clearSelection();
    }

    // stores the current selection in the given pointer (if the selection is a
    // valid correspondence) and resets the picker
    bool pick(shared_ptr<VisualCorrespondence<T>>& correspondence) {
        if(counter_ >= 2) {
            renderer_->RemoveActor(selectionActor_);
            renderer_->RemoveActor(lineActor_);
            renderer_->GetRenderWindow()->Render();
            
            correspondence = move(correspondence_);
            correspondence_.reset(new VisualCorrespondence<T>(renderer_, factory_()));
            //reset counter
            counter_ = 0;
            return true;
        }
        
        clearSelection();
        return false;
    }
    
    //add face (or vertex) ID + shape pair to correspondence. Returns true if id+shape pair has successfully been added (i.e. if shape has not been added twice).
    int addShape(shared_ptr<Shape> shape, vtkIdType selectionId) throw(std::invalid_argument) {
        if(correspondence_ == nullptr) {
            correspondence_.reset(new VisualCorrespondence<T>(renderer_, factory_()));
        }
        
        if(counter_ == 0) {
            correspondence_->addToRenderer();
        }
        
        // visual response for picked face or vertex
        double source[3];
        
        //get point that is the source of the moving line
        SelectionVisualizationStrategy<T>::getCoordinates(source, shape.get(), selectionId);
        
        //visualize mouse line drawn from source to mouse coordinates
        visualizeLine(shape.get(), source);
        
        //visualize selected vertex or face
        double color[3] = { 0.0, 1.0, 1.0 };
        SelectionVisualizationStrategy<T>::initActor(selectionActor_, selectionMapper_, color, shape.get(), selectionId);
        renderer_->AddActor(selectionActor_);
        renderer_->Render();
        
        int result = correspondence_->addShape(shape, selectionId);
        
        if(result == 1) { // success in adding the selection
            counter_++;
            
            return result;
        }
        if(result == -1) { // clicked shape is already part of correspondence and not equal to the last added
            // reset selection
            counter_ = 1;
            correspondence_->clear();
            correspondence_->addShape(shape, selectionId);
            return result;
        }
        
        return result;
    }
    
    int getCounter() {
        return counter_;
    }

    void onMouseMove(int x, int y){
        if(counter_ > 0) {
            vtkSmartPointer<vtkPointPlacer> pointPlacer = vtkSmartPointer<vtkPointPlacer>::New();
            double orientation[3];
            double point[3];
            double pos[2];
            pos[0] = x;
            pos[1] = y;
            pointPlacer->ComputeWorldPosition(renderer_, pos, point, orientation);
            
            lineMapper_->GetInput()->GetPoints()->SetPoint(0, point);
            lineMapper_->GetInput()->Modified();
            renderer_->GetRenderWindow()->Render();
        }
    }
    
    // Resets information and deletes actors from renderer
    void clearSelection() {
        if(counter_ > 0) {
            correspondence_->clear();
            correspondence_->removeFromRenderer();
            renderer_->RemoveActor(selectionActor_);
            renderer_->RemoveActor(lineActor_);
            renderer_->GetRenderWindow()->Render();
            counter_ = 0;
        }
    }
private:
    void visualizeLine(Shape* shape, double point[3]){
        //initialize poly data and required data structures
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkPolyData> linePolyData = vtkSmartPointer<vtkPolyData>::New();
        vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
        vtkSmartPointer<vtkCellArray> lineCells = vtkSmartPointer<vtkCellArray>::New();
        
        
        points->InsertNextPoint(point);
        points->InsertNextPoint(point);
        line->GetPointIds()->SetId(0, 0);
        line->GetPointIds()->SetId(1, 1);
        lineCells->InsertNextCell(line);
        linePolyData->SetPoints(points);
        linePolyData->SetLines(lineCells);
        
        //create mapper and actor. mapper maps the poly data to the line actor
        lineMapper_->SetInputData(linePolyData);
        
        vtkLinearTransform* t1 = shape->getTransformation();
        if(t1 != nullptr) {
            linePolyData->GetPoints()->SetPoint(0, t1->TransformPoint(point));
            linePolyData->GetPoints()->SetPoint(1, t1->TransformPoint(point));
        }
        
        lineActor_->SetMapper(lineMapper_);
        lineActor_->GetProperty()->SetLineWidth(1);
        lineActor_->GetProperty()->SetColor(0, 1, 1);
        lineActor_->SetPickable(0);
        renderer_->AddActor(lineActor_);
    }
    
    
    // visualization stuff
    
    /// \brief Moving line between last selected point and mouse coordinates.
    vtkSmartPointer<vtkActor> lineActor_;
    /// \brief vtkMapper for lineActor_.
    vtkSmartPointer<vtkPolyDataMapper> lineMapper_;
    
    /// \brief Actor responsible for visualizing selected vertex or selected face on selected shape.
    vtkSmartPointer<vtkActor> selectionActor_;
    /// \brief vtkMapper for selectionMapper_.
    vtkSmartPointer<vtkPolyDataMapper> selectionMapper_;
    
    vtkSmartPointer<vtkRenderer> renderer_;

    unique_ptr<VisualCorrespondence<T>> correspondence_;
    
    int counter_;
    
    function<shared_ptr<T>()> factory_;
};

#endif /* defined(__ShapeAnalyzer__CorrespondencePicker__) */
