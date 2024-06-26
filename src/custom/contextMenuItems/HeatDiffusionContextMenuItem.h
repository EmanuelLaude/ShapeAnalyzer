
#ifndef __ShapeAnalyzer__HeatDiffusionContextMenuItem__
#define __ShapeAnalyzer__HeatDiffusionContextMenuItem__

#include <string>
#include <chrono>
#include <thread>

#include "CustomContextMenuItem.h"

#include "../../domain/laplaceBeltrami/PetscFEMLaplaceBeltramiOperator.h"
#include "../../domain/laplaceBeltrami/LaplaceBeltramiError.h"
#include "../../domain/PetscHeatDiffusion.h"

#include "../Factory.h"

#include <QInputDialog>
#include <QMessageBox>

using namespace std;
using namespace laplaceBeltrami;

namespace custom {
namespace contextMenuItems {

template<bool ANIMATED>
class HeatDiffusionContextMenuItem : public CustomContextMenuItem {
public:
    HeatDiffusionContextMenuItem(shared_ptr<Shape> shape, ShapeAnalyzerInterface* shapeAnalyzer) : CustomContextMenuItem(shape, shapeAnalyzer) {}
    
    virtual void onClick(vtkIdType pointId, vtkIdType faceId, QWidget* parent) {
        bool ok;
        double tFinal = QInputDialog::getDouble(
                                                parent,
                                                "Heat diffusion",
                                                "Choose diffusion time.",
                                                0,
                                                0,
                                                2147483647,
                                                2,
                                                &ok
                                                );
        if (!ok) {
            return;
        }
        
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
        
        if (ok) {
            try {
                vtkSmartPointer<vtkDoubleArray> u0 = vtkSmartPointer<vtkDoubleArray>::New();
                u0->SetNumberOfValues(shape_->getPolyData()->GetNumberOfPoints());
                for(vtkIdType i = 0; i < shape_->getPolyData()->GetNumberOfPoints(); i++) {
                    if(i == source) {
                        u0->SetValue(i, 1.0);
                    } else {
                        u0->SetValue(i, 0.0);
                    }
                }
                shared_ptr<PetscLaplaceBeltramiOperator> laplacian = make_shared<PetscFEMLaplaceBeltramiOperator>(shape_, 100);
                
                PetscHeatDiffusion heatDiffusion(shape_, laplacian, u0);
                
                if(ANIMATED) {
                    for(double t = 0.0; t <= tFinal; t+=5.0) {
                        vtkSmartPointer<vtkDoubleArray> ut = heatDiffusion.getHeat(t);

                        shape_->setColoring(ut, Shape::Coloring::Type::PointScalar);
                        
                        shapeAnalyzer_->render();
                        
                        this_thread::sleep_for(chrono::milliseconds(10));
                    }
                } else {
                    vtkSmartPointer<vtkDoubleArray> ut = heatDiffusion.getHeat(tFinal);

                    shape_->setColoring(ut, Shape::Coloring::Type::PointScalar);
                    
                    shapeAnalyzer_->render();
                }
            } catch(LaplaceBeltramiError& e) {
                QMessageBox::warning(parent, "Exception", e.what());
            }
        }
    }
};
    
}
}

#endif /* defined(__ShapeAnalyzer__HeatDiffusionContextMenuItem__) */
