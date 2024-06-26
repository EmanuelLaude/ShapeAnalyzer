#include "ShapeAnalyzer.h"

// Constructor
ShapeAnalyzer::ShapeAnalyzer() : visualFaceCorrespondences_(1000), visualPointCorrespondences_(1000), faceCorrespondences_(100000), pointCorrespondences_(100000), lastInsertShapeId_(0), lastInsertCorrespondenceId_(0) {
    this->setupUi(this);
    
    //Group actions related to different Views. Automatically unselected other members of group.
    actionGroupCorrespondenceType_ = new QActionGroup(this);
    actionGroupCorrespondenceType_->addAction(this->actionDisplayFaceCorrespondences);
    actionGroupCorrespondenceType_->addAction(this->actionDisplayPointCorrespondences);
    
    
    //Group actions related to different modes.
    actionGroupMode_ = new QActionGroup(this);
    actionGroupMode_->addAction(this->actionTransformScene);
    actionGroupMode_->addAction(this->actionTransformShapes);
    actionGroupMode_->addAction(this->actionPickPointCorrespondences);
    actionGroupMode_->addAction(this->actionPickFaceCorrespondences);
    
    
    actionGroupVisualRepresentation_ = new QActionGroup(this);
    actionGroupVisualRepresentation_->addAction(this->actionShowSurface);
    actionGroupVisualRepresentation_->addAction(this->actionShowSurfaceNormals);
    actionGroupVisualRepresentation_->addAction(this->actionShowTriangulatedMesh);
    actionGroupVisualRepresentation_->addAction(this->actionShowPointCloud);
    
    
    actionGroupProjectionMode_ = new QActionGroup(this);
    actionGroupProjectionMode_->addAction(this->actionProjectionPerspective);
    actionGroupProjectionMode_->addAction(this->actionProjectionParallel);
    
    
    this->listShapes->setContextMenuPolicy(Qt::CustomContextMenu);
    this->listCorrespondences->setContextMenuPolicy(Qt::CustomContextMenu);
    
    
    // Set up action signals and slots
    connect(this->actionExit,                       SIGNAL(triggered()),
            this,                                   SLOT(slotExit()));
    
    connect(this->actionResetCamera,                SIGNAL(triggered()),
            this,                                   SLOT(slotResetCamera()));
    
    connect(this->actionClear,                      SIGNAL(triggered()),
            this,                                   SLOT(slotClear()));
    
    connect(this->actionOpenScene,                  SIGNAL(triggered()),
            this,                                   SLOT(slotOpenScene()));

    connect(this->actionImportShape,                SIGNAL(triggered()),
            this,                                   SLOT(slotImportShape()));
    
    connect(this->actionImportCorrespondences,      SIGNAL(triggered()),
            this,                                   SLOT(slotImportCorrespondences()));
    
    connect(this->actionSaveScene,                  SIGNAL(triggered()),
            this,                                   SLOT(slotSaveScene()));
    
    connect(this->actionExportCorrespondences,      SIGNAL(triggered()),
            this,                                   SLOT(slotExportCorrespondences()));
    
    connect(this->actionSaveScreenshot,             SIGNAL(triggered()),
            this,                                   SLOT(slotSaveScreenshot()));
    
    connect(this->actionGroupMode_,                 SIGNAL(triggered(QAction*)),
            this,                                   SLOT(slotClearCurrentSelection()));
    
    connect(this->actionGroupMode_,                 SIGNAL(triggered(QAction*)),
            this,                                   SLOT(slotToggleBoxWidget()));
    
    connect(this->actionGroupMode_,                 SIGNAL(triggered(QAction*)),
            this,                                   SLOT(slotModeAddCorrespondences()));

    connect(this->actionGroupCorrespondenceType_,   SIGNAL(triggered(QAction*)),
            this,                                   SLOT(slotSetCorrespondenceType()));
    
    connect(this->actionGroupVisualRepresentation_, SIGNAL(triggered(QAction*)),
                                                    SLOT(slotSetVisualRepresentationShape()));
    
    connect(this->actionHelp,                       SIGNAL(triggered()),
            this,                                   SLOT(slotShowHelp()));
    
    connect(this->actionSetBackgroundColor,         SIGNAL(triggered()),
            this,                                   SLOT(slotSetBackgroundColor()));
    
    connect(this->actionGroupProjectionMode_,       SIGNAL(triggered(QAction*)),
            this,                                   SLOT(slotToggleProjectionMode()));
    
    connect(this->actionShowScalarBar,              SIGNAL(toggled(bool)),
            this,                                   SLOT(slotShowScalarBar(bool)));
    
    connect(this->buttonHideCorrespondences,        SIGNAL(released()),
            this,                                   SLOT(slotHideVisibleCorrespondences()));
    
    connect(this->buttonSampleCorrespondences,      SIGNAL(released()),
            this,                                   SLOT(slotSampleCorrespondences()));
    
    connect(this->buttonClearAllCorrespondences,    SIGNAL(released()),
            this,                                   SLOT(slotClearAllCorrespondences()));

    
    // connection of list widgets is done in extra functions since signals of
    // list widgets are disconnected before and reconnected after deletion of
    // list items
    qtConnectListCorrespondences();
    qtConnectListShapes();

    this->vtkSetup();
    
    CustomClassesRegistry::registerTabs();
    CustomClassesRegistry::registerContextMenuItems();
    
    //Initialize Slepc for eigenfunction computation
    SlepcInitializeNoArguments();
    
    qtCreateMenuCustomTabs();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::qtConnectListCorrespondences() {
    connect(this->listCorrespondences,              SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this,                                   SLOT(slotSetSelectedCurrentCorrespondence(QListWidgetItem*, QListWidgetItem*)));
    connect(this->listCorrespondences,              SIGNAL(customContextMenuRequested(const QPoint&)),
            this,                                   SLOT(slotShowContextMenuCorrespondences(const QPoint&)));
    
    
    slotSetSelectedCurrentCorrespondence(listCorrespondences->currentItem(), nullptr);
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::qtConnectListShapes() {
    connect(this->listShapes,                       SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this,                                   SLOT(slotSetSelectedCurrentShape(QListWidgetItem*, QListWidgetItem*)));
    connect(this->listShapes,                       SIGNAL(customContextMenuRequested(const QPoint&)),
            this,                                   SLOT(slotShowContextMenuShapes(const QPoint&)));
    
    //manually call slot set box widget
    slotSetSelectedCurrentShape(listShapes->currentItem(), nullptr);
}

///////////////////////////////////////////////////////////////////////////////
// Public Functions
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::render() {
    this->qvtkWidget->GetRenderWindow()->Render();
}


///////////////////////////////////////////////////////////////////////////////
// Functions handling QT
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
bool ShapeAnalyzer::eventFilter(QObject *object, QEvent *event) {
    //catch and ignore mouse wheel events since they cause strange behavior on Mac OSX
    if (object == qvtkWidget && event->type() == QEvent::Wheel) {
        return true;
    }
    
    return false;
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::qtInputDialogOpacity(Shape* shape) {
    bool ok;
    double opacity = QInputDialog::getDouble (
                                              this,
                                              tr("Opacity"),
                                              tr("Choose a value between 0 and 1"),
                                              shape->getActor()->GetProperty()->GetOpacity(),
                                              0.0,
                                              1.0,
                                              2,
                                              &ok
                                              );
    // change opacity if ok was given
    if (ok) {
        shape->getActor()->GetProperty()->SetOpacity(opacity);
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::qtInputDialogRenameShape(CustomListWidgetItem<Shape>* item) {
    bool ok;
    QString name = QInputDialog::getText (
                                           this,
                                           tr("Rename"),
                                           tr("New Name"),
                                           QLineEdit::Normal,
                                           item->text(),
                                           &ok
                                           );
    // rename if ok was given
    if (ok) {
        scalarBar_->SetTitle(name.toStdString().c_str());
        
        item->setText(name);
        item->getItem()->setName(name.toStdString());
        
        
        // fire event for shapeTabs
        for(auto entry : customTabs_) {
            entry.second->onShapeEdit(item->getItem().get());
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::qtExportShapeDialog(Shape* shape) {
    QStringList types;
    types << "OFF" << "Tosca"<< "PLY (ASCII)" <<"PLY (Binary)"<<"OBJ";
    
    bool ok;
    QString type = QInputDialog::getItem(this, "Which file type?", "Write to file", types, 0, true, &ok);
    
    if(!ok) {
        return;
    }
    
    if(type == "OFF") {
        vtkSmartPointer<vtkOFFWriter> writer = vtkSmartPointer<vtkOFFWriter>::New();
        writer->SetInputData(shape->getPolyData());
        QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), tr(""), tr("OFF Files (*.off)"));
        if(filename.isEmpty()) {
            return;
        }
        writer->SetFileName(filename.toStdString().c_str());
        writer->Write();
    } else if(type == "Tosca") {
        vtkSmartPointer<vtkToscaWriter> writer = vtkSmartPointer<vtkToscaWriter>::New();
        writer->SetInputData(shape->getPolyData());
        QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), tr(""), tr("Tosca Files (*.vert)"));
        if(filename.isEmpty()) {
            return;
        }
        writer->SetFileName(filename.toStdString().c_str());
        writer->Write();
    } else if(type == "PLY (ASCII)") {
        vtkSmartPointer<vtkPLYWriter> writer = vtkSmartPointer<vtkPLYWriter>::New();
        writer->SetFileTypeToASCII();
        writer->SetInputData(shape->getPolyData());
        QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), tr(""), tr("ASCII PLY Files (*.ply)"));
        if(filename.isEmpty()) {
            return;
        }
        writer->SetFileName(filename.toStdString().c_str());
        writer->Write();
    } else if(type == "PLY (Binary)") {
        vtkSmartPointer<vtkPLYWriter> writer = vtkSmartPointer<vtkPLYWriter>::New();
        writer->SetFileTypeToBinary();
        writer->SetInputData(shape->getPolyData());
        QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), tr(""), tr("Binary PLY Files (*.ply)"));
        if(filename.isEmpty()) {
            return;
        }
        writer->SetFileName(filename.toStdString().c_str());
        writer->Write();
    } else {
        vtkSmartPointer<vtkOBJWriter> writer = vtkSmartPointer<vtkOBJWriter>::New();
        writer->SetInputData(shape->getPolyData());
        QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), tr(""), tr("OBJ Files (*.obj)"));
        if(filename.isEmpty()) {
            return;
        }
        writer->SetFileName(filename.toStdString().c_str());
        writer->Write();
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::qtShowContextMenuCorrepondences(const QPoint &pos) {
    QMenu menu;
    QAction* hideAction   = menu.addAction("Hide");
    QAction* deleteAction   = menu.addAction("Delete");
    // ...
    
    QAction* selectedItem = menu.exec(pos);
    if (selectedItem == deleteAction) { // deletes correspondence data
        deleteCorrespondence(this->listCorrespondences->currentRow());
    } else if (selectedItem == hideAction) { // correspondence data is stored, vtkActor is deleted
        hideCorrespondence(this->listCorrespondences->currentRow());
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::qtParseContextMenuItems(QMenu* menu, HashMap<QAction*, string>& customActions) {
    // get list of all menu item paths (Home>>foo>>bar>>action)
    const vector<pair<string, string>>& paths = CustomContextMenuItemFactory::getInstance()->getLabels();
    
    // menus in the menu tree indexed by their unique path.
    HashMap<string, QMenu*> menus;
    
    // for each path A (...>>A>>...) create a unique menu in the menu tree
    for(auto entry : paths) {
        QString path(entry.second.c_str());
        QStringList list = path.split(">>");
        int i = 0;
        
        QString currentPath = "";
        QString parent = "";
        
        // iterate over all path elements except the leaf and create a menu in the hierarchy
        for(; i < list.size() - 1; i++) {
            currentPath.append((i != 0 ? ">>" : "")).append(list[i]);
            
            QMenu* currentMenu;
            if(menus.containsKey(currentPath.toStdString())) {
                currentMenu = menus[currentPath.toStdString()];
            } else {
                currentMenu = new QMenu(list[i]);
                if(i == 0) {
                    menu->addMenu(currentMenu);
                }
                menus.insert(currentPath.toStdString(), currentMenu);
                if(menus.containsKey(parent.toStdString())) {
                    menus[parent.toStdString()]->addMenu(currentMenu);
                }
            }
            
            parent = currentPath;
        }
        
        // create a action for the leaf (last element of path) and return it via customActions (here the actions are indexed by the unique identifier of the ContextMenuItem class)
        if(i > 0) {
            QAction* action = menus[parent.toStdString()]->addAction(list[i]);
            customActions.insert(action, entry.first);
        } else {
            QAction* action = menu->addAction(list[i]);
            customActions.insert(action, entry.first);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::qtShowContextMenuShapes(const QPoint &pos, vtkIdType pointId, vtkIdType faceId) {
    QMenu myMenu;

    QAction* opacityAction  = myMenu.addAction("Set Opacity");
    QAction* clearAction    = myMenu.addAction("Clear Coloring");
    myMenu.addSeparator();
    QAction* renameAction   = myMenu.addAction("Rename");
    QAction* deleteAction   = myMenu.addAction("Delete");
    QAction* volumAction    = myMenu.addAction("Create Tetrahedralized Shape");
    QAction* exportAction   = myMenu.addAction("Export Shape");
    myMenu.addSeparator();

    //create custom menu items out of the list of custom context menu items registered in the CustomContextMenuItemFactory
    HashMap<QAction*, string> customActions;
    qtParseContextMenuItems(&myMenu, customActions);
    
    QAction* selectedItem = myMenu.exec(pos);
    if (selectedItem == deleteAction) {
        deleteShape(this->listShapes->currentRow());
    } else if (selectedItem == volumAction) {
        VolumetricShape test = VolumetricShape(((CustomListWidgetItem<Shape> *)
                                                this->listShapes->currentItem())->getItem().get());
    } else if (selectedItem == clearAction) {
        ((CustomListWidgetItem<Shape>*) this->listShapes->currentItem())->getItem()->clearColoring();
    } else if (selectedItem == renameAction) {
        qtInputDialogRenameShape((CustomListWidgetItem<Shape>*) this->listShapes->currentItem());
    } else if (selectedItem == exportAction) {
        qtExportShapeDialog(((CustomListWidgetItem<Shape> *) this->listShapes->currentItem())->getItem().get());
    } else if (selectedItem == opacityAction) {
        qtInputDialogOpacity(((CustomListWidgetItem<Shape> *) this->listShapes->currentItem())->getItem().get());
    } else {
        if(customActions.containsKey(selectedItem)) {
            shared_ptr<CustomContextMenuItem> menuItem = shared_ptr<CustomContextMenuItem>(CustomContextMenuItemFactory::getInstance()->create(customActions[selectedItem], ((CustomListWidgetItem<Shape> *) this->listShapes->currentItem())->getItem(), dynamic_cast<ShapeAnalyzerInterface*>(this)));
            menuItem->onClick(pointId, faceId, this);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::qtCreateMenuCustomTabs() {
    const vector<pair<string, string>>& paths = CustomTabFactory::getInstance()->getLabels();
    
    // for each key + path pair create entry in view menu and connect it to slotViewCustomTab.
    for(auto entry : paths) {
        QString path(entry.second.c_str());
        QStringList list = path.split(">>");
        QAction* action;
        if(list.count() < 2) {
            action = this->menuShapes->addAction(path);
        } else {
            if(list[0] == "Correspondences") {
                action = this->menuCorrespondences->addAction(list[1]);
            } else if (list[0] == "Shapes")  {
                action = this->menuShapes->addAction(list[1]);
            } else {
                // only correspondence and shape are allowed as sections
                break;
            }
        }
        action->setCheckable(true);
        action->setChecked(false);
        
        // remember which action is responsible for which CustomTab class
        viewCustomTabActions_.insert(action, entry.first);
        
        connect(action, SIGNAL(toggled(bool)), this, SLOT(slotViewCustomTab(bool)));
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::qtUpdateLabelVisibleCorrespondences() {
    int numberOfVisibleCorrespondences = 0;
    int numberOfAllCorrespondences = 0;
    if(this->actionDisplayPointCorrespondences->isChecked()) {
        numberOfAllCorrespondences = pointCorrespondences_.size();
        numberOfVisibleCorrespondences = visualPointCorrespondences_.size();
    } else {
        numberOfAllCorrespondences = faceCorrespondences_.size();
        numberOfVisibleCorrespondences = visualFaceCorrespondences_.size();
    }
    
    double ratio = 100.0;
    if(numberOfAllCorrespondences > 0) {
        ratio = ( (double) numberOfVisibleCorrespondences / (double) numberOfAllCorrespondences ) * 100.0;
    }
    
    stringstream ss;
    ss << "Visible: "<<numberOfVisibleCorrespondences<<" out of "<<numberOfAllCorrespondences<<" ("<< ratio <<" %)";
    
    this->labelNumberOfVisibleCorrespondences->setText(QString(ss.str().c_str()));
}

///////////////////////////////////////////////////////////////////////////////
// QT slots
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotViewCustomTab(bool visible) {
    // obtain sender action and query key of selected CustomTab
    string key = viewCustomTabActions_[(QAction*) sender()];
    if(visible) {
        CustomTab* tab = CustomTabFactory::getInstance()->create(key, shapes_, pointCorrespondences_, faceCorrespondences_, this);
        
        QString path(CustomTabFactory::getInstance()->getLabel(key).c_str());
        QStringList list = path.split(">>");

        if((list.count() > 1 && list[0] == "Shapes") || list.count() < 2) {
            int i = this->tabWidgetShapes->addTab(dynamic_cast<QWidget*>(tab), (list.count() > 1 ? list[1] : path));
            this->tabWidgetShapes->setCurrentIndex(i);
        } else {
            int i = this->tabWidgetCorrespondences->addTab(dynamic_cast<QWidget*>(tab), list[1]);
            this->tabWidgetCorrespondences->setCurrentIndex(i);
        }

        customTabs_.insert(key, tab);
    } else {
        delete customTabs_[key];
        customTabs_.remove(key);
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotExit() {
    qApp->exit();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotSetBackgroundColor() {
    double dcurrentColor[3];
    renderer_->GetBackground(dcurrentColor);
    QColor currentColor;
    currentColor.setRed(dcurrentColor[0] * 255);
    currentColor.setGreen(dcurrentColor[1] * 255);
    currentColor.setBlue(dcurrentColor[2] * 255);
    QColor color = QColorDialog::getColor(currentColor);
    renderer_->SetBackground(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0);
    renderer_->GetRenderWindow()->Render();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotClearCurrentSelection() {
    pointCorrespondencePicker_->clearSelection();
    faceCorrespondencePicker_->clearSelection();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotHideVisibleCorrespondences() {
    hideCorrespondences();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotSampleCorrespondences() {
    sampleCorrespondences(this->spinBoxNumberOfSamples->value());
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotClearAllCorrespondences() {
    clearCorrespondences();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotSetCorrespondenceType() {
    // remove all current face/point correspondences from renderer
    // then add visible point/face correspondences to renderer
    if(this->actionDisplayFaceCorrespondences->isChecked()) {
        this->labelTypeCorrespondences->setText("Type: Face Correspondences");
        if(!actionPickFaceCorrespondences->isChecked() && actionPickPointCorrespondences->isChecked()) {
            actionPickFaceCorrespondences->setChecked(true);
            actionPickFaceCorrespondences->trigger();
        }

        

        // vtk: remove all point correspondences from renderer
        for(int i = listCorrespondences->count()-1; i > -1; i--) {
            //get correspondence
            CustomListWidgetItem<VisualCorrespondence<PointCorrespondence>>* item = (CustomListWidgetItem<VisualCorrespondence<PointCorrespondence>>*) listCorrespondences->item(i);
            shared_ptr<VisualCorrespondence<PointCorrespondence>> correspondence = item->getItem();
            
            correspondence->removeFromRenderer();
        }
        // clear list
        this->listCorrespondences->clear();
        
        
        // add face correspondences from before
        for (auto entry : visualFaceCorrespondences_) {
            // add to list
            qtAddListCorrespondencesItem<FaceCorrespondence>(entry.second);
            
            // add to renderer
            entry.second->addToRenderer();
        }

        if(visualFaceCorrespondences_.size() > 0)
            listCorrespondences->setCurrentRow(0);
        
    } else {
        this->labelTypeCorrespondences->setText("Type: Point Correspondences");
        if(!actionPickPointCorrespondences->isChecked() && actionPickFaceCorrespondences->isChecked()) {
            actionPickPointCorrespondences->setChecked(true);
            actionPickPointCorrespondences->trigger();
        }
        
        if(tabWidgetCorrespondences->tabText(tabWidgetCorrespondences->currentIndex()) != "All Point Correspondences") {
            tabWidgetCorrespondences->setCurrentIndex(0);
        }

        // vtk: remove all face correspondences from renderer
        for(int i = listCorrespondences->count()-1; i > -1; i--) {
            //get correspondence
            CustomListWidgetItem<VisualCorrespondence<FaceCorrespondence>>* item = (CustomListWidgetItem<VisualCorrespondence<FaceCorrespondence>>*) listCorrespondences->item(i);
            shared_ptr<VisualCorrespondence<FaceCorrespondence>> correspondence = item->getItem();
            
            correspondence->removeFromRenderer();
        }
        // clear list
        this->listCorrespondences->clear();
        
        
        // add point correspondences from before
        for (auto entry : visualPointCorrespondences_) {
            // add to list
            qtAddListCorrespondencesItem<PointCorrespondence>(entry.second);
            
            // add to renderer
            entry.second->addToRenderer();
        }
        
        if(visualPointCorrespondences_.size() > 0)
            listCorrespondences->setCurrentRow(0);
    }
    
    this->qvtkWidget->GetRenderWindow()->Render();
    
    qtUpdateLabelVisibleCorrespondences();
}

///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotShowScalarBar(bool checked) {
    if(checked) {
        scalarBar_->SetVisibility(1);
        scalarBar_->Modified();
        this->qvtkWidget->GetRenderWindow()->Render();
    } else {
        scalarBar_->SetVisibility(0);
        scalarBar_->Modified();
        this->qvtkWidget->GetRenderWindow()->Render();
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotToggleProjectionMode() {
    vtkSmartPointer<vtkCamera> camera = renderer_->GetActiveCamera();
    
    if (!actionProjectionParallel->isChecked())
        camera->ParallelProjectionOff();
    else
        camera->ParallelProjectionOn();
    
    renderer_->GetRenderWindow()->Render();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotSetVisualRepresentationShape() {
    for(auto entry : shapes_) {
        if(this->actionShowSurface->isChecked()) {
            entry.second->setVisualRepresentation(Shape::VisualRepresentation::MeshSurface);
        } else if(this->actionShowSurfaceNormals->isChecked()) {
            entry.second->setVisualRepresentation(Shape::VisualRepresentation::InterpolatedNormals);
        } else if(this->actionShowTriangulatedMesh->isChecked()) {
            entry.second->setVisualRepresentation(Shape::VisualRepresentation::Mesh);
        } else if(this->actionShowPointCloud->isChecked()) {
            entry.second->setVisualRepresentation(Shape::VisualRepresentation::PointCloud);
        } else {
            ;
        }
    }
    
    this->qvtkWidget->GetRenderWindow()->Render();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotShowHelp() {
    
    QDialog* dialog = new QDialog(this);
    
    Ui_HelpDialog ui;
    ui.setupUi(dialog);
    
    dialog->show();
    
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotOpenScene() {
    pointCorrespondencePicker_->clearSelection();
    faceCorrespondencePicker_->clearSelection();
    
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Scene"),
                                                    tr(""),
                                                    tr("Files (*.bin *.txt)"));
    
    if(filename.isEmpty())
        return;
    
    clear();
    
    renderer_->ResetCamera();
    
    vector<shared_ptr<Shape>> shapes;
    
    vector<pair<shared_ptr<PointCorrespondence>, shared_ptr<VisualCorrespondence<PointCorrespondence>>>> pointCorrespondences;
    vector<pair<shared_ptr<FaceCorrespondence>, shared_ptr<VisualCorrespondence<FaceCorrespondence>>>> faceCorrespondences;
    
    try {
        
    if(filename.endsWith(tr(".bin"))) {
        pair<int, int> lastInsertIds = SceneWriterReader::importSceneBinary(filename.toStdString(), renderer_, shapes, pointCorrespondences, faceCorrespondences);
        lastInsertShapeId_ = lastInsertIds.first;
        lastInsertCorrespondenceId_ = lastInsertIds.second;
    } else {
        pair<int, int> lastInsertIds = SceneWriterReader::importSceneASCII(filename.toStdString(), renderer_, shapes, pointCorrespondences, faceCorrespondences);
        lastInsertShapeId_ = lastInsertIds.first;
        lastInsertCorrespondenceId_ = lastInsertIds.second;
    }
    
    for(auto it : shapes) {
        addShape(it);
    }
    
    for(auto entry : pointCorrespondences) {
        if(entry.second != nullptr) {
            pointCorrespondences_.insert(entry.first, true);
            visualPointCorrespondences_.insert(entry.second->getActor(), entry.second);
            
            // add correspondence to qt list widget
            if(actionDisplayPointCorrespondences->isChecked()) {
                entry.second->addToRenderer();
                
                qtAddListCorrespondencesItem(entry.second);
            }
        } else {
            pointCorrespondences_.insert(entry.first, false);
        }
    }

    for(auto entry : faceCorrespondences) {
        if(entry.second != nullptr) {
            faceCorrespondences_.insert(entry.first, true);
            visualFaceCorrespondences_.insert(entry.second->getActor(), entry.second);
            
            // add correspondence to qt list widget
            if(actionDisplayFaceCorrespondences->isChecked()) {
                entry.second->addToRenderer();
                
                qtAddListCorrespondencesItem(entry.second);
            }
        } else {
            faceCorrespondences_.insert(entry.first, false);
        }
    }
        
    } catch(IOError& e) {
        showErrorMessage("The Scene could not be opened", e.what());
    }
    
    
    qvtkWidget->GetRenderWindow()->Render();
    qtUpdateLabelVisibleCorrespondences();
    qvtkWidget->GetRenderWindow()->Render();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotImportShape() {
    pointCorrespondencePicker_->clearSelection();
    faceCorrespondencePicker_->clearSelection();
    
    QString filename = QFileDialog::getOpenFileName(this, tr("Import Shape"),
                                                    tr(""),
                                                    tr("Files (*.off *.vert *.ply *.obj)"));
    
    if(filename.isEmpty())
        return;
    
    if(filename.endsWith(tr(".off"))) {
        // read .off file
        vtkSmartPointer<vtkOFFReader> reader = vtkSmartPointer<vtkOFFReader>::New();
        
        // vtk error handling
        vtkSmartPointer<ErrorObserver> errorObserver = vtkSmartPointer<ErrorObserver>::New();
        reader->AddObserver(vtkCommand::ErrorEvent, errorObserver);
        reader->AddObserver(vtkCommand::WarningEvent, errorObserver);
        
        reader->SetFileName(filename.toStdString().c_str());
        
        vtkAlgorithmOutput* output;
        output = reader->GetOutputPort();
        
        importShape(output, filename.mid(filename.lastIndexOf('/')+1, filename.lastIndexOf('.')-filename.lastIndexOf('/')-1).toStdString());
        
        if (errorObserver->GetError()) {
            showErrorMessage("The file cound not be opened", errorObserver->GetErrorMessage());
            return;
        }
        if (errorObserver->GetWarning()) {
            showErrorMessage("There was a warning reading the file", errorObserver->GetWarningMessage());
        }
    } else if(filename.endsWith(tr(".vert"))) {
        // read .tri .vert files
        vtkSmartPointer<vtkToscaReader> reader = vtkSmartPointer<vtkToscaReader>::New();
        
        // vtk error handling
        vtkSmartPointer<ErrorObserver> errorObserver = vtkSmartPointer<ErrorObserver>::New();
        reader->AddObserver(vtkCommand::ErrorEvent, errorObserver);
        reader->AddObserver(vtkCommand::WarningEvent, errorObserver);
        
        reader->SetFileName(filename.toStdString().c_str());
        
        vtkAlgorithmOutput* output;
        output = reader->GetOutputPort();
        
        importShape(output, filename.mid(filename.lastIndexOf('/')+1, filename.lastIndexOf('.')-filename.lastIndexOf('/')-1).toStdString());
        
        if (errorObserver->GetError()) {
            showErrorMessage("The file cound not be opened", errorObserver->GetErrorMessage());
            return;
        }
        if (errorObserver->GetWarning()) {
            showErrorMessage("There was a warning reading the file", errorObserver->GetWarningMessage());
        }
    } else if(filename.endsWith(".ply")) {
        // read .ply file
        vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
        
        // vtk error handling
        vtkSmartPointer<ErrorObserver> errorObserver = vtkSmartPointer<ErrorObserver>::New();
        reader->AddObserver(vtkCommand::ErrorEvent, errorObserver);
        reader->AddObserver(vtkCommand::WarningEvent, errorObserver);
        
        reader->SetFileName(filename.toStdString().c_str());
        
        vtkAlgorithmOutput* output;
        output = reader->GetOutputPort();
        
        importShape(output, filename.mid(filename.lastIndexOf('/')+1, filename.lastIndexOf('.')-filename.lastIndexOf('/')-1).toStdString());
        
        if (errorObserver->GetError()) {
            showErrorMessage("The file cound not be opened", errorObserver->GetErrorMessage());
            return;
        }
        if (errorObserver->GetWarning()) {
            showErrorMessage("There was a warning reading the file", errorObserver->GetWarningMessage());
        }
    } else {
        // read .obj file
        vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
        
        // vtk error handling
        vtkSmartPointer<ErrorObserver> errorObserver = vtkSmartPointer<ErrorObserver>::New();
        reader->AddObserver(vtkCommand::ErrorEvent, errorObserver);
        reader->AddObserver(vtkCommand::WarningEvent, errorObserver);
        
        reader->SetFileName(filename.toStdString().c_str());
        
        vtkAlgorithmOutput* output;
        output = reader->GetOutputPort();
        
        importShape(output, filename.mid(filename.lastIndexOf('/')+1, filename.lastIndexOf('.')-filename.lastIndexOf('/')-1).toStdString());
        
        if (errorObserver->GetError()) {
            showErrorMessage("The file cound not be opened", errorObserver->GetErrorMessage());
            return;
        }
        if (errorObserver->GetWarning()) {
            showErrorMessage("There was a warning reading the file", errorObserver->GetWarningMessage());
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotImportCorrespondences() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    tr(""),
                                                    tr("Files (*.txt *.bin)"));
    
    if(filename.isEmpty())
        return;

    vector<shared_ptr<Shape>> shapes;
    shapes_.getValues(shapes);
    
    ShapeComparator comp;
    // sort shapes to guarantee some ordering (in this case orderd by ID) which is not provided by unordered_map
    std::sort(shapes.begin(), shapes.end(), comp);
    
    vector<shared_ptr<PointCorrespondence>> pointCorrespondences;
    vector<shared_ptr<FaceCorrespondence>> faceCorrespondences;
    
    function<shared_ptr<PointCorrespondence>()> pointCorrespondenceFactory = [this]()->shared_ptr<PointCorrespondence> {
        return make_shared<PointCorrespondence>(lastInsertCorrespondenceId_++);
    };
    
    function<shared_ptr<FaceCorrespondence>()> faceCorrespondenceFactory = [this]()->shared_ptr<FaceCorrespondence> {
        return make_shared<FaceCorrespondence>(lastInsertCorrespondenceId_++);
    };
    
    try {
        
    if(filename.endsWith(".txt")) {
        SceneWriterReader::importCorrespondencesASCII(filename.toStdString(), pointCorrespondenceFactory, faceCorrespondenceFactory, pointCorrespondences, faceCorrespondences, shapes, this);
    } else {
        SceneWriterReader::importCorrespondencesBinary(filename.toStdString(), pointCorrespondenceFactory, faceCorrespondenceFactory, pointCorrespondences, faceCorrespondences, shapes, this);
    }
    
    
    // insert point correspondences
    for(auto c : pointCorrespondences) {
        pointCorrespondences_.insert(c, false);
    }
    
    // insert face correspondences and fire corresponding events if vector not empty
    for(auto c : faceCorrespondences) {
        faceCorrespondences_.insert(c, false);
    }
        
    } catch(IOError& e) {
        showErrorMessage("The Correspondences could not be imported", e.what());
    }
    
    qtUpdateLabelVisibleCorrespondences();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotSaveScene() {
    QStringList formats;
    formats << "Binary" << "Text (ASCII)";
    
    bool ok;
    QString format = QInputDialog::getItem(this, "Format", "Choose file format.", formats, 0, true, &ok);
    
    if(!ok) {
        return;
    }
    
    vector<shared_ptr<Shape>> shapes;
    shapes_.getValues(shapes);
    
    ShapeComparator comp;
    // sort shapes to guarantee some ordering (in this case orderd by ID) which is not provided by unordered_map
    std::sort(shapes.begin(), shapes.end(), comp);
    
    vector<pair<shared_ptr<PointCorrespondence>, shared_ptr<VisualCorrespondence<PointCorrespondence>>>> pointCorrespondences;
    vector<pair<shared_ptr<FaceCorrespondence>, shared_ptr<VisualCorrespondence<FaceCorrespondence>>>> faceCorrespondences;
    
    for(auto i : pointCorrespondences_) {
        if(i.second) {
            for(auto j : visualPointCorrespondences_) {
                if(j.second->getCorrespondence() == i.first) {
                    pointCorrespondences.push_back(make_pair(i.first, j.second));
                    break;
                }
            }
        } else {
            pointCorrespondences.push_back(make_pair(i.first, nullptr));
        }
    }

    for(auto i : faceCorrespondences_) {
        if(i.second) {
            for(auto j : visualFaceCorrespondences_) {
                if(j.second->getCorrespondence() == i.first) {
                    faceCorrespondences.push_back(make_pair(i.first, j.second));
                    break;
                }
            }
        } else {
            faceCorrespondences.push_back(make_pair(i.first, nullptr));
        }
    }
    
    if(format == formats.value(0)) {
        QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), tr(""), tr("Binary Scene Files (*.bin)"));
        if(filename.isEmpty()) {
            return;
        }
        SceneWriterReader::exportSceneBinary(filename.toStdString(), renderer_, shapes, lastInsertShapeId_, pointCorrespondences, faceCorrespondences, lastInsertCorrespondenceId_);
    } else {
        QString filename = QFileDialog::getSaveFileName(this, tr("Save file"), tr(""), tr("Text (ASCII) Scene Files (*.txt)"));
        if(filename.isEmpty()) {
            return;
        }
        SceneWriterReader::exportSceneASCII(filename.toStdString(), renderer_, shapes, lastInsertShapeId_, pointCorrespondences, faceCorrespondences, lastInsertCorrespondenceId_);
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotExportCorrespondences() {
    QStringList types;
    types << "Point Correspondences" << "Face Correspondences";
    bool ok; // stores if user pressed ok button
    QString save = QInputDialog::getItem(this, tr("Which Correspondences?"), tr("Correspondences"), types, 0, true, &ok);
    
    if(!ok) {
        return;
    }
    
    QStringList formats;
    formats << "Binary" << "Text (ASCII)";
    
    
    QString format = QInputDialog::getItem(this, "Format", "Choose file format.", formats, 0, true, &ok);
    
    if(!ok) {
        return;
    }

    
    //sort shapes since result of HashMap.getValues is unordered
    vector<shared_ptr<Shape>> shapes;
    shapes_.getValues(shapes);
    
    ShapeComparator comp;
    
    std::sort(shapes.begin(), shapes.end(), comp);
    
    if (save == types.value(0) && pointCorrespondences_.size() > 0) {
        if(format == formats.value(0)) {
            QString filename = QFileDialog::getSaveFileName(
                                                            this,
                                                            tr("Save file"),
                                                            tr(""),
                                                            tr("Binary Point Correspondence Files (*.bin)")
                                                            );
            if (!filename.isEmpty()) {
                SceneWriterReader::exportPointCorrespondencesBinary(pointCorrespondences_, shapes, filename.toStdString());
            }
        } else {
            QString filename = QFileDialog::getSaveFileName(
                                                            this,
                                                            tr("Save file"),
                                                            tr(""),
                                                            tr("ASCII Point Correspondence Files (*.txt)")
                                                            );
            if (!filename.isEmpty()) {
                SceneWriterReader::exportPointCorrespondencesASCII(pointCorrespondences_, shapes, filename.toStdString());
            }
        }
    }
    
    
    if (save == types.value(1) && faceCorrespondences_.size() > 0) {
        if(format == formats.value(0)) {
            QString filename = QFileDialog::getSaveFileName(
                                                            this,
                                                            tr("Save file"),
                                                            tr(""),
                                                            tr("Binary Face Correspondence Files (*.bin)")
                                                            );
            if (!filename.isEmpty()) {
                SceneWriterReader::exportFaceCorrespondencesBinary(faceCorrespondences_, shapes, filename.toStdString());
            }
        } else {
            QString filename = QFileDialog::getSaveFileName(
                                                            this,
                                                            tr("Save file"),
                                                            tr(""),
                                                            tr("ASCII Face Correspondence Files (*.txt)")
                                                            );
            if (!filename.isEmpty()) {
                SceneWriterReader::exportFaceCorrespondencesASCII(faceCorrespondences_, shapes, filename.toStdString());
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotSaveScreenshot() {
    QDialog* dialog = new QDialog(this, 0);
    Ui_SaveScreenshotDialog ui;
    ui.setupUi(dialog);

    if(dialog->exec() == QDialog::Rejected) {
        return;
    }
    
    
    vtkSmartPointer<vtkGL2PSExporter> exporter = vtkSmartPointer<vtkGL2PSExporter>::New();
    
    
    QString filename;
    if(ui.comboBoxFileFormat->currentText() == "PNG") {
        filename = QFileDialog::getSaveFileName(this, tr("Save scene as image"), tr(""), tr("PNG (*.png)"));
    } else if(ui.comboBoxFileFormat->currentText() == "PDF") {
        filename = QFileDialog::getSaveFileName(this, tr("Save scene as image"), tr(""), tr("PDF (*.pdf)"));
        exporter->SetFileFormatToPDF();
    } else if(ui.comboBoxFileFormat->currentText() == "SVG") {
        filename = QFileDialog::getSaveFileName(this, tr("Save scene as image"), tr(""), tr("SVG (*.svg)"));
        exporter->SetFileFormatToSVG();
    } else if(ui.comboBoxFileFormat->currentText() == "PS") {
        filename = QFileDialog::getSaveFileName(this, tr("Save scene as image"), tr(""), tr("PS (*.ps)"));
        exporter->SetFileFormatToPS();
    } else if(ui.comboBoxFileFormat->currentText() == "EPS") {
        filename = QFileDialog::getSaveFileName(this, tr("Save scene as image"), tr(""), tr("EPS (*.eps)"));
        exporter->SetFileFormatToEPS();
    }
    
    if(filename.isEmpty())
        return;
    
    vtkSmartPointer<vtkCamera> oldCamera = renderer_->GetActiveCamera();
    
    // if checked, rerender to fit the whole scene
    if(ui.checkBoxResetCamera->isChecked()){
        vtkSmartPointer<vtkCamera> newCamera = vtkSmartPointer<vtkCamera>::New();
        newCamera->DeepCopy(oldCamera);
        
        renderer_->SetActiveCamera(newCamera);
        renderer_->ResetCamera();
        renderer_->GetRenderWindow()->Render();
    }
    
    
    if(ui.comboBoxFileFormat->currentText() != "PNG") {
        exporter->SetRenderWindow(renderer_->GetRenderWindow());
        exporter->SetFilePrefix(filename.split('.')[0].toStdString().c_str());
        exporter->SetCompress(0);
        exporter->Write();
    } else {
        // save in file
        vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
        windowToImageFilter->SetInput(renderer_->GetRenderWindow());
        // transparency settings
        if (!(ui.checkBoxTransparency->isChecked()))
            windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
        windowToImageFilter->ShouldRerenderOff();
        windowToImageFilter->Update();
        
        vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
        writer->SetFileName(filename.toStdString().c_str());
        writer->SetInputConnection(windowToImageFilter->GetOutputPort());
        writer->Write();
    }
    
    
    
    // if checked, rerender to fit the whole scene
    if(ui.checkBoxResetCamera->isChecked()){
        renderer_->SetActiveCamera(oldCamera);
        renderer_->GetRenderWindow()->Render();
    }
}



///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotShowContextMenuCorrespondences(const QPoint& pos) {
    int currentRow = this->listCorrespondences->currentRow();
    
    // menu appears only if item was selected
    if(currentRow >= 0) {
        
        QPoint globalPos = this->listCorrespondences->mapToGlobal(pos);

        qtShowContextMenuCorrepondences(globalPos);
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotShowContextMenuShapes(const QPoint& pos) {
    int currentRow = this->listShapes->currentRow();
    
    // menu appears only if item was selected
    if(currentRow >= 0) {
        
        QPoint globalPos = this->listShapes->mapToGlobal(pos);

        qtShowContextMenuShapes(globalPos, -1, -1);
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotToggleBoxWidget() {
    if(listShapes->count() > 0) {
        shared_ptr<Shape> selectedShape = ((CustomListWidgetItem<Shape>*) listShapes->currentItem())->getItem();
        if(this->actionTransformShapes->isChecked()) {
            selectedShape->setShowBoxWidget(true);
        } else {
            selectedShape->setShowBoxWidget(false);
        }
    }
    render();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotModeAddCorrespondences() {
    if(this->actionPickPointCorrespondences->isChecked()) {
        if(!this->actionDisplayPointCorrespondences->isChecked()) {
            this->actionDisplayPointCorrespondences->setChecked(true);
            this->actionDisplayPointCorrespondences->trigger();
        }
        
        if(!actionShowPointCloud->isChecked() && !actionShowSurface->isChecked()) {
            this->actionShowSurface->setChecked(true);
            this->actionShowSurface->trigger();
        }
        
        this->actionShowSurfaceNormals->setEnabled(false);
        this->actionShowPointCloud->setEnabled(actionDisplayPointCorrespondences->isChecked());
        this->actionShowTriangulatedMesh->setEnabled(actionDisplayFaceCorrespondences->isChecked());
    } else if(this->actionPickFaceCorrespondences->isChecked()) {
        if(!this->actionDisplayFaceCorrespondences->isChecked()) {
            this->actionDisplayFaceCorrespondences->setChecked(true);
            this->actionDisplayFaceCorrespondences->trigger();
        }
        
        if(!actionShowTriangulatedMesh->isChecked() && !actionShowSurface->isChecked()) {
            this->actionShowSurface->setChecked(true);
            this->actionShowSurface->trigger();
        }
        
        this->actionShowSurfaceNormals->setEnabled(false);
        this->actionShowPointCloud->setEnabled(actionDisplayPointCorrespondences->isChecked());
        this->actionShowTriangulatedMesh->setEnabled(actionDisplayFaceCorrespondences->isChecked());
    } else {
        this->actionShowSurfaceNormals->setEnabled(true);
        this->actionShowPointCloud->setEnabled(true);
        this->actionShowTriangulatedMesh->setEnabled(true);
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotSetSelectedCurrentShape(QListWidgetItem* current, QListWidgetItem* previous) {
    if(current != nullptr) {
        shared_ptr<Shape> currentShape = ((CustomListWidgetItem<Shape>*) current)->getItem();
        

        scalarBar_->SetLookupTable(currentShape->getLookupTable());
        scalarBar_->SetTitle(currentShape->getName().c_str());
        scalarBar_->Modified();

        if(this->actionTransformShapes->isChecked()) {
            currentShape->setShowBoxWidget(true);
            
            if(previous != nullptr) {
                ((CustomListWidgetItem<Shape>*) previous)->getItem()->setShowBoxWidget(false);
            }
        }
        
        this->qvtkWidget->GetRenderWindow()->Render();
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotClear() {
    clear();
}

///////////////////////////////////////////////////////////////////////////////
// Slots handling VTK
///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotResetCamera() {
    renderer_->ResetCamera();
    qvtkWidget->GetRenderWindow()->Render();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::slotSetSelectedCurrentCorrespondence(QListWidgetItem* current, QListWidgetItem* previous) {
    // only change color if there is a selection
    if(current != nullptr) {
        CustomListWidgetItem<VisualCorrespondence<PointCorrespondence>>* currentItem = dynamic_cast<CustomListWidgetItem<VisualCorrespondence<PointCorrespondence>>*>(current);
        if(currentItem != nullptr) {
            // set current correspondence red
            currentItem->getItem()->setSelected(true);
        } else {
            CustomListWidgetItem<VisualCorrespondence<FaceCorrespondence>>* currentItem = dynamic_cast<CustomListWidgetItem<VisualCorrespondence<FaceCorrespondence>>*>(current);
            // set current correspondence red
            currentItem->getItem()->setSelected(true);
        }
        

        
        // update
        this->qvtkWidget->GetRenderWindow()->Render();
    }
    
    //if there exists a previous selection set previous correspondence to green
    if(previous != nullptr) {
        CustomListWidgetItem<VisualCorrespondence<PointCorrespondence>>* previousItem = dynamic_cast<CustomListWidgetItem<VisualCorrespondence<PointCorrespondence>>*>(previous);
        
        if(previousItem != nullptr) {
            previousItem->getItem()->setSelected(false);
        } else {
            CustomListWidgetItem<VisualCorrespondence<FaceCorrespondence>>* previousItem = dynamic_cast<CustomListWidgetItem<VisualCorrespondence<FaceCorrespondence>>*>(previous);
            previousItem->getItem()->setSelected(false);
        }
        
        
        // update
        this->qvtkWidget->GetRenderWindow()->Render();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Functions handling VTK
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::vtkSetup() {
    this->qvtkWidget->installEventFilter(this);
    
    //Connect qvtk widget with this object
    this->connections_ = vtkSmartPointer<vtkEventQtSlotConnect>::New();
    
    this->connections_->Connect(this->qvtkWidget->GetRenderWindow()->GetInteractor(),
                                vtkCommand::LeftButtonPressEvent,
                                this,
                                SLOT(vtkClickHandler(vtkObject*, unsigned long, void*, void*, vtkCommand*)),
                                nullptr, 1.0);
    this->connections_->Connect(this->qvtkWidget->GetRenderWindow()->GetInteractor(),
                                vtkCommand::RightButtonPressEvent,
                                this,
                                SLOT(vtkClickHandler(vtkObject*, unsigned long, void*, void*, vtkCommand*)),
                                nullptr, 1.0);
    this->connections_->Connect(this->qvtkWidget->GetRenderWindow()->GetInteractor(),
                                vtkCommand::MouseMoveEvent,
                                this,
                                SLOT(vtkMouseMoveHandler(vtkObject*, unsigned long, void*, void*, vtkCommand*)),
                                nullptr, 1.0);
    
    this->connections_->Connect(this->qvtkWidget->GetRenderWindow()->GetInteractor(),
                                vtkCommand::KeyPressEvent,
                                this,
                                SLOT(vtkKeyPressHandler(vtkObject*, unsigned long, void*, void*, vtkCommand*)),
                                nullptr, 1.0);
    
    vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
    lookupTable->SetTableRange(1, 1);
    lookupTable->SetHueRange(0.667, 0.0);
    lookupTable->Build();
    
    scalarBar_ = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBar_->SetLookupTable(lookupTable);
    scalarBar_->SetTitle(" ");
    scalarBar_->SetWidth(0.1);
    scalarBar_->SetHeight(0.9);
    scalarBar_->SetPosition(0.9, 0);
    scalarBar_->SetNumberOfLabels(15);
    scalarBar_->SetVisibility(0);
    
    renderer_ = vtkSmartPointer<vtkRenderer>::New();
    renderer_->AddActor2D(scalarBar_);
    
    this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer_);
    
    vtkSmartPointer<ShapeAnalyzerInteractorStyle> style =
    vtkSmartPointer<ShapeAnalyzerInteractorStyle>::New();
    this->qvtkWidget->GetRenderWindow()->GetInteractor()->SetInteractorStyle( style );
    
    this->pointCorrespondencePicker_.reset(new CorrespondencePicker<PointCorrespondence>(renderer_, [this]()->shared_ptr<PointCorrespondence> { return make_shared<PointCorrespondence>(lastInsertCorrespondenceId_++); }));
    this->faceCorrespondencePicker_.reset(new CorrespondencePicker<FaceCorrespondence>(renderer_, [this]()->shared_ptr<FaceCorrespondence> { return make_shared<FaceCorrespondence>(lastInsertCorrespondenceId_++); }));
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::importShape(vtkAlgorithmOutput* output, string name) {
    // the following will filter the shape for certain properties
    
    // dialog for options
    QDialog* dialog = new QDialog(this, 0);
    Ui_ImportShapeDialog ui;
    ui.setupUi(dialog);
    
    if(dialog->exec() == QDialog::Rejected) {
        return;
    }
    
    vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
    vtkSmartPointer<vtkCleanPolyData> cleanPolyData = vtkSmartPointer<vtkCleanPolyData>::New();
    
    // vtk error observe
    vtkSmartPointer<ErrorObserver> triangleObserver = vtkSmartPointer<ErrorObserver>::New();
    triangleFilter->AddObserver(vtkCommand::ErrorEvent, triangleObserver);
    triangleFilter->AddObserver(vtkCommand::WarningEvent, triangleObserver);
    
    vtkSmartPointer<ErrorObserver> cleanDataObserver = vtkSmartPointer<ErrorObserver>::New();
    cleanPolyData->AddObserver(vtkCommand::ErrorEvent, cleanDataObserver);
    cleanPolyData->AddObserver(vtkCommand::WarningEvent, cleanDataObserver);
    
    // filter to triangulate shape
    if(ui.checkBoxTriangulation->isChecked()) {
        //make sure that all faces are triangles
        triangleFilter->SetInputConnection(output);
        triangleFilter->Update();
        output = triangleFilter->GetOutputPort();
    }
    
    if (triangleObserver->GetError()) {
        showErrorMessage("The file cound not be opended", triangleObserver->GetErrorMessage());
        return;
    }
    if (triangleObserver->GetWarning()) {
        showErrorMessage("There was a warning reading the file", triangleObserver->GetWarningMessage());
    }

    //Remove all isolated points.
    if(ui.checkBoxDegeneratedElements->isChecked()) {
        cleanPolyData->SetInputConnection(output);
        //cleanPolyData->ToleranceIsAbsoluteOn();
        //cleanPolyData->SetAbsoluteTolerance(0.00005);
        cleanPolyData->Update();
        output = cleanPolyData->GetOutputPort();
    }
    
    if (cleanDataObserver->GetError()) {
        showErrorMessage("The file cound not be opended", cleanDataObserver->GetErrorMessage());
        return;
    }
    if (cleanDataObserver->GetWarning()) {
        showErrorMessage("There was a warning reading the file", cleanDataObserver->GetWarningMessage());
    }

    
    //If shape is not connected (This only happens with bad shape data). Find largest connected region and extract it.
    if(ui.checkBoxLargestComponent->isChecked()) {
        vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =
        vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
        
        // vtk error observer
        vtkSmartPointer<ErrorObserver> connectivityObserver = vtkSmartPointer<ErrorObserver>::New();
        connectivityFilter->AddObserver(vtkCommand::ErrorEvent, connectivityObserver);
        connectivityFilter->AddObserver(vtkCommand::WarningEvent, connectivityObserver);
        
        connectivityFilter->SetInputConnection(output);
        connectivityFilter->SetExtractionModeToLargestRegion();
        connectivityFilter->Update();
        output = connectivityFilter->GetOutputPort();
        
        if (connectivityObserver->GetError()) {
            showErrorMessage("The file cound not be opended", connectivityObserver->GetErrorMessage());
            return;
        }
        if (connectivityObserver->GetWarning()) {
            showErrorMessage("There was a warning reading the file", connectivityObserver->GetWarningMessage());
        }
        
        // get vtk actor and add to renderer_
        vtkSmartPointer<vtkPolyDataReader> polyDataReader = (vtkPolyDataReader*) output->GetProducer();
        shared_ptr<Shape> shape = make_shared<Shape>(lastInsertShapeId_, name, polyDataReader->GetOutput(), renderer_);
        addShape(shape);
        
    } else {
        // extract all regions in different shapes
        vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =
        vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
        
        // vtk error observer
        vtkSmartPointer<ErrorObserver> connectivityObserver = vtkSmartPointer<ErrorObserver>::New();
        connectivityFilter->AddObserver(vtkCommand::ErrorEvent, connectivityObserver);
        connectivityFilter->AddObserver(vtkCommand::WarningEvent, connectivityObserver);
        
        connectivityFilter->SetInputConnection(output);
        connectivityFilter->SetExtractionModeToAllRegions();
        connectivityFilter->Update();
        int numberOfRegions = connectivityFilter->GetNumberOfExtractedRegions();
        
        // add all regions separatly
        for (int i = 0; i < numberOfRegions; i++) {
            vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =
            vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
            connectivityFilter->SetInputConnection(output);
            connectivityFilter->SetExtractionModeToSpecifiedRegions();
            connectivityFilter->AddSpecifiedRegion(i);
            connectivityFilter->Update();
            
            if (connectivityObserver->GetError()) {
                showErrorMessage("The file cound not be opended", connectivityObserver->GetErrorMessage());
                return;
            }
            if (connectivityObserver->GetWarning()) {
                showErrorMessage("There was a warning reading the file", connectivityObserver->GetWarningMessage());
            }
            
            // get vtk actor and add to renderer_
            vtkSmartPointer<vtkPolyDataReader> polyDataReader = (vtkPolyDataReader*) connectivityFilter->GetOutputPort()->GetProducer();
            
            shared_ptr<Shape> shape = make_shared<Shape>(lastInsertShapeId_, name + ":cc" + to_string(i), polyDataReader->GetOutput(), renderer_);
            addShape(shape);
        }
    }
    
    if(shapes_.size() == 1) {
        renderer_->ResetCamera();
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::vtkClickHandler(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command) {
    vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
    // Get the location of the click (in window coordinates)
    int* pos =interactor->GetEventPosition();
    
    //Get global position of the click
    int* size = interactor->GetSize();
    QPoint globalPos = qvtkWidget->mapToGlobal(QPoint(pos[0], size[1]-pos[1]));
    
    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance(0.005);
    
    // Pick from this location.
    picker->Pick(pos[0], pos[1], 0, renderer_);
    
    
    // check if pick was valid
    if(picker->GetPointId() != -1 && picker->GetCellId() != -1) {
        
        if(shapes_.containsKey(picker->GetActor())) {
            shared_ptr<Shape> shape = shapes_[picker->GetActor()];
            // depending on whether we want to add face or point correspondences provide picker->GetCellId or picker->GetPointId to vtkShapeClicked method
            vtkShapeClicked(shape, picker->GetPointId(), picker->GetCellId(), globalPos, vtkEvent, command);
        } else {
            if(visualPointCorrespondences_.containsKey(picker->GetActor())) {
                vtkCorrespondenceClicked(visualPointCorrespondences_[picker->GetActor()].get(), picker->GetPointId(), globalPos, vtkEvent, command);
            }
            if(visualFaceCorrespondences_.containsKey(picker->GetActor())) {
                vtkCorrespondenceClicked(visualFaceCorrespondences_[picker->GetActor()].get(), picker->GetPointId(), globalPos, vtkEvent, command);
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::vtkKeyPressHandler(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command) {
    vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
    
    std::string key = interactor->GetKeySym();
    
    if(this->actionPickPointCorrespondences->isChecked()) {
        if(key == "a") {
            // add current seleceted correspondence if more than one shape
            // is involved
            if(pointCorrespondencePicker_->getCounter() > 1) {
                pickCorrespondence();
            }
        } else if(key == "Escape") {
            pointCorrespondencePicker_->clearSelection();
        }
    } else if(this->actionPickFaceCorrespondences->isChecked()) {
        if(key == "a") {
            // add current seleceted correspondence if more than one shape
            // is involved
            if(faceCorrespondencePicker_->getCounter() > 1) {
                pickCorrespondence();
            }
        } else if(key == "Escape") {
            faceCorrespondencePicker_->clearSelection();
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::vtkMouseMoveHandler(vtkObject *caller, unsigned long vtkEvent, void *clientData, void *callData, vtkCommand *command) {
    vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::SafeDownCast(caller);
    // Get the location of the click (in window coordinates)
    int* pos =interactor->GetEventPosition();
    
    if(this->actionPickPointCorrespondences->isChecked()) {
        pointCorrespondencePicker_->onMouseMove(pos[0], pos[1]);
    } else if(this->actionPickFaceCorrespondences->isChecked()) {
        faceCorrespondencePicker_->onMouseMove(pos[0], pos[1]);
    } else {}
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::vtkShapeClicked(shared_ptr<Shape> shape, vtkIdType pointId, vtkIdType faceId, QPoint &pos, unsigned long vtkEvent, vtkCommand *command) {
    if(vtkEvent == vtkCommand::LeftButtonPressEvent && this->actionPickPointCorrespondences->isChecked()) {
        pointCorrespondencePicker_->addShape(shape, pointId);
        
        
        // all shapes are in the current correspondence
        if(shapes_.size() > 1 && pointCorrespondencePicker_->getCounter() == shapes_.size()) {
            pickCorrespondence();
        }
    } else if(vtkEvent == vtkCommand::LeftButtonPressEvent && this->actionPickFaceCorrespondences->isChecked()) {
        faceCorrespondencePicker_->addShape(shape, faceId);
        
        // all shapes are in the current correspondence
        if(shapes_.size() > 1 && faceCorrespondencePicker_->getCounter() == shapes_.size()) {
            pickCorrespondence();
        }
    } else {
        //select item in list if clicked on shape
        for(int i = 0; i < listShapes->count(); i++) {
            if(((CustomListWidgetItem<Shape>*) listShapes->item(i))->getItem() == shape) {
                listShapes->setCurrentRow(i);
                if(vtkEvent == vtkCommand::RightButtonPressEvent && !this->actionTransformShapes->isChecked()) {
                    command->AbortFlagOn();
                    qtShowContextMenuShapes(pos, pointId, faceId);
                }
            }
        }
    }
}



///////////////////////////////////////////////////////////////////////////////
// Functions managing data structures
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::clear() {
    if(this->shapes_.size() > 0) {
        scalarBar_->SetTitle(" ");
    }
    
    // fire event for customTabs
    for(auto& entry : customTabs_) {
        entry.second->onClear();
    }
    
    // qt
    listShapes->disconnect();
    listCorrespondences->disconnect();

    //delete all items from the list.
    listCorrespondences->clear();
    

    //delete all correspondences (not neccessarily all correspondences are in the listWidget)
    for(auto& entry : visualPointCorrespondences_) {
        
        entry.second->removeFromRenderer();
    }
    visualPointCorrespondences_.clear();
    
    for(auto& entry : visualFaceCorrespondences_) {
        
        entry.second->removeFromRenderer();
    }
    visualFaceCorrespondences_.clear();
    
    // delete all correspondence data
    pointCorrespondences_.clear();
    faceCorrespondences_.clear();
    
    
    
    // delete all shapes
    for(auto& entry : shapes_) {
        entry.second->removeFromRenderer();
    }
    shapes_.clear();
    listShapes->clear();
    
    lastInsertShapeId_ = 0;
    lastInsertCorrespondenceId_ = 0;
    
    pointCorrespondencePicker_->clearSelection();
    faceCorrespondencePicker_->clearSelection();
    
    this->qvtkWidget->GetRenderWindow()->Render();
    
    qtConnectListCorrespondences();
    qtConnectListShapes();
    qtUpdateLabelVisibleCorrespondences();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::clearCorrespondences() {
    listCorrespondences->disconnect();
    
    //delete all items from the list if point correspondences are currently in the listWidget. Remove displayed correspondences from renderer
    listCorrespondences->clear();
    
    if(actionDisplayPointCorrespondences->isChecked()) {
        //delete all correspondences (not neccessarily all correspondences are in the listWidget)
        for(auto& entry : visualPointCorrespondences_) {
            entry.second->removeFromRenderer();
        }
        visualPointCorrespondences_.clear();
        
        
        // delete all correspondence data
        pointCorrespondences_.clear();
        
        this->qvtkWidget->GetRenderWindow()->Render();
    } else {
        //delete all correspondences (not neccessarily all correspondences are in the listWidget)
        for(auto& entry : visualFaceCorrespondences_) {
            entry.second->removeFromRenderer();
        }
        visualFaceCorrespondences_.clear();
        
        
        // delete all correspondence data
        faceCorrespondences_.clear();
        
        this->qvtkWidget->GetRenderWindow()->Render();
    }
    
    qtConnectListCorrespondences();
    qtUpdateLabelVisibleCorrespondences();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::deleteCorrespondence(int i) {
    // qt
    listCorrespondences->disconnect();
    
    // correspondence deleting

    CustomListWidgetItem<VisualCorrespondence<PointCorrespondence>>* item = dynamic_cast<CustomListWidgetItem<VisualCorrespondence<PointCorrespondence>>*>(listCorrespondences->item(i));
    if(item != nullptr) {
        
        
        pointCorrespondences_.remove(item->getItem()->getCorrespondence());
        visualPointCorrespondences_.remove(item->getItem()->getActor());

        item->getItem()->removeFromRenderer();
        
        delete item;
    } else {
        CustomListWidgetItem<VisualCorrespondence<FaceCorrespondence>>* item = dynamic_cast<CustomListWidgetItem<VisualCorrespondence<FaceCorrespondence>>*>(listCorrespondences->item(i));
        
        faceCorrespondences_.remove(item->getItem()->getCorrespondence());
        visualFaceCorrespondences_.remove(item->getItem()->getActor());
        
        item->getItem()->removeFromRenderer();
        
        delete item;
    }

    this->qvtkWidget->GetRenderWindow()->Render();
    
    qtConnectListCorrespondences();
    qtUpdateLabelVisibleCorrespondences();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::hideCorrespondences() {
    listCorrespondences->disconnect();

    listCorrespondences->clear();
    
    if(actionDisplayPointCorrespondences->isChecked()) {
        for(auto entry : visualPointCorrespondences_) {
            entry.second->removeFromRenderer();
        }
        
        // Set flag "displayed" to false for each Correspondence object
        for(auto& entry : pointCorrespondences_) {
            entry.second = false;
        }
        
        visualPointCorrespondences_.clear();
    } else {
        for(auto& entry : visualFaceCorrespondences_) {
            entry.second->removeFromRenderer();
        }
        
        // Set flag "displayed" to false for each Correspondence object
        for(auto& entry : faceCorrespondences_) {
            entry.second = false;
        }
        
        visualFaceCorrespondences_.clear();
    }
    this->qvtkWidget->GetRenderWindow()->Render();
    
    
    qtConnectListCorrespondences();
    qtUpdateLabelVisibleCorrespondences();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::hideCorrespondence(int i) {
    // qt
    listCorrespondences->disconnect();
    

    // correspondence deleting
    CustomListWidgetItem<VisualCorrespondence<PointCorrespondence>>* item = dynamic_cast<CustomListWidgetItem<VisualCorrespondence<PointCorrespondence>>*>(listCorrespondences->item(i));
    if(item != nullptr) {
        pointCorrespondences_[item->getItem()->getCorrespondence()] = false;
        visualPointCorrespondences_.remove(item->getItem()->getActor());
        
        item->getItem()->removeFromRenderer();
        delete item;
    } else {
        CustomListWidgetItem<VisualCorrespondence<FaceCorrespondence>>* item = dynamic_cast<CustomListWidgetItem<VisualCorrespondence<FaceCorrespondence>>*>(listCorrespondences->item(i));
        
        faceCorrespondences_[item->getItem()->getCorrespondence()] = false;
        visualFaceCorrespondences_.remove(item->getItem()->getActor());
        
        item->getItem()->removeFromRenderer();
        delete item;
    }
    
    this->qvtkWidget->GetRenderWindow()->Render();
    
    qtConnectListCorrespondences();
    qtUpdateLabelVisibleCorrespondences();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::sampleCorrespondences(unsigned int size) {
    hideCorrespondences();
    if (this->actionDisplayPointCorrespondences->isChecked()) { // point correspondence
        vector<shared_ptr<PointCorrespondence>> sample;
        pointCorrespondences_.getRandomSampleKeys(size, sample);
        
        for(int i = 0; i < sample.size(); i++) {
            
            shared_ptr<VisualCorrespondence<PointCorrespondence>> correspondence = make_shared<VisualCorrespondence<PointCorrespondence>>(renderer_, sample[i]);
            
            // create actor and add to vtk
            correspondence->addToRenderer();
            
            pointCorrespondences_[sample[i]] = true;
            visualPointCorrespondences_.insert(correspondence->getActor(), correspondence);
            
            // add shape to qt list widget
            qtAddListCorrespondencesItem(correspondence);
        }
    } else {
        vector<shared_ptr<FaceCorrespondence>> sample;
        faceCorrespondences_.getRandomSampleKeys(size, sample);
        
        for(int i = 0; i < sample.size(); i++) {
            
            shared_ptr<VisualCorrespondence<FaceCorrespondence>> correspondence = make_shared<VisualCorrespondence<FaceCorrespondence>>(renderer_, sample[i]);
            
            // create actor and add to vtk
            correspondence->addToRenderer();
            
            faceCorrespondences_[sample[i]] = true;
            visualFaceCorrespondences_.insert(correspondence->getActor(), correspondence);
            
            // add shape to qt list widget
            qtAddListCorrespondencesItem(correspondence);
        }
    }
    
    this->qvtkWidget->GetRenderWindow()->Render();
    qtUpdateLabelVisibleCorrespondences();
}


///////////////////////////////////////////////////////////////////////////////
shared_ptr<PointCorrespondence> ShapeAnalyzer::addPointCorrespondence(const vector<pair<shared_ptr<Shape>, vtkIdType>>& correspondence) {
    shared_ptr<PointCorrespondence> c = make_shared<PointCorrespondence>(lastInsertCorrespondenceId_++);
    for(auto& p : correspondence) {
        c->addShape(p.first, p.second);
    }
    pointCorrespondences_.insert(dynamic_pointer_cast<PointCorrespondence>(c), false);

    qtUpdateLabelVisibleCorrespondences();
    
    return c;
}


///////////////////////////////////////////////////////////////////////////////
shared_ptr<FaceCorrespondence> ShapeAnalyzer::addFaceCorrespondence(const vector<pair<shared_ptr<Shape>, vtkIdType>>& correspondence) {
    shared_ptr<FaceCorrespondence> c = make_shared<FaceCorrespondence>(lastInsertCorrespondenceId_++);
    for(auto& p : correspondence) {
        c->addShape(p.first, p.second);
    }
    faceCorrespondences_.insert(dynamic_pointer_cast<FaceCorrespondence>(c), false);
    
    qtUpdateLabelVisibleCorrespondences();
    
    return c;
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::pickCorrespondence() {
    if(actionPickPointCorrespondences->isChecked()) {
        shared_ptr<VisualCorrespondence<PointCorrespondence>> pointCorrespondence;
        if(pointCorrespondencePicker_->pick(pointCorrespondence)) {
            pointCorrespondences_.insert(pointCorrespondence->getCorrespondence(), true);
            visualPointCorrespondences_.insert(pointCorrespondence->getActor(), pointCorrespondence);
            
            // add shape to qt list widget
            qtAddListCorrespondencesItem(pointCorrespondence);
        }
    } else {
        shared_ptr<VisualCorrespondence<FaceCorrespondence>> faceCorrespondence;
        if(faceCorrespondencePicker_->pick(faceCorrespondence)) {
            faceCorrespondences_.insert(faceCorrespondence->getCorrespondence(), true);
            visualFaceCorrespondences_.insert(faceCorrespondence->getActor(), faceCorrespondence);
            
            // add shape to qt list widget
            qtAddListCorrespondencesItem(faceCorrespondence);
        }
    }


    qtUpdateLabelVisibleCorrespondences();
}
                   
                   
///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::deleteFace(Shape* shape, vtkIdType faceId) {
    vtkSmartPointer<vtkPolyData> polyData = shape->getPolyData();
    //if(faceId > polyData->GetNumberOfCells()) { return; }
    
    polyData->DeleteCell(faceId);
    
    this->qvtkWidget->GetRenderWindow()->Render();
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::deleteShape(int i) {
    listCorrespondences->disconnect();
    listShapes->disconnect();
    
    // reset scalar bar. It displays the shapes coloring since the shape is selected when it is deleted.
    vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
    lookupTable->SetTableRange(1.0, 1.0);
    lookupTable->SetHueRange(0.667, 0.0);
    lookupTable->Build();
    scalarBar_->SetLookupTable(lookupTable);
    scalarBar_->SetTitle(" ");
    scalarBar_->Modified();
    
    
    // qt
    CustomListWidgetItem<Shape> *item = (CustomListWidgetItem<Shape>*) this->listShapes->item(i);
    shared_ptr<Shape> shape = item->getItem();
    

    // fire event for shapesTabs
    for(auto& entry : customTabs_) {
        entry.second->onShapeDelete(shape.get());
    }
    
    
    pointCorrespondencePicker_->clearSelection();
    faceCorrespondencePicker_->clearSelection();
    
    // remove shape from list
    delete item;
    
    
    // delete all items in the list
    // iterate through the correspondence list backwards beginning from the end of the list since delete listItem decreases list->count() and to make sure that we get all correspondences that we want to delete.
    for(int j = listCorrespondences->count()-1; j > -1; j--) {
        
        shared_ptr<Correspondence> correspondence;
        CustomListWidgetItem<VisualCorrespondence<PointCorrespondence>>* item = dynamic_cast<CustomListWidgetItem<VisualCorrespondence<PointCorrespondence>>*> (listCorrespondences->item(j));
        if(item != nullptr) {
            correspondence = item->getItem()->getCorrespondence();
        } else {
            CustomListWidgetItem<VisualCorrespondence<FaceCorrespondence>>* item = dynamic_cast<CustomListWidgetItem<VisualCorrespondence<FaceCorrespondence>>*> (listCorrespondences->item(j));
            correspondence = item->getItem()->getCorrespondence();
        }
        if(correspondence->getShapes().size() == 2) {
            //check whether one of the shapes of correspondence equals our shape that we want to delete
            if(shape == correspondence->getShapes().at(0) || shape == correspondence->getShapes().at(1)) {
                //destroy widgetItem object
                delete listCorrespondences->item(j);
            }
        }
    }
    
    // delete all visual correspondences belonging to the shape
    for(auto it = visualPointCorrespondences_.begin(); it != visualPointCorrespondences_.end();) {

        if(it->second->getCorrespondence()->size() == 2 && (it->second->getCorrespondence()->getShapes().at(0) == shape || it->second->getCorrespondence()->getShapes().at(1) == shape)) {
            
            it->second->removeFromRenderer();
            it = visualPointCorrespondences_.getEntries().erase(it);
        } else {
            if(it->second->getCorrespondence()->size() > 2) {
                it->second->removeShape(shape.get());
            }
            ++it;
        }
    }
    for(auto it = visualFaceCorrespondences_.begin(); it != visualFaceCorrespondences_.end();) {
        
        if(it->second->getCorrespondence()->size() == 2 && (it->second->getCorrespondence()->getShapes().at(0) == shape || it->second->getCorrespondence()->getShapes().at(1) == shape)) {
            
            it->second->removeFromRenderer();
            it = visualFaceCorrespondences_.getEntries().erase(it);
        } else {
            if(it->second->getCorrespondence()->size() > 2) {
                it->second->removeShape(shape.get());
            }
            ++it;
        }
    }
    
    // delete all correspondences that belong to the shape
    for(auto it = pointCorrespondences_.begin(); it != pointCorrespondences_.end();) {
        
        shared_ptr<PointCorrespondence> correspondence = it->first;
        //check whether one of the shapes of correspondence equals our shape that we want to delete
        
        if(correspondence->size() == 2 && (correspondence->getShapes().at(0) == shape || correspondence->getShapes().at(1) == shape)) {
            it = pointCorrespondences_.getEntries().erase(it);
        } else {
            if(correspondence->size() > 2) {
                correspondence->removeShape(shape.get());
            }
            ++it;
        }
    }
    for(auto it = faceCorrespondences_.begin(); it != faceCorrespondences_.end();) {
        
        shared_ptr<FaceCorrespondence> correspondence = it->first;
        //check whether one of the shapes of correspondence equals our shape that we want to delete
        
        if(correspondence->size() == 2 && (correspondence->getShapes().at(0) == shape || correspondence->getShapes().at(1) == shape)) {
            it = faceCorrespondences_.getEntries().erase(it);
        } else {
            if(correspondence->size() > 2) {
                correspondence->removeShape(shape.get());
            }
            ++it;
        }
    }

    
    // delete shape
    shape->removeFromRenderer();
    shapes_.remove(shape->getActor());
    
    this->qvtkWidget->GetRenderWindow()->Render();
    
    qtConnectListCorrespondences();
    qtConnectListShapes();
    qtUpdateLabelVisibleCorrespondences();
}


///////////////////////////////////////////////////////////////////////////////
shared_ptr<Shape> ShapeAnalyzer::addShape(string name, vtkSmartPointer<vtkPolyData> polyData) {
    shared_ptr<Shape> shape = make_shared<Shape>(lastInsertShapeId_, name, polyData, renderer_);
    addShape(shape);
    
    return shape;
}


///////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::addShape(shared_ptr<Shape> shape) {
    if(this->actionShowSurface->isChecked()) {
        shape->setVisualRepresentation(Shape::VisualRepresentation::MeshSurface);
    } else if(this->actionShowSurfaceNormals->isChecked()) {
        shape->setVisualRepresentation(Shape::VisualRepresentation::InterpolatedNormals);
    } else if(this->actionShowTriangulatedMesh->isChecked()) {
        shape->setVisualRepresentation(Shape::VisualRepresentation::Mesh);
    } else if(this->actionShowPointCloud->isChecked()) {
        shape->setVisualRepresentation(Shape::VisualRepresentation::PointCloud);
    } else {
        ;
    }
    

    vtkSmartPointer<vtkBoxWidgetCallback> callback = vtkSmartPointer<vtkBoxWidgetCallback>::New();
    callback->sa = this;
    callback->shape = shape.get();
    
    shape->addObserver(callback);

    shapes_.insert(shape->getActor(), shape);
    
    // fire event for shapesTabs
    for(auto& entry : customTabs_) {
        entry.second->onShapeAdd(shape.get());
    }
    
    // add shape to qt list widget
    lastInsertShapeId_++;
    
    CustomListWidgetItem<Shape>* item = new CustomListWidgetItem<Shape>(QString(shape->getName().c_str()), shape);
    listShapes->addItem(item);
    
    //make sure that there always is exactly one item selected if there exists at least one item
    listShapes->setCurrentRow(listShapes->count()-1);
    
    
    qvtkWidget->GetRenderWindow()->Render();
}


//////////////////////////////////////////////////////////////////////////////
void ShapeAnalyzer::showErrorMessage(string description, string error) {
    QMessageBox::warning(this, "Error", QString::fromStdString(description + string(": ") + error));
}

