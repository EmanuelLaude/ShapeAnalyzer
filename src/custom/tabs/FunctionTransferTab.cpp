#include "FunctionTransferTab.h"

///////////////////////////////////////////////////////////////////////////////
custom::tabs::FunctionTransferTab::~FunctionTransferTab() {
}


///////////////////////////////////////////////////////////////////////////////
custom::tabs::FunctionTransferTab::FunctionTransferTab(
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
        buttonTransfer->setEnabled(false);
    }
    
    comboBoxSourceShape->insertItems(0, labels);
    comboBoxTargetShape->insertItems(0, labels);
    
    connect(this->buttonTransfer,                   SIGNAL(released()),
            this,                                   SLOT(slotTransfer()));
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::FunctionTransferTab::slotTransfer() {
    this->textBrowserLog->clear();
    
    vtkIdType sid = getIdFromIdentifier(comboBoxSourceShape->currentText());
    vtkIdType tid = getIdFromIdentifier(comboBoxTargetShape->currentText());

    
    shared_ptr<Shape> source = nullptr;
    shared_ptr<Shape> target = nullptr;
    for(auto entry : shapes_) {
        if(sid == entry.second->getId()) {
            source = entry.second;
        }
        
        if(tid == entry.second->getId()) {
            target = entry.second;
        }
    }
    if(source == target) {
        QMessageBox::warning(dynamic_cast<QWidget*>(shapeAnalyzer_), "Error", "The shapes source and target have to be different.");
        return;
    }
    

    
    if(source->getColoring() == nullptr || (source->getColoring()->type != Shape::Coloring::Type::PointScalar && source->getColoring()->type != Shape::Coloring::Type::PointSegmentation)) {
        QMessageBox::warning(dynamic_cast<QWidget*>(shapeAnalyzer_), "Error", "Shape \"" + QString(source->getName().c_str()) + "\" is neither colored with a scalar point map nor a segmentation. Compute scalar point map or a segmentation first.");
        return;
    }

    try {
        // initialize lists of corresponding contraints on both shapes. Ordering represents correspondence of contraints. I.e. c1[5] on shape1 corresponds to c2[5] on shape2.
        // corresponds to contraints on shape1
        vector<vtkSmartPointer<vtkDoubleArray>> constraintsSource;
        
        // corresponds to contraints on shape2
        vector<vtkSmartPointer<vtkDoubleArray>> constraintsTarget;

        int numberOfEigenfunctions = max(spinBoxNumberOfEigenfunctions->value(), max(spinBoxWaveKernelSignatureNumberOfEigenfunctions->value(), spinBoxHeatBumpsNumberOfEigenfunctions->value()));


        if(spinBoxWaveKernelSignatureComponentsTo->value() >= spinBoxWaveKernelSignatureComponentsTotalNumber->value()) {
            QMessageBox::warning(dynamic_cast<QWidget*>(shapeAnalyzer_),
                                 "Error", QString::fromStdString("Wave Kernel Signature \"To\" has to be smaller than total number of WKS components (= " + to_string(spinBoxWaveKernelSignatureComponentsTotalNumber->value()) + ")"));
            return;
        }
        

        log("Compute Laplacian of source... ");
        auto laplacianSource = make_shared<laplaceBeltrami::PetscFEMLaplaceBeltramiOperator>(source, numberOfEigenfunctions);
        log("done.\n");
        
        log("Compute Laplacian of target... ");
        auto laplacianTarget = make_shared<laplaceBeltrami::PetscFEMLaplaceBeltramiOperator>(target, numberOfEigenfunctions);
        log("done.\n");

        
        
        // Compute landmark correspondences using all available correspondences between shape1 and shape2 and geodesic metric
        // Compute heat diffusion snapshots using the corresponding points.

        vector<pair<int, string>> landmarkCorrespondences;
        
        shared_ptr<metric::GeodesicMetric> metricSource;
        shared_ptr<metric::GeodesicMetric> metricTarget;
        if(checkBoxGeodesicDistanceFunctions->isChecked()) {
            metricSource = make_shared<metric::GeodesicMetric>(source);
            metricTarget = make_shared<metric::GeodesicMetric>(target);
        }
        
        for(auto entry : pointCorrespondences_) {
            shared_ptr<PointCorrespondence> corr = entry.first;
            
            // make sure that correspondence contains both source and target at the same time (dealing with multi-correspondences)
            // if yes do nothing. if no jump to next correspondence
            int count = 0;
            for(int i = 0; i < corr->size(); i++) {
                shared_ptr<Shape> shape = corr->getShapes().at(i);
                if(shape == source) {
                    count++;
                }
                if(shape == target) {
                    count++;
                }
                
            }
            if(count != 2) {
                continue;
            }
            
            
            for(int i = 0; i < corr->getShapes().size(); i++) {
                if(corr->getShapes().at(i) == source) {
                    // Heat diffusion
                    if(groupBoxHeatBumps->isChecked()) {
                        vtkSmartPointer<vtkDoubleArray> u0s = vtkSmartPointer<vtkDoubleArray>::New();
                        u0s->SetNumberOfValues(source->getPolyData()->GetNumberOfPoints());
                        for(vtkIdType j = 0; j < source->getPolyData()->GetNumberOfPoints(); j++) {
                            u0s->SetValue(j, 0.0);
                        }
                        u0s->SetValue(corr->getCorrespondingIds().at(i), doubleSpinBoxHeatBumpsInitialValue->value());
                        
                        log("Add Landmark Correspondence Heat Bump Constraints (CID = " + to_string(corr->getId()) + ")... ");
                        PetscHeatDiffusion hdSource(source, laplacianSource, u0s);
                        for(double t = doubleSpinBoxHeatBumpsFrom->value(); t <= doubleSpinBoxHeatBumpsTo->value(); t+=doubleSpinBoxHeatBumpsStep->value()) {
                            vtkSmartPointer<vtkDoubleArray> normHeat = vtkSmartPointer<vtkDoubleArray>::New();
                            normalize(hdSource.getHeat(t), normHeat, 1.0);
                            constraintsSource.push_back(normHeat);
                            landmarkCorrespondences.push_back(make_pair<int, string>(corr->getId(), "Heat Bump at t = " + to_string(t)));
                        }
                        log("done\n");
                    }
                        
                    // distance functions
                    if(checkBoxGeodesicDistanceFunctions->isChecked()) {
                        log("Add Landmark Correspondence Distance Function Constraint (CID = " + to_string(corr->getId()) + ")... ");
                        vtkSmartPointer<vtkDoubleArray> distances = metricSource->getAllDistances(corr->getCorrespondingIds().at(i));
                        vtkSmartPointer<vtkDoubleArray> normDist = vtkSmartPointer<vtkDoubleArray>::New();
                        normalize(distances, normDist, 1.0);
                        constraintsSource.push_back(normDist);
                        landmarkCorrespondences.push_back(make_pair<int, string>(corr->getId(), "Distance function from source = " + to_string(corr->getCorrespondingIds().at(i))));
                        
                        log("done.\n");
                    }
                }
                
                if(corr->getShapes().at(i) == target) {
                    // Heat diffusion
                    if(groupBoxHeatBumps->isChecked()) {
                        vtkSmartPointer<vtkDoubleArray> u0t = vtkSmartPointer<vtkDoubleArray>::New();
                        u0t->SetNumberOfValues(target->getPolyData()->GetNumberOfPoints());
                        
                        for(vtkIdType j = 0; j < target->getPolyData()->GetNumberOfPoints(); j++) {
                            u0t->SetValue(j, 0.0);
                        }
                        u0t->SetValue(corr->getCorrespondingIds().at(i), doubleSpinBoxHeatBumpsInitialValue->value());
                        
                        
                        PetscHeatDiffusion hdTarget(source, laplacianSource, u0t);
                        for(double t = doubleSpinBoxHeatBumpsFrom->value(); t <= doubleSpinBoxHeatBumpsTo->value(); t+=doubleSpinBoxHeatBumpsStep->value()) {
                            vtkSmartPointer<vtkDoubleArray> normHeat = vtkSmartPointer<vtkDoubleArray>::New();
                            normalize(hdTarget.getHeat(t), normHeat, 1.0);
                            constraintsTarget.push_back(normHeat);
                        }
                    }
                    
                    // distance functions
                    if(checkBoxGeodesicDistanceFunctions->isChecked()) {
                        vtkSmartPointer<vtkDoubleArray> distances = metricTarget->getAllDistances(corr->getCorrespondingIds()[i]);
                        vtkSmartPointer<vtkDoubleArray> normDist = vtkSmartPointer<vtkDoubleArray>::New();
                        normalize(distances, normDist, 1.0);
                        constraintsTarget.push_back(normDist);
                        
                    }
                }
            }
        }
        
        vector<int> waveKernelComponents;
        // add wave kernel compontents to contraint vectors
        if(groupBoxWaveKernelSignature->isChecked()) {
            // compute wave kernel discriptor on both shapes
            log("Compute Wave Kernel Signature of source... ");
            PetscWaveKernelSignature wksSource(source, spinBoxWaveKernelSignatureComponentsTotalNumber->value(), laplacianSource, spinBoxWaveKernelSignatureNumberOfEigenfunctions->value());
            log("done.\n");

            log("Compute Wave Kernel Signature of target... ");
            PetscWaveKernelSignature wksTarget(target, spinBoxWaveKernelSignatureComponentsTotalNumber->value(), laplacianTarget, spinBoxWaveKernelSignatureNumberOfEigenfunctions->value());
            log("done.\n");
            
            log("Add Wave Kernel Signature components... ");
            for(int i = spinBoxWaveKernelSignatureComponentsFrom->value(); i <= spinBoxWaveKernelSignatureComponentsTo->value(); i+=spinBoxWaveKernelSignatureComponentsStep->value()) {
                vtkSmartPointer<vtkDoubleArray> wksiSource = wksSource.getComponent(i);
                vtkSmartPointer<vtkDoubleArray> normWaveS = vtkSmartPointer<vtkDoubleArray>::New();
                normalize(wksiSource, normWaveS, 1.0);
                constraintsSource.push_back(normWaveS);
                
                vtkSmartPointer<vtkDoubleArray> wksiTarget = wksTarget.getComponent(i);
                vtkSmartPointer<vtkDoubleArray> normWaveT = vtkSmartPointer<vtkDoubleArray>::New();
                normalize(wksiTarget, normWaveT, 1.0);
                constraintsTarget.push_back(normWaveT);
                waveKernelComponents.push_back(i);
            }
            log("done.\n");
        }
        
        function<void(int, double)> iterationCallback = [this](int iteration, double residual)->void {
            lcdNumberCurrentIteration->display(iteration);
            lcdNumberResidual->display(residual);
            
            log("    > Iteration " + to_string(iteration) + " (Residual = " + to_string(residual) + ")\n");
        };
        
        // compute correspondence matrix C
        log("Initialize Functional Maps...\n");
        PetscFunctionalMaps functionalMaps(source, target, laplacianSource, laplacianTarget, constraintsSource, constraintsTarget, spinBoxNumberOfEigenfunctions->value(), spinBoxStepSize->value(), doubleSpinBoxSparsityPriorWeight->value(), spinBoxNumberOfIterations->value(), groupBoxOutlierAbsorption->isChecked(), doubleSpinBoxOutlierAbsorptionWeight->value(), iterationCallback);
        log("done.\n");
        
        log("Run Functional Maps Forward Backward Splitting...\n");
        functionalMaps.computeCorrespondence();
        log("done.\n");
        
        if(groupBoxOutlierAbsorption->isChecked()) {
            log("\n\nOutliers scores:\n");
            int i = 0;
            for(auto p : landmarkCorrespondences) {
                log("Correspondence ID " + to_string(p.first) + " => " + p.second + " => Score = " + to_string(functionalMaps.getOutlierScore(i)) + "\n");
                i++;
            }
            
            i = 0;
            for(auto v : waveKernelComponents) {
                log("WKS " + to_string(v) + " => Score = " + to_string(functionalMaps.getOutlierScore(i)) + "\n");
                i++;
            }
        }
        
        if(source->getColoring()->type == Shape::Coloring::Type::PointSegmentation) {
            // find out the number of segments
            int numberOfSegments = 0;
            for(vtkIdType i = 0; i < source->getColoring()->values->GetNumberOfTuples(); i++) {
                if(source->getColoring()->values->GetComponent(i, 0) > numberOfSegments) {
                    numberOfSegments = source->getColoring()->values->GetComponent(i, 0);
                }
            }
            numberOfSegments++;
            
            // transfer each segment seperately as an segment indicator function
            vector<vtkSmartPointer<vtkDoubleArray>> components(numberOfSegments);
            for(int i = 0; i < numberOfSegments; i++) {
                vtkSmartPointer<vtkDoubleArray> indicatorFunction = vtkSmartPointer<vtkDoubleArray>::New();
                indicatorFunction->SetNumberOfValues(source->getPolyData()->GetNumberOfPoints());
                // color 2nd shape
                for(vtkIdType j = 0; j < source->getPolyData()->GetNumberOfPoints(); j++) {
                    if(source->getColoring()->values->GetComponent(j, 0) == i) {
                        indicatorFunction->SetValue(j, 1.0);
                    } else {
                        indicatorFunction->SetValue(j, 0.0);
                    }
                }
                vtkSmartPointer<vtkDoubleArray> Tf = functionalMaps.transferFunction(indicatorFunction);
                target->setColoring(Tf, Shape::Coloring::Type::PointScalar);
                shapeAnalyzer_->render();
                this_thread::sleep_for(chrono::milliseconds(2000));
                components.at(i) = Tf;
            }
            
            vtkSmartPointer<vtkIntArray> Tf = vtkSmartPointer<vtkIntArray>::New();
            Tf->SetNumberOfValues(source->getPolyData()->GetNumberOfPoints());
            
            for(vtkIdType j = 0; j < source->getPolyData()->GetNumberOfPoints(); j++) {
                int segment = 0;
                double score = components.at(0)->GetValue(j);
                for(int i = 1; i < numberOfSegments; i++) {
                    if(components.at(i)->GetValue(j) > score) {
                        score = components.at(i)->GetValue(j);
                        segment = i;
                    }
                }
                
                Tf->SetValue(j, segment);
            }
            
            target->setColoring(Tf, Shape::Coloring::Type::PointSegmentation);
        } else {
            // transfer function 
            vtkSmartPointer<vtkDoubleArray> Tf = functionalMaps.transferFunction(source->getColoring()->values);
            // color 2nd shape
            target->setColoring(Tf, Shape::Coloring::Type::PointScalar);
        }
    } catch(metric::MetricError& me) {
        QMessageBox::warning(dynamic_cast<QWidget*>(shapeAnalyzer_), "Error", QString::fromStdString(me.what()));
    } catch(laplaceBeltrami::LaplaceBeltramiError& le) {
        QMessageBox::warning(dynamic_cast<QWidget*>(shapeAnalyzer_), "Error", QString::fromStdString(le.what()));
    }
}


