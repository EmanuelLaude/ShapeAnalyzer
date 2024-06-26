#include "MeshCheckTab.h"

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
custom::tabs::MeshCheckTab::MeshCheckTab(
                        const HashMap<vtkActor*, shared_ptr<Shape>>& shapes,
                        const HashMap<shared_ptr<PointCorrespondence>, bool>& pointCorrespondences,
                        const HashMap<shared_ptr<FaceCorrespondence>, bool>& faceCorrespondences,
                        ShapeAnalyzerInterface* shapeAnalyzer)
    :   QWidget(dynamic_cast<QWidget*>(shapeAnalyzer), 0),
        CustomTab(shapes, pointCorrespondences, faceCorrespondences, shapeAnalyzer)
{
    this->setupUi(this);
    
    setUpComboBox();
    
    connect(this->buttonCheckMesh,          SIGNAL(released()),
            this,                           SLOT(slotCheckMesh()));
    connect(this->comboBoxMesh,             SIGNAL(currentIndexChanged(int)),
            this->textBrowserOutput,        SLOT(clear()));
}


///////////////////////////////////////////////////////////////////////////////
// Slots
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::MeshCheckTab::slotCheckMesh() {
    // clear previous output
    this->textBrowserOutput->clear();
    
    // find correct shape
    shared_ptr<Shape> shape;
    bool shapeFound = false;
    for (auto entry : shapes_) {
        if(entry.second->getId() == comboBoxMesh->currentText().split(':')[0].toInt()) {
            shape = entry.second;
            shapeFound = true;
            break;
        }
    }
    
    // return if no shape was found
    if(!shapeFound)
        return;
    
    // run all selected checks
    MeshChecker check(shape);
    
    // triangulation check
    if (checkBoxNonTriangles->isChecked()) {
        vector<pair<vtkIdType, vtkIdType> > nonTriangles;
        bool notTriangulated = check.checkTriangulation(&nonTriangles);
        
        if(notTriangulated) {
            textBrowserOutput->insertPlainText(QString::fromStdString("- Non-Triangles in form (cell-id, #vertices): "));
            // list of all border edges
            for (auto nonTriangle : nonTriangles) {
                string tupel = "(";
                tupel.append(to_string(nonTriangle.first));
                tupel.append(", ");
                tupel.append(to_string(nonTriangle.second));
                tupel.append(") ");
                textBrowserOutput->insertPlainText(QString::fromStdString(tupel));
            }
            textBrowserOutput->insertHtml(QString::fromStdString("<br />"));
        } else {
            textBrowserOutput->insertPlainText(QString::fromStdString("- All cells are triangles."));
            textBrowserOutput->insertHtml(QString::fromStdString("<br />"));
        }
    }
    
    // number of regions
    if (checkBoxConnectivity->isChecked()) {
        int numberRegions = check.checkNumberOfRegions();
        string regions = "- Number of connected Regions: ";
        regions.append(to_string(numberRegions));
        textBrowserOutput->insertPlainText(QString::fromStdString(regions));
        textBrowserOutput->insertHtml(QString::fromStdString("<br />"));
    }
    
    
    // orientation checking
    if (checkBoxOrientation->isChecked()) {
        vector<pair<vtkIdType, vtkIdType> > unorientedEdges;
        bool isUnoriented = check.checkOrientation(&unorientedEdges);
        
        // output
        if(isUnoriented) {
            textBrowserOutput->insertPlainText(QString::fromStdString("- Inconsistent orientation at: "));
            // list of all border edges
            for (auto e : unorientedEdges) {
                string tupel = "(";
                tupel.append(to_string(e.first));
                tupel.append(", ");
                tupel.append(to_string(e.second));
                tupel.append(") ");
                textBrowserOutput->insertPlainText(QString::fromStdString(tupel));
                
            }
            textBrowserOutput->insertHtml(QString::fromStdString("<br />"));
        } else {
            textBrowserOutput->insertPlainText(QString::fromStdString("- Is consistently oriented."));
            textBrowserOutput->insertHtml(QString::fromStdString("<br />"));
        }
    }
    
    
    // border checking
    if (checkBoxBorders->isChecked()) {
        vector<pair<vtkIdType, vtkIdType> > borders;
        bool hasBorders = check.checkForBorders(&borders);
        
        // output
        if(hasBorders) {
            textBrowserOutput->insertPlainText(QString::fromStdString("- Has Borders: "));
            // list of all border edges
            for (auto b : borders) {
                string tupel = "(";
                tupel.append(to_string(b.first));
                tupel.append(", ");
                tupel.append(to_string(b.second));
                tupel.append(") ");
                textBrowserOutput->insertPlainText(QString::fromStdString(tupel));
            }
            textBrowserOutput->insertHtml(QString::fromStdString("<br />"));
        } else {
            textBrowserOutput->insertPlainText(QString::fromStdString("- No Borders."));
            textBrowserOutput->insertHtml(QString::fromStdString("<br />"));
        }
    }
    
    // orientation checking
    if (checkBoxInconsistency->isChecked()) {
        vector<pair<vtkIdType, vtkIdType> > inconsistentEdges;
        bool isInconsistent = check.checkInconsistency(&inconsistentEdges);
        
        // output
        if(isInconsistent) {
            textBrowserOutput->insertPlainText(QString::fromStdString("- Half-edge structure inconsistent at: "));
            // list of all border edges
            for (auto e : inconsistentEdges) {
                string tupel = "(";
                tupel.append(to_string(e.first));
                tupel.append(", ");
                tupel.append(to_string(e.second));
                tupel.append(") ");
                textBrowserOutput->insertPlainText(QString::fromStdString(tupel));
                
            }
            textBrowserOutput->insertHtml(QString::fromStdString("<br />"));
        } else {
            textBrowserOutput->insertPlainText(QString::fromStdString("- Half-edge structure is consistent."));
            textBrowserOutput->insertHtml(QString::fromStdString("<br />"));
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Public CustomTab Functions
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::MeshCheckTab::onShapeDelete(Shape* shape) {
    for(int i = comboBoxMesh->count()-1; i >= 0; i--) {
        // check if items name matches the on in the combo box, if yes delete
        if(getIdFromIdentifier(comboBoxMesh->itemText(i)) == shape->getId()) {
            // clear grid, if the deleted shape was the reference shape
            if (i == comboBoxMesh->currentIndex()) {
                textBrowserOutput->clear();
            }
            comboBoxMesh->removeItem(i);
            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::MeshCheckTab::onShapeAdd(Shape* shape) {
    QString label = getShapeIdentifier(shape);
    comboBoxMesh->addItem(label);
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::MeshCheckTab::onShapeEdit(Shape* shape) {
    QString label = getShapeIdentifier(shape);
    
    for(int i = comboBoxMesh->count()-1; i >= 0; i--) {
        if(getIdFromIdentifier(comboBoxMesh->itemText(i)) == shape->getId()) {
            comboBoxMesh->setItemText(i, label);
            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::MeshCheckTab::onClear() {
    this->comboBoxMesh->clear();
    this->textBrowserOutput->clear();
}


///////////////////////////////////////////////////////////////////////////////
// Private Functions
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::MeshCheckTab::setUpComboBox() {
    this->comboBoxMesh->clear();
    
    this->comboBoxMesh->insertItem(0, QString(tr(" ")));
    
    QStringList labels = getShapeIdentifierList();
    labels.sort();
    this->comboBoxMesh->insertItems(1, labels);

}

