
#include "ColorEigenfunctionContextMenuItem.h"

///////////////////////////////////////////////////////////////////////////////
void custom::contextMenuItems::ColorEigenfunctionContextMenuItem::onClick(vtkIdType pointId, vtkIdType faceId, QWidget* parent) {
    bool ok;
    int i = QInputDialog::getInt(
                                 parent,
                                 "Eigenfunction",
                                 "Choose an eigenfunction.",
                                 0,
                                 0,
                                 std::min((vtkIdType) 299, shape_->getPolyData()->GetNumberOfPoints()),
                                 1,
                                 &ok
                                 );
    // show eigenfunction
    if (ok) {
        try {
            PetscFEMLaplaceBeltramiOperator laplacian(shape_, i + 1);
            vtkSmartPointer<vtkDoubleArray> eigenfunction = laplacian.getEigenfunction(i);
            
            shape_->setColoring(eigenfunction, Shape::Coloring::Type::PointScalar);
        } catch(LaplaceBeltramiError& e) {
            QMessageBox::warning(parent, "Exception", e.what());
        }
    }
}
