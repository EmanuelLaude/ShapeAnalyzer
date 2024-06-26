#ifndef __ShapeAnalyzer__CorrespondenceColoringTab__
#define __ShapeAnalyzer__CorrespondenceColoringTab__

#include <QGridLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QListWidget>
#include <QString>
#include <QStringList>
#include <QWidget>

#include <memory>

#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>

#include "../../view/CustomListWidgetItem.h"
#include "CustomTab.h"

#include "../../util/HashMap.h"
#include "../../domain/Shape.h"

#include "../../domain/correspondences/FaceCorrespondence.h"
#include "../../domain/correspondences/PointCorrespondence.h"

#include "CorrespondenceColoring.h"

#include "ui_CorrespondenceColoringTabWidget.h"

namespace custom {
namespace tabs {

///
/// \brief Visualizes correspondences with colors on the shapes.
/// \details One of the shapes in the GUI can be chosen as the reference shape. The reference
/// shape will be colored according to the coordinates of its vertices. Every correspondence
/// including the reference shape will be visualized by transferring the color from the
/// reference shape to each other shape to the corresponding vertex/face. Point and face
/// correspondences can be switched. For more details see CorrespondenceColoring.
///
/// @author Emanuel Laude and Zorah Lähner
///
class CorrespondenceColoringTab : public QWidget, public Ui::CorrespondenceColoringTabWidget, public CustomTab {
    Q_OBJECT
    
public:
    /// \brief Standard constructor. Sets up the tab.
    CorrespondenceColoringTab(const HashMap<vtkActor*, shared_ptr<Shape>>& shapes,
                              const HashMap<shared_ptr<PointCorrespondence>, bool>& pointCorrespondences,
                              const HashMap<shared_ptr<FaceCorrespondence>, bool>& faceCorrespondences,
                              ShapeAnalyzerInterface* shapeAnalyzer);
    /// Empty destructor.
    virtual ~CorrespondenceColoringTab() {}
    
    /// Deletes the shape out of the shape drop menu.
    virtual void onShapeDelete(Shape* shape);
    /// Adds the shape to the shape drop menu.
    virtual void onShapeAdd(Shape* shape);
    /// Nothing happens.
    virtual void onShapeEdit(Shape* shape);
    /// Clears the shape drop menu. Clears old information about the density of correspondences.
    virtual void onClear();

private slots:
    virtual void slotColorCorrespondences();
    virtual void slotClearColoring();
    virtual void slotCorrespondencesToggled(bool);
    
private:
    /// Clears the correspondence density grid.
    void clearGrid();
};

}
}

#endif /* defined(__ShapeAnalyzer__CorrespondenceColoringTab__) */
