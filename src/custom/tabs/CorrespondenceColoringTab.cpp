#include "CorrespondenceColoringTab.h"

///////////////////////////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
custom::tabs::CorrespondenceColoringTab::CorrespondenceColoringTab(
                                                     const HashMap<vtkActor*, shared_ptr<Shape>>& shapes,
                                                     const HashMap<shared_ptr<PointCorrespondence>, bool>& pointCorrespondences,
                                                     const HashMap<shared_ptr<FaceCorrespondence>, bool>& faceCorrespondences, ShapeAnalyzerInterface* shapeAnalyzer)
: QWidget(dynamic_cast<QWidget*>(shapeAnalyzer), 0), CustomTab(shapes, pointCorrespondences, faceCorrespondences, shapeAnalyzer)
{
    this->setupUi(this);
    

    QStringList labels = getShapeIdentifierList();
    labels.sort();
    this->comboBoxReference->insertItems(0, labels);
    
    connect(this->buttonColor,         SIGNAL(released()),
            this,                      SLOT(slotColorCorrespondences()));
    
    connect(this->buttonClearColoring, SIGNAL(released()),
            this,                      SLOT(slotClearColoring()));
    
    connect(this->radioButtonFaceCorrespondences,   SIGNAL(toggled(bool)),
            this,                                   SLOT(slotCorrespondencesToggled(bool)));
    connect(this->radioButtonPointCorrespondences,  SIGNAL(toggled(bool)),
            this,                                   SLOT(slotCorrespondencesToggled(bool)));
}


///////////////////////////////////////////////////////////////////////////////
// Slots
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::CorrespondenceColoringTab::slotColorCorrespondences() {
    shared_ptr<Shape> reference = nullptr;
    
    // get Shape corresponding with the label
    reference = getShapeFromIdentifier(comboBoxReference->currentText());
    
    // proceed if shape was found
    if (reference != nullptr) {
        CorrespondenceColoring cc = CorrespondenceColoring(shapes_,
                                                           pointCorrespondences_,
                                                           faceCorrespondences_,
                                                           reference);
        
        shared_ptr<vector<pair<vtkIdType, double>>> matched = make_shared<vector<pair<vtkIdType, double>>>();
        shared_ptr<vector<pair<vtkIdType, double>>> multiple = make_shared<vector<pair<vtkIdType, double>>>();
        
        if(radioButtonPointCorrespondences->isChecked()) {
            cc.showPointCorrespondences(matched, multiple);
            
            clearGrid();
            
            // fill in percentage of matched points
            int index = 1;
            for (auto entry : shapes_) {
                if (entry.second != reference) {
                    // shape name
                    QString label = QString::fromStdString(entry.second->getName());
                    gridLayout->addWidget(new QLabel(label), index, 0);
                    
                    // % matched
                    for (int i = 0; i < matched->size(); i++) {
                        if (matched->at(i).first == entry.second->getId()) {
                            QString labelMatched = QString::fromStdString(std::to_string(matched->at(i).second));
                            gridLayout->addWidget(new QLabel(labelMatched), index, 1);
                            break;
                        }
                    }
                    
                    // % multiple
                    for (int i = 0; i < multiple->size(); i++) {
                        if (multiple->at(i).first == entry.second->getId()) {
                            QString labelMultiple = QString::fromStdString(std::to_string(multiple->at(i).second));
                            gridLayout->addWidget(new QLabel(labelMultiple), index, 2);
                            break;
                        }
                    }
                    
                    index++;
                }
            }
            
        } else {
            cc.showFaceCorrespondences(matched, multiple);
            
            clearGrid();
            
            // fill in percentage of matched points
            int index = 1;
            for (auto entry : shapes_) {
                if (entry.second != reference) {
                    // shape name
                    QString label = QString::fromStdString(entry.second->getName());
                    gridLayout->addWidget(new QLabel(label), index, 0);
                    
                    // % matched
                    for (int i = 0; i < matched->size(); i++) {
                        if (matched->at(i).first == entry.second->getId()) {
                            QString labelMatched = QString::fromStdString(std::to_string(matched->at(i).second));
                            gridLayout->addWidget(new QLabel(labelMatched), index, 1);
                            break;
                        }
                    }
                    
                    // % multiple
                    for (int i = 0; i < multiple->size(); i++) {
                        if (multiple->at(i).first == entry.second->getId()) {
                            QString labelMultiple = QString::fromStdString(std::to_string(multiple->at(i).second));
                            gridLayout->addWidget(new QLabel(labelMultiple), index, 2);
                            break;
                        }
                    }
                    
                    index++;
                }
            }
        }

    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::CorrespondenceColoringTab::slotClearColoring() {
    for (auto entry : shapes_) {
        entry.second->clearColoring();
    }
    clearGrid();
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::CorrespondenceColoringTab::slotCorrespondencesToggled(bool b) {
    if(b) slotClearColoring();
}


///////////////////////////////////////////////////////////////////////////////
// Public CustomTab Functions
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::CorrespondenceColoringTab::onShapeDelete(Shape *shape) {
    for(int i = comboBoxReference->count()-1; i >= 0; i--) {
        // check if items name matches the on in the combo box, if yes delete
        if(getIdFromIdentifier(comboBoxReference->itemText(i)) == shape->getId()) {
            comboBoxReference->removeItem(i);
            // clear grid, if the deleted shape was the reference shape
            if (i == comboBoxReference->currentIndex()) {
                clearGrid();
            }
            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::CorrespondenceColoringTab::onShapeAdd(Shape *shape) {
    QString label = getShapeIdentifier(shape);
    comboBoxReference->addItem(label);
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::CorrespondenceColoringTab::onShapeEdit(Shape *shape) {
    QString label = getShapeIdentifier(shape);
    
    for(int i = comboBoxReference->count()-1; i >= 0; i--) {
        if(getIdFromIdentifier(comboBoxReference->itemText(i)) == shape->getId()) {
            comboBoxReference->setItemText(i, label);
            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::CorrespondenceColoringTab::onClear() {
    comboBoxReference->clear();
    clearGrid();
}


///////////////////////////////////////////////////////////////////////////////
// Private Functions
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::CorrespondenceColoringTab::clearGrid() {
    for (int i = 1; i < gridLayout->rowCount(); i++) {
        for (int j = 0; j < gridLayout->columnCount(); j++) {
            if(gridLayout->itemAtPosition(i, j) != nullptr)
                delete gridLayout->itemAtPosition(i, j)->widget();
        }
    }
}
