#include "SceneWriterReader.h"


///////////////////////////////////////////////////////////////////////////////
pair<int, int> io::SceneWriterReader::importSceneBinary(
                                string filename,
                                vtkSmartPointer<vtkRenderer> renderer,
                                vector<shared_ptr<Shape>>& shapes,
                                vector<pair<shared_ptr<PointCorrespondence>,
                                        shared_ptr<VisualCorrespondence<PointCorrespondence>>>>&
                                        pointCorrespondences,
                                vector<pair<shared_ptr<FaceCorrespondence>,
                                        shared_ptr<VisualCorrespondence<FaceCorrespondence>>>>&
                                        faceCorrespondences)
{
    //open input file stream in binary mode
    ifstream is(filename, ios::binary);
    
    if (!is.good()) {
        throw IOError("The file is not readable.");

    }
    
    //read number of shapes
    int64_t numberOfShapes;
    is.read(reinterpret_cast<char*>(&numberOfShapes), sizeof(int64_t));
    
    pair<int, int> lastInsertIds;
    //read last insert shape ID
    is.read(reinterpret_cast<char*>(&lastInsertIds.first), sizeof(int64_t));
    
    
    //read shapes
    unordered_map<int, shared_ptr<Shape>> shapesById;
    for(int64_t i = 0; i < numberOfShapes; i++) {
        shared_ptr<Shape> shape = importShapeBinary(is, renderer);
        shapes.push_back(shape);
        shapesById.insert(make_pair(shape->getId(), shape));
    }
    
    // read camera attributes
    double focalPoint[3];
    is.read(reinterpret_cast<char*>(&focalPoint), 3 * sizeof(double));
    renderer->GetActiveCamera()->SetFocalPoint(focalPoint);
    
    double position[3];
    is.read(reinterpret_cast<char*>(&position), 3 * sizeof(double));
    renderer->GetActiveCamera()->SetPosition(position);
    
    double viewUp[3];
    is.read(reinterpret_cast<char*>(&viewUp), 3 * sizeof(double));
    renderer->GetActiveCamera()->SetViewUp(viewUp);
    
    double viewAngle;
    is.read(reinterpret_cast<char*>(&viewAngle), sizeof(double));
    renderer->GetActiveCamera()->SetViewAngle(viewAngle);
    
    double parallelScale;
    is.read(reinterpret_cast<char*>(&parallelScale), sizeof(double));
    renderer->GetActiveCamera()->SetParallelScale(parallelScale);
    
    //read last insert correspondence ID
    is.read(reinterpret_cast<char*>(&lastInsertIds.second), sizeof(int64_t));
    

    //read point correspondences
    int64_t numberOfPointCorrespondences;
    
    is.read(reinterpret_cast<char*>(&numberOfPointCorrespondences), sizeof(int64_t));

    for(int64_t i = 0; i < numberOfPointCorrespondences; i++) {
        int64_t id;
        is.read(reinterpret_cast<char*>(&id), sizeof(int64_t));
        shared_ptr<PointCorrespondence> correspondence = make_shared<PointCorrespondence>(id);
        
        bool visible;
        is.read(reinterpret_cast<char*>(&visible), sizeof(bool));

        int64_t numberOfShapes;
        is.read(reinterpret_cast<char*>(&numberOfShapes), sizeof(int64_t));
        
        for(int64_t j = 0; j < numberOfShapes; j++) {
            int64_t shapeId;
            int64_t pointId;
            is.read(reinterpret_cast<char*>(&shapeId), sizeof(int64_t));
            is.read(reinterpret_cast<char*>(&pointId), sizeof(int64_t));
            correspondence->addShape(shapesById.at(shapeId), pointId);
        }
        
        shared_ptr<VisualCorrespondence<PointCorrespondence>> visualCorrespondence = nullptr;
        if(visible) {
            visualCorrespondence.reset(new VisualCorrespondence<PointCorrespondence>(renderer, correspondence));        }
        
        pointCorrespondences.push_back(make_pair(correspondence, visualCorrespondence));
    }
    
    //read face correspondences
    int64_t numberOfFaceCorrespondences;
    is.read(reinterpret_cast<char*>(&numberOfFaceCorrespondences), sizeof(int64_t));

    for(int64_t i = 0; i < numberOfFaceCorrespondences; i++) {
        int64_t id;
        is.read(reinterpret_cast<char*>(&id), sizeof(int64_t));
        shared_ptr<FaceCorrespondence> correspondence = make_shared<FaceCorrespondence>(id);
        
        bool visible;
        is.read(reinterpret_cast<char*>(&visible), sizeof(bool));
        
        int64_t numberOfShapes;
        is.read(reinterpret_cast<char*>(&numberOfShapes), sizeof(int64_t));
        
        for(int64_t j = 0; j < numberOfShapes; j++) {
            int64_t shapeId;
            int64_t pointId;
            is.read(reinterpret_cast<char*>(&shapeId), sizeof(int64_t));
            is.read(reinterpret_cast<char*>(&pointId), sizeof(int64_t));
            correspondence->addShape(shapesById.at(shapeId), pointId);
        }
        
        shared_ptr<VisualCorrespondence<FaceCorrespondence>> visualCorrespondence = nullptr;
        if(visible) {
            visualCorrespondence.reset(new VisualCorrespondence<FaceCorrespondence>(renderer, correspondence));        }
        
        faceCorrespondences.push_back(make_pair(correspondence, visualCorrespondence));
    }
    is.close();
    
    // check if stream is empty
    is.peek();
    if (!is.eof()) {
        throw IOError("The file content is not a scene.");
        
    }
    
    return lastInsertIds;
}


