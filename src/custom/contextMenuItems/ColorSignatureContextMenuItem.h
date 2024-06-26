
#ifndef __ShapeAnalyzer__ColorSignatureContextMenuItem__
#define __ShapeAnalyzer__ColorSignatureContextMenuItem__

#include <string>

#include "CustomContextMenuItem.h"

#include "../../domain/laplaceBeltrami/PetscLaplaceBeltramiOperator.h"
#include "../../domain/laplaceBeltrami/LaplaceBeltramiError.h"
#include "../../domain/signatures/PetscLaplaceBeltramiSignature.h"

#include "../Factory.h"

#include <QInputDialog>
#include <QMessageBox>

#include <vtkDoubleArray.h>

using namespace std;

namespace custom {
namespace contextMenuItems {

template<class T = PetscLaplaceBeltramiSignature>
class ColorSignatureContextMenuItem : public CustomContextMenuItem {
public:
    ColorSignatureContextMenuItem<T>(shared_ptr<Shape> shape, ShapeAnalyzerInterface* shapeAnalyzer) : CustomContextMenuItem(shape, shapeAnalyzer) {}
    
    virtual void onClick(vtkIdType pointId, vtkIdType faceId, QWidget* parent) {
        bool ok;
        int i = QInputDialog::getInt(
                                     parent,
                                     "Component",
                                     "Choose component.",
                                     0,
                                     0,
                                     99,
                                     1,
                                     &ok
                                     );
        
        if (ok) {
            try {
                shared_ptr<PetscLaplaceBeltramiOperator> laplacian = make_shared<PetscFEMLaplaceBeltramiOperator>(shape_, 100);
                
                shared_ptr<T> s = make_shared<T>(shape_, 100, laplacian, 20);
                
                vtkSmartPointer<vtkDoubleArray> component = s->getComponent(i);
                
                shape_->setColoring(component, Shape::Coloring::Type::PointScalar);
            } catch(LaplaceBeltramiError& e) {
                QMessageBox::warning(parent, "Exception", e.what());
            }
        }
    }
};

}
}
    
#endif /* defined(__ShapeAnalyzer__ColorSignatureContextMenuItem__) */
