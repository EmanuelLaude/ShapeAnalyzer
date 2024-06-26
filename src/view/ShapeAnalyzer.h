#ifndef __ShapeAnalyzer__ShapeAnalyzer__
#define __ShapeAnalyzer__ShapeAnalyzer__

#include <vtkActor.h>
#include <vtkAlgorithmOutput.h>
#include <vtkBoxWidget2.h>
#include <vtkBoxRepresentation.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkExtractSelection.h>
#include <vtkDataArray.h>
#include <vtkDataSetMapper.h>
#include <vtkObject.h>
#include <vtkPNGWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataReader.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkTriangleFilter.h>
#include <vtkSelectionNode.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkCleanPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkPointSet.h>
#include <vtkWindowToImageFilter.h>
#include <vtkScalarBarActor.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <vtkPLYReader.h>
#include <vtkOBJReader.h>
#include <vtkPLYWriter.h>
#include <vtkGL2PSExporter.h>

#include <QActionGroup>
#include <QCheckBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QKeySequence>
#include <QList>
#include <QListWidget>
#include <QMainWindow>
#include <QObject>
#include <QSharedPointer>
#include <QShortcut>
#include <QString>
#include <QObject>

#include <exception>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <typeinfo>

#include "CorrespondencePicker.h"
#include "CustomListWidgetItem.h"
#include "ShapeAnalyzerInteractorStyle.h"
#include "ShapeAnalyzerInterface.h"

#include "../custom/CustomClassesRegistry.h"

#include "../util/ErrorObserver.h"
#include "../util/HashMap.h"

#include "../domain/Shape.h"
#include "../domain/VolumetricShape.h"
#include "../domain/correspondences/Correspondence.h"

#include "../io/SceneWriterReader.h"
#include "../io/vtkOFFReader.h"
#include "../io/vtkToscaReader.h"
#include "../io/vtkOFFWriter.h"
#include "../io/vtkToscaWriter.h"
#include "../io/vtkOBJWriter.h"

#include "ui_HelpDialog.h"
#include "ui_ImportShapeDialog.h"
#include "ui_SaveScreenshotDialog.h"
#include "ui_ShapeAnalyzer.h"

using namespace std;
using namespace io;
using namespace util;

///
/// \brief Manages the interaction with the GUI.
/// \details Main class responsible for both the GUI and the data structures containing all the Shapes. It implements ShapeAnalyzerInterface
///
/// \author Emanuel Laude and Zorah Lähner
///

class ShapeAnalyzer : public QMainWindow, private Ui::ShapeAnalyzer, public ShapeAnalyzerInterface {
    Q_OBJECT
    
    /// \brief Needed to obtain a ordered sequence of shapes. Result from HashMap is always unsorted. I.e. no statement about the ordering of
    /// the elements can be made.
    struct ShapeComparator {
        bool operator() (shared_ptr<Shape> s1, shared_ptr<Shape> s2) { return (s1->getId() < s2->getId()); }
    };
    
    /// \brief Box widget callback that manages update of correspondences after a shape has been transformed independently from the rest of the scene.
    class vtkBoxWidgetCallback : public vtkCommand {
    public:
        /// \brief Reference to the parent object
        ShapeAnalyzer *sa;
        
        /// \brief The shape that has been transformed.
        Shape* shape;
        
        /// \brief Method for construction of class
        static vtkBoxWidgetCallback *New() {
            return new vtkBoxWidgetCallback;
        }
        