///////////////////////////////////////////////////////////////////////////////
void io::SceneWriterReader::exportSceneBinary(
                    string filename,
                    vtkSmartPointer<vtkRenderer> renderer,
                    vector<shared_ptr<Shape>>& shapes,
                    int lastInsertShapeId,
                    vector<pair<shared_ptr<PointCorrespondence>,
                                shared_ptr<VisualCorrespondence<PointCorrespondence>>>>&
                                pointCorrespondences,
                    vector<pair<shared_ptr<FaceCorrespondence>,
                                shared_ptr<VisualCorrespondence<FaceCorrespondence>>>>&
                                faceCorrespondences,
                    int lastInsertCorrespondenceId)
{
    ofstream os(filename, ios::binary);
    int64_t numberOfShapes = (int64_t) shapes.size();
    os.write(reinterpret_cast<char*>(&numberOfShapes), sizeof(int64_t));
    
    int64_t lastInsertSid = (int64_t) lastInsertShapeId;
    os.write(reinterpret_cast<char*>(&lastInsertSid), sizeof(int64_t));
    
    // write shapes
    for(auto it : shapes) {
        exportShapeBinary(os, it.get());
    }
    
    // write camera attributes
    double focalPoint[3];
    renderer->GetActiveCamera()->GetFocalPoint(focalPoint);
    os.write(reinterpret_cast<const char*>(focalPoint), 3 * sizeof(double));
    
    double position[3];
    renderer->GetActiveCamera()->GetPosition(position);
    os.write(reinterpret_cast<const char*>(position), 3 * sizeof(double));
    
    double viewUp[3];
    renderer->GetActiveCamera()->GetViewUp(viewUp);
    os.write(reinterpret_cast<const char*>(viewUp), 3 * sizeof(double));
    
    double viewAngle = renderer->GetActiveCamera()->GetViewAngle();
    os.write(reinterpret_cast<const char*>(&viewAngle), sizeof(double));
    
    double parallelScale = renderer->GetActiveCamera()->GetParallelScale();
    os.write(reinterpret_cast<const char*>(&parallelScale), sizeof(double));
    
    // write last insert correspondence id
    int64_t lastInsertCid = (int64_t) lastInsertCorrespondenceId;
    os.write(reinterpret_cast<char*>(&lastInsertCid), sizeof(int64_t));
    
    // write point correspondences
    int64_t numberOfPointCorrespondences = (int64_t) pointCorrespondences.size();
    os.write(reinterpret_cast<char*>(&numberOfPointCorrespondences), sizeof(int64_t));
    
    
    for(auto entry : pointCorrespondences) {
        int64_t id = entry.first->getId();
        os.write(reinterpret_cast<char*>(&id), sizeof(int64_t));
        
        bool visible = entry.second != nullptr;
        os.write(reinterpret_cast<char*>(&visible), sizeof(bool));
        
        int64_t numberOfShapes = entry.first->size();
        os.write(reinterpret_cast<char*>(&numberOfShapes), sizeof(int64_t));
        
        for(int64_t i = 0; i < numberOfShapes; i++) {
            int64_t shapeId = entry.first->getShapes().at(i)->getId();
            int64_t pointId = entry.first->getCorrespondingIds().at(i);
            os.write(reinterpret_cast<char*>(&shapeId), sizeof(int64_t));
            os.write(reinterpret_cast<char*>(&pointId), sizeof(int64_t));
        }
    }

    // write face correspondences
    int64_t numberOfFaceCorrespondences = (int64_t) faceCorrespondences.size();
    os.write(reinterpret_cast<char*>(&numberOfFaceCorrespondences), sizeof(int64_t));
    
    
    for(auto entry : faceCorrespondences) {
        int64_t id = entry.first->getId();
        os.write(reinterpret_cast<char*>(&id), sizeof(int64_t));
        
        bool visible = entry.second != nullptr;
        os.write(reinterpret_cast<char*>(&visible), sizeof(bool));
        
        int64_t numberOfShapes = entry.first->size();
        os.write(reinterpret_cast<char*>(&numberOfShapes), sizeof(int64_t));
        
        for(int64_t i = 0; i < numberOfShapes; i++) {
            int64_t shapeId = entry.first->getShapes().at(i)->getId();
            int64_t pointId = entry.first->getCorrespondingIds().at(i);
            os.write(reinterpret_cast<char*>(&shapeId), sizeof(int64_t));
            os.write(reinterpret_cast<char*>(&pointId), sizeof(int64_t));
        }
    }
    
    os.close();
}