void custom::tabs::FunctionTransferTab::log(string line) {
    QScrollBar* sb = this->textBrowserLog->verticalScrollBar();
    
    this->textBrowserLog->insertPlainText(QString::fromStdString(line));
    sb->setValue(sb->maximum());
    QCoreApplication::processEvents();
}

///////////////////////////////////////////////////////////////////////////////
void custom::tabs::FunctionTransferTab::normalize(
                                                  vtkSmartPointer<vtkDoubleArray> array,
                                                  vtkSmartPointer<vtkDoubleArray> result,
                                                  double norm
                                                  ) {
    result->SetNumberOfComponents(1);
    result->SetNumberOfTuples(array->GetNumberOfTuples());
    
    double sum = 0;
    
    for (int i = 0; i < array->GetNumberOfTuples(); i++) {
        double value;
        array->GetTuple(i, &value);
        sum = sum + value;
    }
    
    for (int i = 0; i < array->GetNumberOfTuples(); i++) {
        double value = 0;
        array->GetTuple(i, &value);
        value = value / sum;
        result->SetTuple(i, &value);
    }
}

///////////////////////////////////////////////////////////////////////////////
void custom::tabs::FunctionTransferTab::onShapeAdd(Shape* shape) {
    QString label = QString::number(shape->getId());
    label.append(QString::fromStdString(":"+shape->getName()));
    comboBoxSourceShape->insertItem(0, label);
    comboBoxTargetShape->insertItem(0, label);
    
    if(shapes_.size() >= 2) {
        this->buttonTransfer->setEnabled(true);
    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::FunctionTransferTab::onShapeDelete(Shape* shape) {
    for(int i = comboBoxSourceShape->count()-1; i >= 0; i--) {
        if(getIdFromIdentifier(comboBoxSourceShape->itemText(i)) == shape->getId()) {
            comboBoxSourceShape->removeItem(i);
            break;
        }
    }
    
    for(int i = comboBoxTargetShape->count()-1; i >= 0; i--) {
        if(getIdFromIdentifier(comboBoxTargetShape->itemText(i)) == shape->getId()) {
            comboBoxTargetShape->removeItem(i);
            break;
        }
    }
    if(comboBoxSourceShape->count() == 0) {
        this->buttonTransfer->setEnabled(false);
    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::FunctionTransferTab::onShapeEdit(Shape* shape) {
    QString label = getShapeIdentifier(shape);
    
    for(int i = comboBoxSourceShape->count()-1; i >= 0; i--) {
        if(getIdFromIdentifier(comboBoxSourceShape->itemText(i)) == shape->getId()) {
            comboBoxSourceShape->setItemText(i, label);
            break;
        }
    }
    
    for(int i = comboBoxTargetShape->count()-1; i >= 0; i--) {
        if(getIdFromIdentifier(comboBoxTargetShape->itemText(i)) == shape->getId()) {
            comboBoxTargetShape->setItemText(i, label);
            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void custom::tabs::FunctionTransferTab::onClear() {
    this->comboBoxSourceShape->clear();
    this->comboBoxTargetShape->clear();
    this->buttonTransfer->setEnabled(false);
}