        /// \brief Method is called if the vtkboxWidget instance to which this callback has been connected was transformed by the user.
        virtual void Execute(vtkObject *caller, unsigned long, void*) {
            vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
            vtkBoxWidget2 *widget = reinterpret_cast<vtkBoxWidget2*>(caller);
            static_cast<vtkBoxRepresentation*>(widget->GetRepresentation())->GetTransform(t);
            
            shape->transform(t);
            
            // transform correspondences
            for(auto entry : sa->visualPointCorrespondences_) {
                entry.second->transform(shape);
            }
            for(auto entry : sa->visualFaceCorrespondences_) {
                entry.second->transform(shape);
            }
            
            sa->qvtkWidget->GetRenderWindow()->Render();
        }
    };
    
public:
    /// \brief Constructor.
    /// \details Initializes all data structures, by default memory for 1000 visible point/face correspondences and 10,000
    /// hidden point/face correspondences is allocated.
    /// All QT-Objects are connected are connected to their respective slots.
    /// Implemented abstract classes are registered in the respective factories (Factory). New implementation have to be added
    /// here to appear in the GUI.
    /// The vtkWidget is initialized.
    /// Slepc is initialized.
    ShapeAnalyzer();
    /// \brief Destructor.
    /// \details Deletes the CorrespondencePicker.
    /// Finalizes Slepc.
    ~ShapeAnalyzer() {
        SlepcFinalize();
    };
    ///@{
    virtual shared_ptr<PointCorrespondence> addPointCorrespondence(const vector<pair<shared_ptr<Shape>, vtkIdType>>& correspondence);
    virtual shared_ptr<FaceCorrespondence> addFaceCorrespondence(const vector<pair<shared_ptr<Shape>, vtkIdType>>& correspondence);
    virtual shared_ptr<Shape> addShape(string name, vtkSmartPointer<vtkPolyData> polyData);
    virtual void render();
    ///@}
private slots:
    ///@{
    /// Terminates the program.
    virtual void slotExit();
    /// Resets the camera to its default settings and renders the vtkWidget.
    /// This does not effect individual transformations of the shapes.
    virtual void slotResetCamera();
    /// Clears all data structures and the vtkWidget.
    virtual void slotClear();

    /// Opens the Help-Window.
    virtual void slotShowHelp();
    /// \brief Opens a context menu with shape interactions on the currently selected shape.
    /// \details The shape will be the currently selected one in the qtListWidget listing all shapes.
    /// Nothing will happen if no shape is selected there.
    virtual void slotShowContextMenuShapes(const QPoint& pos);
    /// \brief Opens a context menu with correspondence interactions on the currently selected visible correspondence.
    /// \details The correspondence will be the currently selected one in the qtListWidget listing all visible
    /// correspondences.
    /// Nothing will happen if no correspondence is selected there.
    virtual void slotShowContextMenuCorrespondences(const QPoint& pos);

    /// Removes any selections that were made with the CorrespondencePicker so far.
    virtual void slotClearCurrentSelection();
    /// Hides all visible correspondences.
    virtual void slotHideVisibleCorrespondences();
    /// Creates and displays a random sample of all correspondences.
    virtual void slotSampleCorrespondences();
    /// Deletes all correspondences including those that are not displayed.
    virtual void slotClearAllCorrespondences();
    
    /// Turns all Box Widget on or off, depending on the previous state.
    virtual void slotToggleBoxWidget();
    /// Changes the visualisation settings to make correspondence adding possible.
    /// For example face correspondences are not possible in the point cloud mode, so it will be
    /// switched to the mesh visualisation.
    virtual void slotModeAddCorrespondences();
    
    /// \brief Changes the visibility of the Color Scalar Bar.
    /// @param visible true correspondes to visible, false to not visible
    virtual void slotShowScalarBar(bool visible);
    
    /// \brief Triggers when another shape is selected in the QListWidget or the vtkWidget.
    /// \details Will change the color scalar bar and the individual transform box widget to the
    /// current shape and turn the individual transform box widget off for the previous one
    /// if necessary. Triggers the qtShapesTab::onShapeSelect() functions of all registered
    /// shape tabs, this might take time depending on the tabs.
    virtual void slotSetSelectedCurrentShape(QListWidgetItem* current, QListWidgetItem* previous);
    /// \brief Triggers when another correspondence is selected in the QListWidget or the vtkWidget.
    /// \details Will change the color of the current correspondence to red and of the previous one
    /// to normal.
    /// Triggers the qtCorrespondencesTab::onCorrespondenceSelect() function of all registered
    /// correspondece tabs, this might take time depending on the tabs.
    virtual void slotSetSelectedCurrentCorrespondence(QListWidgetItem* current, QListWidgetItem* previous);
    