///////////////////////////////////////////////////////////////////////////////
pair<int, int> io::SceneWriterReader::importSceneASCII(string filename,
                            vtkSmartPointer<vtkRenderer> renderer,
                            vector<shared_ptr<Shape>>& shapes,
                            vector<pair<shared_ptr<PointCorrespondence>,
                                        shared_ptr<VisualCorrespondence<PointCorrespondence>>>>&
                                        pointCorrespondences,
                            vector<pair<shared_ptr<FaceCorrespondence>,
                                        shared_ptr<VisualCorrespondence<FaceCorrespondence>>>>&
                                        faceCorrespondences)
{
    ifstream is(filename);
    
    if (!is.good()) {
        throw IOError("The file is not readable.");
        
    }
    
    string line;
    
    pair<int, int> lastInsertIds;
    
    // read shapes
    unsigned int numberOfShapes;
    {
        stringstream ss;
        getline(is, line);
        ss << line;
        ss >> numberOfShapes;
    }
    
    {
        stringstream ss;
        getline(is, line);
        ss << line;
        ss >> lastInsertIds.first;
    }
    
    unordered_map<int, shared_ptr<Shape>> shapesById;
    //read shapes
    for(unsigned int i = 0; i < numberOfShapes; i++) {
        shared_ptr<Shape> shape = importShapeASCII(is, renderer);
        
        shapes.push_back(shape);
        shapesById.insert(make_pair(shape->getId(), shape));
    }
    
    // read camera attributes
    double focalPoint[3];
    {
        stringstream ss;
        getline(is, line);
        ss << line;
        ss >> focalPoint[0];
        ss >> focalPoint[1];
        ss >> focalPoint[2];
    }
    renderer->GetActiveCamera()->SetFocalPoint(focalPoint);
    
    double position1, position2, position3;
    {
        stringstream ss;
        getline(is, line);
        ss << line;
        ss >> position1;
        ss >> position2;
        ss >> position3;
    }
    renderer->GetActiveCamera()->SetPosition(position1, position2, position3);
    
    double viewUp[3];
    {
        stringstream ss;
        getline(is, line);
        ss << line;
        ss >> viewUp[0];
        ss >> viewUp[1];
        ss >> viewUp[2];
    }
    renderer->GetActiveCamera()->SetViewUp(viewUp);
    
    double viewAngle;
    {
        stringstream ss;
        getline(is, line);
        ss << line;
        ss >> viewAngle;
    }
    renderer->GetActiveCamera()->SetViewAngle(viewAngle);
    
    double parallelScale;
    {
        stringstream ss;
        getline(is, line);
        ss << line;
        ss >> parallelScale;
    }
    renderer->GetActiveCamera()->SetParallelScale(parallelScale);
    
    // read correspondences
    
    //read last insert correspondence ID
    {
        stringstream ss;
        getline(is, line);
        ss << line;
        ss >> lastInsertIds.second;
    }
    
    
    //read point correspondences
    int numberOfPointCorrespondences;
    {
        stringstream ss;
        getline(is, line);
        ss << line;
        ss >> numberOfPointCorrespondences;
    }
    
    for(int i = 0; i < numberOfPointCorrespondences; i++) {
        stringstream ss;
        getline(is, line);
        ss << line;
        
        int id;
        ss >> id;
        
        shared_ptr<PointCorrespondence> correspondence = make_shared<PointCorrespondence>(id);
        
        bool visible;
        ss >> visible;
        
        int numberOfShapes;
        ss >> numberOfShapes;
        
        for(int j = 0; j < numberOfShapes; j++) {
            int shapeId;
            int pointId;
            ss >> shapeId;
            ss >> pointId;
            correspondence->addShape(shapesById.at(shapeId), pointId);
        }
        
        shared_ptr<VisualCorrespondence<PointCorrespondence>> visualCorrespondence = nullptr;
        if(visible) {
            visualCorrespondence.reset(new VisualCorrespondence<PointCorrespondence>(renderer, correspondence));
        }
        
        pointCorrespondences.push_back(make_pair(correspondence, visualCorrespondence));
    }
    
    //read face correspondences
    int numberOfFaceCorrespondences;
    {
        stringstream ss;
        getline(is, line);
        ss << line;
        ss >> numberOfFaceCorrespondences;
    }
    
    for(int i = 0; i < numberOfFaceCorrespondences; i++) {
        stringstream ss;
        getline(is, line);
        ss << line;
        
        int id;
        ss >> id;
        
        shared_ptr<FaceCorrespondence> correspondence = make_shared<FaceCorrespondence>(id);
        
        bool visible;
        ss >> visible;
        
        int numberOfShapes;
        ss >> numberOfShapes;
        
        for(int j = 0; j < numberOfShapes; j++) {
            int shapeId;
            int pointId;
            ss >> shapeId;
            ss >> pointId;
            correspondence->addShape(shapesById.at(shapeId), pointId);
        }
        
        shared_ptr<VisualCorrespondence<FaceCorrespondence>> visualCorrespondence = nullptr;
        if(visible) {
            visualCorrespondence.reset(new VisualCorrespondence<FaceCorrespondence>(renderer, correspondence));
        }
        
        faceCorrespondences.push_back(make_pair(correspondence, visualCorrespondence));
    }
    
    // check if stream is empty
    is.peek();
    if (!is.eof()) {
        throw IOError("The file content is not a scene.");
        
    }
    
    is.close();
    
    return lastInsertIds;
}


