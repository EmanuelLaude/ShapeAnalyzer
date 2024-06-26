#ifndef __ShapeAnalyzer__ColorEigenfunctionContextMenuItem__
#define __ShapeAnalyzer__ColorEigenfunctionContextMenuItem__

#include <string>

#include "CustomContextMenuItem.h"

#include "../../domain/laplaceBeltrami/PetscFEMLaplaceBeltramiOperator.h"
#include "../../domain/laplaceBeltrami/LaplaceBeltramiError.h"

#include "../Factory.h"

#include <QInputDialog>
#include <QMessageBox>

using namespace std;
using namespace laplaceBeltrami;

namespace custom {
namespace contextMenuItems {

class ColorEigenfunctionContextMenuItem : public CustomContextMenuItem {
public:
    ColorEigenfunctionContextMenuItem(shared_ptr<Shape> shape, ShapeAnalyzerInterface* shapeAnalyzer) : CustomContextMenuItem(shape, shapeAnalyzer) {}
    
    virtual void onClick(vtkIdType pointId, vtkIdType faceId, QWidget* parent);
};
    
}
}

#endif /* defined(__ShapeAnalyzer__ColorEigenfunctionContextMenuItem__) */
