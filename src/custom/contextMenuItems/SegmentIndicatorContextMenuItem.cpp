
#include "SegmentIndicatorContextMenuItem.h"

///////////////////////////////////////////////////////////////////////////////
void custom::contextMenuItems::SegmentIndicatorContextMenuItem::onClick(
                                                                 vtkIdType pointId,
                                                                 vtkIdType faceId,
                                                                 QWidget* parent)
{

    if(shape_->getColoring() == nullptr || shape_->getColoring()->type != Shape::Coloring::Type::PointSegmentation) {
        QMessageBox::warning(parent, "Error", "Shape \"" + QString(shape_->getName().c_str()) + "\" not segmented. Compute a segmentation first.");
        return;
    }
    
    vtkIdType segmentId = shape_->getColoring()->values->GetTuple(pointId)[0];
    
    vtkSmartPointer<vtkIntArray> indicatorFunction = vtkSmartPointer<vtkIntArray>::New();
    indicatorFunction->SetNumberOfValues(shape_->getPolyData()->GetNumberOfPoints());
    // color 2nd shape
    for(vtkIdType j = 0; j < shape_->getPolyData()->GetNumberOfPoints(); j++) {
        if(shape_->getColoring()->values->GetComponent(j, 0) == segmentId) {
            indicatorFunction->SetValue(j, 1.0);
        } else {
            indicatorFunction->SetValue(j, 0.0);
        }
    }
    shape_->setColoring(indicatorFunction, Shape::Coloring::Type::PointSegmentation);
}