///////////////////////////////////////////////////////////////////////////////
void io::SceneWriterReader::exportSceneASCII(
                            string                          filename,
                            vtkSmartPointer<vtkRenderer>    renderer,
                            vector<shared_ptr<Shape>>&      shapes,
                            int lastInsertShapeId,
                            vector<pair<shared_ptr<PointCorrespondence>,
                                        shared_ptr<VisualCorrespondence<PointCorrespondence>>>>&
                                        pointCorrespondences,
                            vector<pair<shared_ptr<FaceCorrespondence>,
                                        shared_ptr<VisualCorrespondence<FaceCorrespondence>>>>&
                                        faceCorrespondences,
                                        int lastInsertCorrespondenceId)
{
    ofstream os(filename);
    
    // write shapes
    os << shapes.size() << endl;
    os << lastInsertShapeId << endl;
    for(auto it : shapes) {
        exportShapeASCII(os, it.get());
    }
    
    // write camera attributes
    double focalPoint[3];
    renderer->GetActiveCamera()->GetFocalPoint(focalPoint);
    os << focalPoint[0] << "\t" << focalPoint[1] << "\t" << focalPoint[2] << endl;
    
    double position1, position2, position3;
    renderer->GetActiveCamera()->GetPosition(position1, position2, position3);
    os << position1 << "\t" << position2 << "\t" << position3 << endl;
    
    double viewUp[3];
    renderer->GetActiveCamera()->GetViewUp(viewUp);
    os << viewUp[0] << "\t" << viewUp[1] << "\t" << viewUp[2] << endl;
    
    double viewAngle = renderer->GetActiveCamera()->GetViewAngle();
    os << viewAngle << endl;
    
    double parallelScale = renderer->GetActiveCamera()->GetParallelScale();
    os << parallelScale << endl;
    
    // write last insert correspondence id
    os << lastInsertCorrespondenceId;
    os << endl;
    
    // write point correspondences
    int numberOfPointCorrespondences = pointCorrespondences.size();
    os << numberOfPointCorrespondences;
    os << endl;
    
    for(auto entry : pointCorrespondences) {
        int id = entry.first->getId();
        os << id << "\t";
        
        bool visible = entry.second != nullptr;
        os << visible << "\t";
        
        int numberOfShapes = entry.first->size();
        os << numberOfShapes << "\t";
        
        for(int i = 0; i < numberOfShapes; i++) {
            int shapeId = entry.first->getShapes().at(i)->getId();
            int pointId = entry.first->getCorrespondingIds().at(i);
            os << shapeId << "\t";
            os << pointId << "\t";
        }
        
        os << endl;
    }
    
    // write face correspondences
    int numberOfFaceCorrespondences = faceCorrespondences.size();
    os << numberOfFaceCorrespondences;
    os << endl;
    
    for(auto entry : faceCorrespondences) {
        int id = entry.first->getId();
        os << id << "\t";
        
        bool visible = entry.second != nullptr;
        os << visible << "\t";
        
        int numberOfShapes = entry.first->size();
        os << numberOfShapes << "\t";
        
        for(int i = 0; i < numberOfShapes; i++) {
            int shapeId = entry.first->getShapes().at(i)->getId();
            int pointId = entry.first->getCorrespondingIds().at(i);
            os << shapeId << "\t";
            os << pointId << "\t";
        }
        
        os << endl;
    }
    
    os.close();
}


///////////////////////////////////////////////////////////////////////////////
void io::SceneWriterReader::exportPointCorrespondencesASCII(
                            const HashMap<shared_ptr<PointCorrespondence>, bool>&   pointCorrespondences,
                            const vector<shared_ptr<Shape>>&                        shapesOrderedById,
                            const string                                            filename)
{
    ofstream os(filename);
    
    // basic information, point correspondences, number of shapes
    // number of correspondences
    os << "P ";
    
    os << shapesOrderedById.size() << " " << pointCorrespondences.size() << endl;
    // write down all correspondences
    for (auto entry : pointCorrespondences) {
        const vector<shared_ptr<Shape>>& shapes = entry.first->getShapes();
        const vector<vtkIdType>& correspondingIds = entry.first->getCorrespondingIds();
        
        // get the same order of shapes for all lines
        for (int i = 0; i < shapesOrderedById.size(); i++) {
            bool contained = false;
            // check for each possible shape whether it is in this correspondences or not
            for (int j = 0; j < shapes.size(); j++) {
                if (shapes.at(j) == shapesOrderedById.at(i)) {
                    // data[i] is the id of the correspondence element on the shape[i]
                    os << correspondingIds.at(j) << " ";
                    contained = true;
                    break;
                }
            }
            // -1 indicates this shape is not in the correspondence
            if (!contained) {
                os << "-1 ";
            }
        }
        // one line for each correspondence
        os << endl;
    }
    os.close();
}


