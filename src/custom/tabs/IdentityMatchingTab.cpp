#include "IdentityMatchingTab.h"

///////////////////////////////////////////////////////////////////////////////
custom::tabs::IdentityMatchingTab::~IdentityMatchingTab() {
}


///////////////////////////////////////////////////////////////////////////////
custom::tabs::IdentityMatchingTab::IdentityMatchingTab(
                            const HashMap<vtkActor*, shared_ptr<Shape>>& shapes,
                            const HashMap<shared_ptr<PointCorrespondence>, bool>& pointCorrespondences,
                            const HashMap<shared_ptr<FaceCorrespondence>, bool>& faceCorrespondences,
                            ShapeAnalyzerInterface* shapeAnalyzer)
        :   QWidget(dynamic_cast<QWidget*>(shapeAnalyzer), 0),
            CustomTab(shapes, pointCorrespondences, faceCorrespondences, shapeAnalyzer)
{
    this->setupUi(this);
    
    QStringList labels = getShapeIdentifierList();
    
    if(shapes_.size() < 2) {
        buttonMatch->setEnabled(false);
    }
    
    comboBoxShape1->insertItems(0, labels);
    comboBoxShape2->insertItems(0, labels);
    
    connect(this->buttonMatch,                      SIGNAL(released()),
            this,                                   SLOT(slotMatch()));
    
    connect(this->buttonGroupMatch,                 SIGNAL(buttonClicked(int)),
            this,                                   SLOT(slotToggleMode()));
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::IdentityMatchingTab::slotToggleMode() {
    if(radioButtonBinaryCorrespondences->isChecked()) {
        labelShape1->setEnabled(true);
        comboBoxShape1->setEnabled(true);
        labelShape2->setEnabled(true);
        comboBoxShape2->setEnabled(true);
    } else {
        labelShape1->setEnabled(false);
        comboBoxShape1->setEnabled(false);
        labelShape2->setEnabled(false);
        comboBoxShape2->setEnabled(false);
        if(comboBoxShape1->count() >= 2) {
            this->buttonMatch->setEnabled(true);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void custom::tabs::IdentityMatchingTab::slotMatch() {
    int step = ceil(100.0 / spinBoxPercentage->value());
    int type = comboBoxCorrespondenceType->currentIndex();
    
    if(radioButtonBinaryCorrespondences->isChecked()) {
        vtkIdType sid1 = getIdFromIdentifier(comboBoxShape1->currentText());
        vtkIdType sid2 = getIdFromIdentifier(comboBoxShape2->currentText());
        
        
        shared_ptr<Shape> shape1 = nullptr;
        shared_ptr<Shape> shape2 = nullptr;
        
        for(auto entry : shapes_) {
            if(sid1 == entry.second->getId()) {
                shape1 = entry.second;
            }
            
            if(sid2 == entry.second->getId()) {
                shape2 = entry.second;
            }
        }
        if(shape1 == shape2) {
            QMessageBox::warning(dynamic_cast<QWidget*>(shapeAnalyzer_),
                                 "Error", "The two selected shapes have to be different.");
            return;
        }
        
        if(type == 0) {
            for(int i = 0; i < min(shape1->getPolyData()->GetNumberOfPoints(), shape2->getPolyData()->GetNumberOfPoints()); i+=step) {
                vector<pair<shared_ptr<Shape>, vtkIdType>> correspondence;
                correspondence.push_back(make_pair(shape1, i));
                correspondence.push_back(make_pair(shape2, i));
                
                shapeAnalyzer_->addPointCorrespondence(correspondence);
            }
        } else {
            for(int i = 0; i < min(shape1->getPolyData()->GetNumberOfCells(), shape2->getPolyData()->GetNumberOfCells()); i+=step) {
                vector<pair<shared_ptr<Shape>, vtkIdType>> correspondence;
                correspondence.push_back(make_pair(shape1, i));
                correspondence.push_back(make_pair(shape2, i));
                
                shapeAnalyzer_->addFaceCorrespondence(correspondence);
            }
        }
    } else {
        int limit;
        if(type == 0) {
            limit = shapes_.begin()->second->getPolyData()->GetNumberOfPoints();
            for(auto s : shapes_) {
                if(s.second->getPolyData()->GetNumberOfPoints() < limit) {
                    limit = s.second->getPolyData()->GetNumberOfPoints();
                }
            }
        } else {
            limit = shapes_.begin()->second->getPolyData()->GetNumberOfCells();
            for(auto s : shapes_) {
                if(s.second->getPolyData()->GetNumberOfCells() < limit) {
                    limit = s.second->getPolyData()->GetNumberOfCells();
                }
            }
        }
        
        for(int i = 0; i < limit; i+=step) {
            vector<pair<shared_ptr<Shape>, vtkIdType>> correspondence;
            for(auto s : shapes_) {
                correspondence.push_back(make_pair(s.second, i));
            }
            
            if(type == 0) {
                shapeAnalyzer_->addPointCorrespondence(correspondence);
            } else {
                shapeAnalyzer_->addFaceCorrespondence(correspondence);
            }
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::IdentityMatchingTab::onShapeAdd(Shape* shape) {
    QString label = getShapeIdentifier(shape);
    comboBoxShape1->insertItem(0, label);
    comboBoxShape2->insertItem(0, label);
    
    if(shapes_.size() >= 2) {
        this->buttonMatch->setEnabled(true);
    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::IdentityMatchingTab::onShapeDelete(Shape* shape) {
    for(int i = comboBoxShape1->count()-1; i >= 0; i--) {
        if(getIdFromIdentifier(comboBoxShape1->itemText(i)) == shape->getId()) {
            comboBoxShape1->removeItem(i);
            break;
        }
    }
    
    for(int i = comboBoxShape2->count()-1; i >= 0; i--) {
        if(getIdFromIdentifier(comboBoxShape2->itemText(i)) == shape->getId()) {
            comboBoxShape2->removeItem(i);
            break;
        }
    }
    
    if(comboBoxShape1->count() == 0) {
        this->buttonMatch->setEnabled(false);
    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::IdentityMatchingTab::onShapeEdit(Shape* shape) {
    QString label = getShapeIdentifier(shape);
    
    for(int i = comboBoxShape1->count()-1; i >= 0; i--) {
        if(getIdFromIdentifier(comboBoxShape1->itemText(i)) == shape->getId()) {
            comboBoxShape1->setItemText(i, label);
            break;
        }
    }
    
    for(int i = comboBoxShape2->count()-1; i >= 0; i--) {
        if(getIdFromIdentifier(comboBoxShape2->itemText(i)) == shape->getId()) {
            comboBoxShape2->setItemText(i, label);
            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::IdentityMatchingTab::onClear() {
    this->comboBoxShape1->clear();
    this->comboBoxShape2->clear();

    this->buttonMatch->setEnabled(false);
}
