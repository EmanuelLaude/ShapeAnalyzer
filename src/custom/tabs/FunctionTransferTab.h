#ifndef __ShapeAnalyzer__FunctionTransferTab__
#define __ShapeAnalyzer__FunctionTransferTab__

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>

#include <chrono>
#include <thread>
#include <iostream>

#include <QMessageBox>
#include <QScrollBar>

#include "../../domain/Shape.h"
#include "../../domain/correspondences/PointCorrespondence.h"
#include "../../domain/metric/GeodesicMetric.h"
#include "../../domain/laplaceBeltrami/PetscFEMLaplaceBeltramiOperator.h"
#include "../../domain/signatures/PetscWaveKernelSignature.h"
#include "../../domain/PetscHeatDiffusion.h"
#include "../../domain/PetscFunctionalMaps.h"
#include "../../util/HashMap.h"

#include "../../view/CustomListWidgetItem.h"

#include "CustomTab.h"

#include "ui_FunctionTransferTabWidget.h"

using namespace util;

namespace custom {
namespace tabs {

class FunctionTransferTab : public QWidget, private Ui::FunctionTransferTabWidget, public CustomTab {
    Q_OBJECT
    
public:
    FunctionTransferTab(const HashMap<vtkActor*, shared_ptr<Shape>>& shapes,
                        const HashMap<shared_ptr<PointCorrespondence>, bool>& pointCorrespondences,
                        const HashMap<shared_ptr<FaceCorrespondence>, bool>& faceCorrespondences,
                        ShapeAnalyzerInterface* shapeAnalyzer);
    
    virtual ~FunctionTransferTab();
    
    virtual void onShapeDelete(Shape* shape);
    virtual void onShapeAdd(Shape* shape);
    virtual void onShapeEdit(Shape* shape);
    virtual void onClear();
private slots:
    virtual void slotTransfer();
    
private:
    void log(string line);
    void normalize(vtkSmartPointer<vtkDoubleArray> array, vtkSmartPointer<vtkDoubleArray> result, double norm);
};

}
}

#endif /* defined(__ShapeAnalyzer__FunctionTransferTab__) */