///////////////////////////////////////////////////////////////////////////////
void io::SceneWriterReader::exportFaceCorrespondencesASCII(
                                                       const HashMap<shared_ptr<FaceCorrespondence>, bool>&    faceCorrespondences,
                                                       const vector<shared_ptr<Shape>>&                        shapesOrderedById,
                                                       const string                                            filename
                                                       )
{
    ofstream os(filename);
    
    // basic information, face correspondences, number of shapes
    // number of correspondences
    os << "F ";
    
    os << shapesOrderedById.size() << " " << faceCorrespondences.size() << endl;
    // write down all correspondences
    for (auto entry : faceCorrespondences) {
        const vector<shared_ptr<Shape>>& shapes = entry.first->getShapes();
        const vector<vtkIdType>& correspondingIds = entry.first->getCorrespondingIds();
        
        // get the same order of shapes for all lines
        for (int i = 0; i < shapesOrderedById.size(); i++) {
            bool contained = false;
            // check for each possible shape whether it is in this correspondences or not
            for (int j = 0; j < shapes.size(); j++) {
                if (shapes.at(j) == shapesOrderedById.at(i)) {
                    // data[i] is the id of the correspondence element on the shape[i]
                    os << correspondingIds.at(j) << " ";
                    contained = true;
                    break;
                }
            }
            // -1 indicates this shape is not in the correspondence
            if (!contained) {
                os << "-1 ";
            }
        }
        // one line for each correspondence
        os << endl;
    }
    
    os.close();
}


///////////////////////////////////////////////////////////////////////////////
void io::SceneWriterReader::importCorrespondencesASCII(
                                                   const string                                    filename,
                                                   function<shared_ptr<PointCorrespondence>()>     pointCorrespondenceFactory,
                                                   function<shared_ptr<FaceCorrespondence>()>      faceCorrespondenceFactory,
                                                   vector<shared_ptr<PointCorrespondence>>&        pointCorrespondences,
                                                   vector<shared_ptr<FaceCorrespondence>>&         faceCorrespondences,
                                                   vector<shared_ptr<Shape>>&                      shapesOrderedById,
                                                   QWidget*                                        parentWidget
                                                   )
{

    ifstream is(filename);
    
    if (!is.good()) {
        throw IOError("The file is not readable.");
        
    }
    
    string line;
    
    // read 3 lines, type of correspondences, number of shapes, number of correspondences
    char type;
    unsigned int numberOfShapes, numberOfCorrespondences;
    
    stringstream ss;
    getline(is, line);
    ss << line;
    ss >> type >> numberOfShapes >> numberOfCorrespondences;
    

    // stop if there not enough shapes to match
    if (shapesOrderedById.size() < numberOfShapes) {
        throw IOError("The scene does not contain enough shapes or the file is faulty.");
    }
    
    QStringList labels;
    // add all shapes that have not been chosen yet
    for (auto it : shapesOrderedById) {
        QString label = QString::number(it->getId());
        label.append(QString::fromStdString(":"+it->getName()));
        labels << label;
    }
    
    vector<shared_ptr<Shape>> correspondingShapes;
    
    // user chooses numberOfShapes many shapes to correspond
    unsigned counter = 1;
    for (int i = 0; i < numberOfShapes; i++) {        
        bool ok;
        QString chosen = QInputDialog::getItem(parentWidget,
                                               QObject::tr("Choose a shape:"),
                                               QString(QObject::tr("Shape for column ")).append(QString::number(counter)),
                                               labels,
                                               0,
                                               true,
                                               &ok);
        if(!ok) {
            return;
        }
        
        shared_ptr<Shape> shape = nullptr;
        // add chosen shape to vector
        for(int j = 0; j < shapesOrderedById.size(); j++) {
            vtkIdType shapeId = chosen.split(':')[0].toInt();
            if(shapesOrderedById.at(j)->getId() == shapeId) {
                shape = shapesOrderedById.at(j);
                break;
            }
        }
        
        correspondingShapes.push_back(shape);
        // delete chosen shape from map, so it can not be chosen again
        labels.removeOne(chosen);
        counter++;
    }
    
    // create correspondeneces
    for (int i = 0; i < numberOfCorrespondences; i++) {
        shared_ptr<Correspondence> correspondence;
        if (type == 'F') {
            correspondence = faceCorrespondenceFactory();
        } else {
            correspondence = pointCorrespondenceFactory();
        }
        
        stringstream ss;
        getline(is, line);
        ss << line;
        for (auto it : correspondingShapes) {
            int id;
            ss >> id;
            if (id > -1) {
                correspondence->addShape(it, id);
            }
        }
        
        if (type == 'F') {
            faceCorrespondences.push_back(dynamic_pointer_cast<FaceCorrespondence>(correspondence));
        } else {
            pointCorrespondences.push_back(dynamic_pointer_cast<PointCorrespondence>(correspondence));
        }
    }
    
    is.peek();
    if (!is.eof()) {
        throw IOError("The file content is not a valid correspondence set.");
        
    }
    
    is.close();
}