    /// \brief Switches the visual representation of the shape depending on the seleced action in the GUI.
    /// \details If one of the following actions is activated: actionShowSurface, actionShowSurfaceNormals,
    /// actionShowTriangulatedMesh or actionShowPointCloud, the display mode will be changed to
    /// that for all shapes.
    virtual void slotSetVisualRepresentationShape();
    /// Switches the correspondence mode between point and face correspondences depending on the GUI.
    virtual void slotSetCorrespondenceType();
    
    /// Opens a QDialog for selecting the background color of the vtkWidget.
    virtual void slotSetBackgroundColor();
    /// Switches the projection mode between parallel and perspective projection.
    virtual void slotToggleProjectionMode();
    
    /// Opens a dialog for loading a scene from a file.
    virtual void slotOpenScene();
    /// Opens a dialog for saving the current scene in ASCII or binary format.
    virtual void slotSaveScene();
    /// Opens a dialog for loading a shape from a file.
    virtual void slotImportShape();
    /// Opens a dialog for importing correspondences from a files.
    virtual void slotImportCorrespondences();
    /// Opens a dialog for exporting correspondences in ASCII or binary format.
    virtual void slotExportCorrespondences();
    /// Opens a dialog for making a screenshot of the current scene in the vtkWidget.
    virtual void slotSaveScreenshot();
    /// \brief Adds or removes custom tab to or from tab widget.
    /// @param bool visible. If true tab is added to widget otherwise removed.
    virtual void slotViewCustomTab(bool visible);
    ///@}
    
    //vtk widget slots
    ///@{
    virtual void vtkClickHandler(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command);
    virtual void vtkMouseMoveHandler(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command);

    virtual void vtkKeyPressHandler(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command);
    ///@}
private:
    /// \brief Qt function for catching user interactions.
    /// \details This function will only catch mouse wheel actions and surpress them.
    /// Everything else will be handled normally by qt.
    bool eventFilter(QObject *object, QEvent *event);
    
    /// \brief Connects changing operations of listCorrespondences to certain slots.
    /// \details By using listCorrespondences->disconnect(), changes in the QListWidget will not
    /// have any effect. This is useful e.g. for clearing the whole list which will normally trigger
    /// gui changes for each delete operation. Use this function to connect the list afterwards again.
    /// Use with care, this may cause inconsistent information if not used correctly.
    void qtConnectListCorrespondences();
    /// \brief Connects changing operations of listShapes to certain slots.
    /// \details By using listShapes->disconnect(), changes in the QListWidget will not
    /// have any effect. This is useful i.e. for clearing the whole list which will normally trigger
    /// gui changes for each delete operation. Use this function to connect the list afterwards again.
    /// Use with care, this may cause inconsistent information if not used correctly.
    void qtConnectListShapes();

    
    void qtParseContextMenuItems(QMenu* menu, HashMap<QAction*, string>& customActions);
    //Show context menus at global position. Either called from qt slots or from VTK widget (right click on shape/correspondence)
    void qtShowContextMenuShapes(const QPoint& pos, vtkIdType pointId, vtkIdType faceId);
    void qtShowContextMenuCorrepondences(const QPoint& pos);
    
    void qtCreateMenuCustomTabs();

    void qtInputDialogRenameShape(CustomListWidgetItem<Shape>* item);
    void qtExportShapeDialog(Shape* shape);
    void qtInputDialogOpacity(Shape* shape);
    
    void qtCreateIdentityCorrespondences(Shape* shape1);
    void qtTransferCoordinateFunction(Shape* shape1);
    void qtCreateShapeSegment(Shape *shape, vtkIdType pointId);
    
    
    void qtUpdateLabelVisibleCorrespondences();
    
