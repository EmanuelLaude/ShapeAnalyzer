
#ifndef __ShapeAnalyzer__VoronoiCellsContextMenuItem__
#define __ShapeAnalyzer__VoronoiCellsContextMenuItem__

#include <string>

#include "CustomContextMenuItem.h"

#include "../../domain/laplaceBeltrami/LaplaceBeltramiOperator.h"
#include "../../domain/segmentation/VoronoiCellSegmentation.h"
#include "../../domain/samplings/FarthestPointSampling.h"
#include "../../domain/metric/MetricError.h"

#include "../Factory.h"

#include <QInputDialog>

using namespace std;
using namespace segmentation;

namespace custom {
namespace contextMenuItems {

template<class T = Metric>
class VoronoiCellsContextMenuItem : public CustomContextMenuItem {
public:
    VoronoiCellsContextMenuItem<T>(shared_ptr<Shape> shape, ShapeAnalyzerInterface* shapeAnalyzer) : CustomContextMenuItem(shape, shapeAnalyzer) {}
    
    virtual void onClick(vtkIdType pointId, vtkIdType faceId, QWidget* parent) {
        bool ok;
        vtkIdType source = QInputDialog::getInt(
                                                parent,
                                                "Source vertex",
                                                "Choose ID of source vertex.",
                                                0,
                                                0,
                                                shape_->getPolyData()->GetNumberOfPoints()-1,
                                                1,
                                                &ok
                                                );
        
        if (!ok) {
            return;
        }
        vtkIdType numberOfSegments = QInputDialog::getInt(
                                                          parent,
                                                          "Number of segments",
                                                          "Choose number of segments",
                                                          0,
                                                          0,
                                                          shape_->getPolyData()->GetNumberOfPoints()-1,
                                                          1,
                                                          &ok
                                                          );
        if(ok) {
            try {
                auto m = make_shared<T>(shape_);
                auto fps = make_shared<FarthestPointSampling>(shape_, m, source, numberOfSegments);
                VoronoiCellSegmentation segmentation(shape_, m, fps);
                
                // save current segmentation for being able to create new shapes out of the segments
                shape_->setColoring(segmentation.getSegments(), Shape::Coloring::Type::PointSegmentation);
            } catch(metric::MetricError& e) {
                QMessageBox::warning(parent, "Exception", e.what());
            }
        }
    }
};
    
}
}

#endif /* defined(__ShapeAnalyzer__VoronoiCellsContextMenuItem__) */