///////////////////////////////////////////////////////////////////////////////
void io::SceneWriterReader::exportPointCorrespondencesBinary(
                                                         const HashMap<shared_ptr<PointCorrespondence>, bool>&   pointCorrespondences,
                                                         const vector<shared_ptr<Shape>>&                        shapesOrderedById,
                                                         const string                                            filename
                                                         )
{
    ofstream os(filename, ios::binary);
    
    // write type
    const char type = 'P';
    os.write(&type, sizeof(char));

    int64_t numberOfShapes = shapesOrderedById.size();
    os.write(reinterpret_cast<char*>(&numberOfShapes), sizeof(int64_t));
    
    int64_t numberOfCorrespondences = pointCorrespondences.size();
    os.write(reinterpret_cast<char*>(&numberOfCorrespondences), sizeof(int64_t));
    
    // write down all correspondences
    for (auto entry : pointCorrespondences) {
        const vector<shared_ptr<Shape>>& shapes = entry.first->getShapes();
        const vector<vtkIdType>& correspondingIds = entry.first->getCorrespondingIds();
        
        // get the same order of shapes for all lines
        for (int i = 0; i < shapesOrderedById.size(); i++) {
            bool contained = false;
            // check for each possible shape whether it is in this correspondences or not
            for (int j = 0; j < shapes.size(); j++) {
                if (shapes.at(j) == shapesOrderedById.at(i)) {
                    // data[i] is the id of the correspondence element on the shape[i]
                    int64_t correspondingId = (int64_t) correspondingIds.at(j);
                    os.write(reinterpret_cast<char*>(&correspondingId), sizeof(int64_t));
                    contained = true;
                    break;
                }
            }
            // -1 indicates this shape is not in the correspondence
            if (!contained) {
                int64_t correspondingId = -1;
                os.write(reinterpret_cast<char*>(&correspondingId), sizeof(int64_t));
            }
        }
    }
    
    os.close();
}


///////////////////////////////////////////////////////////////////////////////
void io::SceneWriterReader::exportFaceCorrespondencesBinary(
                                                        const HashMap<shared_ptr<FaceCorrespondence>, bool>&    faceCorrespondences,
                                                        const vector<shared_ptr<Shape>>&                        shapesOrderedById,
                                                        const string                                            filename
                                                        )
{
    ofstream os(filename, ios::binary);
    
    // write type
    const char type = 'F';
    os.write(&type, sizeof(char));
    
    int64_t numberOfShapes = shapesOrderedById.size();
    os.write(reinterpret_cast<char*>(&numberOfShapes), sizeof(int64_t));
    
    int64_t numberOfCorrespondences = faceCorrespondences.size();
    os.write(reinterpret_cast<char*>(&numberOfCorrespondences), sizeof(int64_t));
    
    // write down all correspondences
    for (auto entry : faceCorrespondences) {
        const vector<shared_ptr<Shape>>& shapes = entry.first->getShapes();
        const vector<vtkIdType>& correspondingIds = entry.first->getCorrespondingIds();
        
        // get the same order of shapes for all lines
        for (int i = 0; i < shapesOrderedById.size(); i++) {
            bool contained = false;
            // check for each possible shape whether it is in this correspondences or not
            for (int j = 0; j < shapes.size(); j++) {
                if (shapes.at(j) == shapesOrderedById.at(i)) {
                    // data[i] is the id of the correspondence element on the shape[i]
                    int64_t correspondingId = (int64_t) correspondingIds.at(j);
                    os.write(reinterpret_cast<char*>(&correspondingId), sizeof(int64_t));
                    contained = true;
                    break;
                }
            }
            // -1 indicates this shape is not in the correspondence
            if (!contained) {
                int64_t correspondingId = -1;
                os.write(reinterpret_cast<char*>(&correspondingId), sizeof(int64_t));
            }
        }
    }
    
    os.close();
}


///////////////////////////////////////////////////////////////////////////////
void io::SceneWriterReader::importCorrespondencesBinary(
                                                    const string                                    filename,
                                                    function<shared_ptr<PointCorrespondence>()>     pointCorrespondenceFactory,
                                                    function<shared_ptr<FaceCorrespondence>()>      faceCorrespondenceFactory,
                                                    vector<shared_ptr<PointCorrespondence>>&        pointCorrespondences,
                                                    vector<shared_ptr<FaceCorrespondence>>&         faceCorrespondences,
                                                    vector<shared_ptr<Shape>>&                      shapesOrderedById,
                                                    QWidget*                                        parentWidget
                                                    )
{
    
    ifstream is(filename, ios::binary);
    
    if (!is.good()) {
        throw IOError("The file is not readable.");
        
    }

    // read 3 lines, type of correspondences, number of shapes, number of correspondences
    char type;
    int64_t numberOfShapes, numberOfCorrespondences;
    
    is.read(&type, sizeof(char));
    is.read(reinterpret_cast<char*>(&numberOfShapes), sizeof(int64_t));
    is.read(reinterpret_cast<char*>(&numberOfCorrespondences), sizeof(int64_t));
    
    
    // stop if there not enough shapes to match
    if (shapesOrderedById.size() < numberOfShapes) {
        throw IOError("The scene does not contain enough shapes or the file is faulty.");
    }
    
    QStringList labels;
    // add all shapes that have not been chosen yet
    for (auto it : shapesOrderedById) {
        QString label = QString::number(it->getId());
        label.append(QString::fromStdString(":"+it->getName()));
        labels << label;
    }
    
    vector<shared_ptr<Shape>> correspondingShapes;
    

    // user chooses numberOfShapes many shapes to correspond
    unsigned counter = 1;
    for (int i = 0; i < numberOfShapes; i++) {
        bool ok;
        QString chosen = QInputDialog::getItem(parentWidget,
                                               QObject::tr("Choose a shape:"),
                                               QString(QObject::tr("Shape for column ")).append(QString::number(counter)),
                                               labels,
                                               0,
                                               true,
                                               &ok);
        if(!ok) {
            return;
        }
        
        shared_ptr<Shape> shape = nullptr;
        // add chosen shape to vector
        for(int j = 0; j < shapesOrderedById.size(); j++) {
            vtkIdType shapeId = chosen.split(':')[0].toInt();
            if(shapesOrderedById.at(j)->getId() == shapeId) {
                shape = shapesOrderedById.at(j);
                break;
            }
        }
        
        correspondingShapes.push_back(shape);
        // delete chosen shape from map, so it can not be chosen again
        labels.removeOne(chosen);
        counter++;
    }
    
    // create correspondeneces
    for (int i = 0; i < numberOfCorrespondences; i++) {
        shared_ptr<Correspondence> correspondence;
        if (type == 'F') {
            correspondence = faceCorrespondenceFactory();
        } else {
            correspondence = pointCorrespondenceFactory();
        }
        
        for (auto it : correspondingShapes) {
            int64_t id;

            is.read(reinterpret_cast<char*>(&id), sizeof(int64_t));
            
            if (id > -1) {
                correspondence->addShape(it, id);
            }
        }
        
        if (type == 'F') {
            faceCorrespondences.push_back(dynamic_pointer_cast<FaceCorrespondence>(correspondence));
        } else {
            pointCorrespondences.push_back(dynamic_pointer_cast<PointCorrespondence>(correspondence));
        }
    }
    
    is.peek();
    if (!is.eof()) {
        throw IOError("The file content is not a valid correspondence set.");
        
    }
    
    is.close();
}


