#ifndef __ShapeAnalyzer__MeshCheckTab__
#define __ShapeAnalyzer__MeshCheckTab__

#include <QListWidget>
#include <QString>
#include <QWidget>

#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

#include "../../util/HashMap.h"
#include "../../domain/MeshChecker.h"
#include "../../domain/Shape.h"

#include "CustomTab.h"

#include "ui_MeshCheckTabWidget.h"

namespace custom {
namespace tabs {

/// \brief Tab for the MeshChecker class.
/// \details All functionalities of the MeshChecker can be chosen individually and
/// the output will be displayed in a text field.
class MeshCheckTab : public QWidget, public Ui::MeshCheckTabWidget, public CustomTab {
    Q_OBJECT
    
public:
    MeshCheckTab(const HashMap<vtkActor*, shared_ptr<Shape>>& shapes,
                 const HashMap<shared_ptr<PointCorrespondence>, bool>& pointCorrespondences,
                 const HashMap<shared_ptr<FaceCorrespondence>, bool>& faceCorrespondences,
                 ShapeAnalyzerInterface* shapeAnalyzer);
    
    virtual ~MeshCheckTab() {}
    
    virtual void onShapeDelete(Shape* shape);
    virtual void onShapeAdd(Shape* shape);
    virtual void onShapeEdit(Shape* shape);
    virtual void onClear();
private slots:
    virtual void slotCheckMesh();
private:
    /// \brief Fills the ComboBox with the names of all shapes.
    /// \details If an entry with the text of currentSelection is created, this entry will be selected. Otherwise the blank entry will be selected.
    void setUpComboBox();
};


}
}
#endif /* defined(__ShapeAnalyzer__MeshCheckTab__) */
