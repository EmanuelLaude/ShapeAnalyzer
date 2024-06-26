#include "VoronoiCellSegmentation.h"


///////////////////////////////////////////////////////////////////////////////
vtkSmartPointer<vtkIntArray> segmentation::VoronoiCellSegmentation::getSegments() {
    vtkSmartPointer<vtkIdList> seeds = sampling_->getPoints();
    return metric_->getVoronoiCells(seeds);
}