/////////////////////////////////////////////////////////////////////////////////
void io::SceneWriterReader::exportShapeBinary(string filename, Shape* shape) {
    ofstream os(filename, ios::binary);
    
    exportShapeBinary(os, shape);
    
    os.close();
}


/////////////////////////////////////////////////////////////////////////////////
void io::SceneWriterReader::exportShapeBinary(ostream& os, Shape* shape) {
    
    //write shape ID.
    int64_t id = (int64_t) shape->getId();
    os.write(reinterpret_cast<const char*>(&id), sizeof(int64_t));
    
    const char* name = shape->getName().c_str();
    
    int64_t length = shape->getName().length();
    os.write(reinterpret_cast<const char*>(&length), sizeof(int64_t));
    os.write(name, length*sizeof(char));
    
    vtkSmartPointer<vtkPolyData> polyData = shape->getPolyData();
    
    vtkSmartPointer<vtkMatrix4x4> transform = shape->getActor()->GetUserMatrix();
    //if user has not transformed shape write identity.
    if(transform == nullptr) {
        transform = vtkSmartPointer<vtkMatrix4x4>::New();
        transform->Identity();
    }
    
    //write elements of 4x4 matrix
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            double value = transform->GetElement(i, j);
            os.write(reinterpret_cast<const char*>(&value), sizeof(double));
        }
    }
    
    //write number of points.
    int64_t numberOfPoints = (int64_t) polyData->GetNumberOfPoints();
    os.write(reinterpret_cast<const char*>(&numberOfPoints), sizeof(int64_t));
    
    
    //write number of faces.
    int64_t numberOfFaces = (int64_t) polyData->GetNumberOfCells();
    os.write(reinterpret_cast<const char*>(&numberOfFaces), sizeof(int64_t));
    
    //write points.
    for(int64_t i = 0; i < numberOfPoints; i++) {
        double point[3];
        polyData->GetPoints()->GetPoint(i, point);
        
        os.write(reinterpret_cast<const char*>(point), 3 * sizeof(double));
    }
    
    //write faces.
    for(int64_t i = 0; i < numberOfFaces; i++) {
        for(int j = 0; j < 3; j++) {
            int64_t pointId = (int64_t) polyData->GetCell(i)->GetPointId(j);
            os.write(reinterpret_cast<const char*>(&pointId), sizeof(int64_t));
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////
shared_ptr<Shape> io::SceneWriterReader::importShapeBinary(string filename, vtkSmartPointer<vtkRenderer> renderer) {
    ifstream is(filename, ios::binary);
    
    if (!is.good()) {
        throw IOError("The file is not readable.");
        
    }
    
    shared_ptr<Shape> shape = importShapeBinary(is, renderer);
    
    is.peek();
    if (!is.eof()) {
        throw IOError("The file content is not a valid shape.");
        
    }
    
    is.close();
    
    return shape;
}


/////////////////////////////////////////////////////////////////////////////////
shared_ptr<Shape> io::SceneWriterReader::importShapeBinary(istream& is, vtkSmartPointer<vtkRenderer> renderer) {
    
    vtkIdType id_in;
    string name_in;
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    
        //read shape ID
        int64_t id;
        is.read(reinterpret_cast<char*>(&id), sizeof(int64_t));
        id_in = id;
    
        int64_t length;
        is.read(reinterpret_cast<char*>(&length), sizeof(int64_t));
        char* name = new char[length+1];
        is.read(name, length*sizeof(char));
        name[length] = '\0';
    
        name_in = name;
        delete [] name;
    
        //read user transform. Set user transform in actor after poly data has been read and shape has been initialized and therefore actor_ != nullptr
        vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                double value;
                is.read(reinterpret_cast<char*>(&value), sizeof(double));
                matrix->SetElement(i, j, value);
            }
        }
    
        //read number of points.
        int64_t numberOfPoints;
        is.read(reinterpret_cast<char*>(&numberOfPoints), sizeof(int64_t));
        //read number of faces.
        int64_t numberOfFaces;
        is.read(reinterpret_cast<char*>(&numberOfFaces), sizeof(int64_t));
    
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    
        //read points
        for(int64_t i = 0; i < numberOfPoints; i++) {
            double point[3];
            is.read(reinterpret_cast<char*>(point), sizeof(double) * 3);
            points->InsertNextPoint(point);
        }
    
        //read faces
        for(int64_t i = 0; i < numberOfFaces; i++) {
            vtkSmartPointer<vtkTriangle> face = vtkSmartPointer<vtkTriangle>::New();
            for(int j = 0; j < 3; j++) {
                int64_t pointId;
                is.read(reinterpret_cast<char*>(&pointId), sizeof(int64_t));
    
                face->GetPointIds()->SetId(j, pointId);
            }
            polys->InsertNextCell(face);
        }
        polyData->SetPoints(points);
        polyData->SetPolys(polys);
    
    
    return make_shared<Shape>(id_in, name_in, polyData, renderer, matrix);

}