    template<class T>
    CustomListWidgetItem<VisualCorrespondence<T>>* qtAddListCorrespondencesItem(shared_ptr<VisualCorrespondence<T>> visualCorrespondence) {
        string label = "Correspondence ";
        if(typeid(T) == typeid(PointCorrespondence)) {
            label.append("P");
        } else {
            label.append("F");
        }
        label.append(to_string(visualCorrespondence->getCorrespondence()->getId()));
        CustomListWidgetItem<VisualCorrespondence<T>>* item = new CustomListWidgetItem<VisualCorrespondence<T>>(
                                                                                              QString::fromStdString(label),
                                                                                              visualCorrespondence);
        item->setToolTip(visualCorrespondence->getCorrespondence()->toString().c_str());
        this->listCorrespondences->addItem(item);
        return item;
    }
    
    //vtk
    template<class T>
    void vtkCorrespondenceClicked(VisualCorrespondence<T>* correspondence, vtkIdType cellId, QPoint &pos, unsigned long vtkEvent, vtkCommand *command) {
        command->AbortFlagOn();
        for(int i = 0; i < listCorrespondences->count(); i++) {
            if(((CustomListWidgetItem<VisualCorrespondence<T>>*) listCorrespondences->item(i))->getItem().get() == correspondence) {
                listCorrespondences->setCurrentRow(i);
                if(vtkEvent == vtkCommand::RightButtonPressEvent) {
                    qtShowContextMenuCorrepondences(pos);
                }
                break;
            }
        }
    }
    void vtkShapeClicked(shared_ptr<Shape> shape, vtkIdType pointId, vtkIdType faceId, QPoint &pos, unsigned long vtkEvent, vtkCommand *command);
    void vtkSetup();

    void importShape(vtkAlgorithmOutput* reader, string name);
    void addShape(shared_ptr<Shape> shape);
    void deleteShape(int i);
    void deleteFace(Shape* shape, vtkIdType faceId);
    void deleteCorrespondence(int i);
    void hideCorrespondence(int i);
    void hideCorrespondences();
    void clearCorrespondences();
    void sampleCorrespondences(unsigned int size);
    void pickCorrespondence();

    void clear();

    void showErrorMessage(string description, string error);
    
    
    /// \brief Maps the vtkActor pointer to a shape pointer.
    /// \details Access in linear time worst case. Usually constant time.
    HashMap<vtkActor*, shared_ptr<Shape>> shapes_;
    
    /// Maps the vtkActor to a FaceCorrespondence pointer.
    HashMap<vtkActor*, shared_ptr<VisualCorrespondence<FaceCorrespondence>>> visualFaceCorrespondences_;
    /// Maps the vtkActor to a PointCorrespondence pointer.
    HashMap<vtkActor*, shared_ptr<VisualCorrespondence<PointCorrespondence>>> visualPointCorrespondences_;
    
    /// \brief All PointCorrespondenceData instances.
    /// \details Maps a PointCorrespondenceData to a bool that indicates if for the data there exists a corresponding PointCorrespondence object.
    HashMap<shared_ptr<FaceCorrespondence>, bool> faceCorrespondences_;

    /// \brief All FaceCorrespondenceData instances.
    /// \details Maps a FaceCorrespondenceData to a bool that indicates if for the data there exists a corresponding FaceCorrespondence object.
    HashMap<shared_ptr<PointCorrespondence>, bool> pointCorrespondences_;

    
    //vtk stuff
    vtkSmartPointer<vtkRenderer> renderer_;
    vtkSmartPointer<vtkEventQtSlotConnect> connections_;
    
    
    unique_ptr<CorrespondencePicker<PointCorrespondence>> pointCorrespondencePicker_;
    unique_ptr<CorrespondencePicker<FaceCorrespondence>> faceCorrespondencePicker_;
    
    
    //QT
    QActionGroup* actionGroupCorrespondenceType_;
    QActionGroup* actionGroupMode_;
    QActionGroup* actionGroupVisualRepresentation_;
    QActionGroup* actionGroupProjectionMode_;
    

    HashMap<string, CustomTab*> customTabs_;
    HashMap<QAction*, string> viewCustomTabActions_;
    
    //counter for ids
    int lastInsertShapeId_;
    int lastInsertCorrespondenceId_;

    
    vtkSmartPointer<vtkScalarBarActor> scalarBar_;
};




#endif