/////////////////////////////////////////////////////////////////////////////////
void io::SceneWriterReader::exportShapeASCII(string filename, Shape* shape) {
    ofstream os(filename);
    
    exportShapeASCII(os, shape);
    
    os.close();
}


/////////////////////////////////////////////////////////////////////////////////
void io::SceneWriterReader::exportShapeASCII(ostream& os, Shape* shape) {
    
    os << shape->getId() << endl;
    os << shape->getName() << endl;
    
    vtkSmartPointer<vtkPolyData> polydata = shape->getPolyData();
    
    vtkSmartPointer<vtkMatrix4x4> transform = shape->getActor()->GetUserMatrix();
    if(transform == nullptr) {
        transform = vtkSmartPointer<vtkMatrix4x4>::New();
        transform->Identity();

    }
    
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            os << transform->GetElement(i, j) << (j != 3 ? "\t" : "");
        }
        os << endl;
    }
    
    os << polydata->GetNumberOfPoints() << "\t" << polydata->GetNumberOfCells() << endl;
    
    for(vtkIdType i = 0; i < polydata->GetNumberOfPoints(); i++) {
        double point[3];
        polydata->GetPoints()->GetPoint(i, point);
        os << point[0] << "\t" << point[1] << "\t" << point[2] << endl;
    }
    
    for(vtkIdType i = 0; i < polydata->GetNumberOfCells(); i++) {
        os << polydata->GetCell(i)->GetPointId(0) << "\t" << polydata->GetCell(i)->GetPointId(1) << "\t" << polydata->GetCell(i)->GetPointId(2) << endl;
    }
}


/////////////////////////////////////////////////////////////////////////////////
shared_ptr<Shape> io::SceneWriterReader::importShapeASCII(string filename, vtkSmartPointer<vtkRenderer> renderer) {
    ifstream is(filename);
    
    if (!is.good()) {
        throw IOError("The file is not readable.");
        
    }
    
    shared_ptr<Shape> shape = importShapeASCII(is, renderer);
    
    is.peek();
    if (!is.eof()) {
        throw IOError("The file content is not a valid shape.");
        
    }
    
    is.close();
    
    return shape;
}


/////////////////////////////////////////////////////////////////////////////////
shared_ptr<Shape> io::SceneWriterReader::importShapeASCII(istream& is, vtkSmartPointer<vtkRenderer> renderer) {
    
    vtkIdType id;
    string name;
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    
    string line;
    
        //read shape ID.
        {
            getline(is, line);
            stringstream ss;
            ss << line;
            ss >> id;
        }
    
        {
            getline(is, line);
            stringstream ss;
            ss << line;
            ss >> name;
        }
    
        //read user transform.
        vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
        for(int i = 0; i < 4; i++) {
            getline(is, line);
            stringstream ss;
            ss << line;
            for(int j = 0; j < 4; j++) {
                double value;
                ss >> value;
                matrix->SetElement(i, j, value);
            }
        }
    
        //read number of points, number of faces
        vtkIdType numberOfPoints, numberOfFaces;
        {
            getline(is, line);
            stringstream ss;
            ss << line;
            ss >> numberOfPoints >> numberOfFaces;
        }
    
        //read points, faces
        vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
        vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    
        for(vtkIdType i = 0; i < numberOfPoints; i++) {
            getline(is, line);
            stringstream ss;
            ss << line;
            double point[3];
            ss >> point[0] >> point[1] >> point[2];
    
            points->InsertNextPoint(point);
    
        }
    
        for(vtkIdType i = 0; i < numberOfFaces; i++) {
            getline(is, line);
            stringstream ss;
            ss << line;
    
            vtkSmartPointer<vtkTriangle> face = vtkSmartPointer<vtkTriangle>::New();
    
            for(int j = 0; j < 3; j++) {
                vtkIdType pointId;
                ss >> pointId;
                face->GetPointIds()->SetId(j, pointId);
            }
            polys->InsertNextCell(face);
        }
    
        polyData->SetPoints(points);
        polyData->SetPolys(polys);
        
    return make_shared<Shape>(id, name, polyData, renderer, matrix);
}

