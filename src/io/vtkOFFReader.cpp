#include "vtkOFFReader.h"

namespace io {
vtkStandardNewMacro(vtkOFFReader);
}
    

///////////////////////////////////////////////////////////////////////////////
// Description:
// Instantiate object with NULL filename.
io::vtkOFFReader::vtkOFFReader() {
    this->FileName = NULL;
    this->SetNumberOfInputPorts(0);
    this->SetNumberOfOutputPorts(1);
}


///////////////////////////////////////////////////////////////////////////////
io::vtkOFFReader::~vtkOFFReader() {
}

int io::vtkOFFReader::FillOutputPortInformation(int port, vtkInformation* info) {
    if (port == 0) {
        info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData" );
        return 1;
    }
    
    return 0;
}

/*--------------------------------------------------------

This is only partial support for the OFF format, which is 
quite complicated. To find a full specification,
search the net for "OFF format", eg.:

http://people.sc.fsu.edu/~burkardt/data/off/off.html
---------------------------------------------------------*/


///////////////////////////////////////////////////////////////////////////////
int io::vtkOFFReader::RequestData(vtkInformation *vtkNotUsed(request),
                              vtkInformationVector **vtkNotUsed(inputVector),
                              vtkInformationVector *outputVector) {
    // get the info object
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the ouptut
    vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

    if (!this->FileName) {
        vtkErrorMacro(<< "A FileName must be specified.");
        return 0;
    }
    
    std::ifstream in(this->FileName);
    if (!in) {
        vtkErrorMacro(<< "File " << this->FileName << " not found");
        return 0;
    }

    vtkDebugMacro(<< "Reading file");

    // intialise some structures to store the file contents in
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
  
    // -- work through the file line by line, assigning into the above structures as appropriate --
    std::string line;

    getline(in, line); //throw the 1st line away, it always says "OFF"
  
    //Read until we get to the first line that is not a comment or a blank line.
    //This line second states the number of vertices, faces, and edges.
    
    while(getline(in, line)) {
        //std::cout << line << std::endl;
        if(line.size() == 0)
            continue;
        if(line[0] == '#')
            continue;
    
        //if we get to here, this is the info line
        break;
    }
    
    //std::cout << "Found info line." << std::endl;
    //At this point, the line were are interested in is stored in 'line'
    //We are only interested in vertices and faces.
    std::stringstream ss;
  
    ss << line;
    unsigned int numberOfVertices, numberOfFaces;

    ss >> numberOfVertices >> numberOfFaces;
    //std::cout << "Vertices: " << NumberOfVertices << " Faces: " << NumberOfFaces << std::endl;
  
    //Now we can start to read the vertices
    unsigned int vertexCounter = 0;
    
    while(getline(in, line) && vertexCounter < numberOfVertices) {
        if(line.size() == 0) //skip blank lines (they should only occur before the vertices start)
            continue;

        std::stringstream ssVertex;
        ssVertex << line;
        float x, y, z;
        ssVertex >> x >> y >> z;
        
        points->InsertNextPoint(x, y, z);
        //std::cout << "adding vertex: " << x << " " << y << " " << z << std::endl;
        vertexCounter++;
    } // (end of vertex while loop)

    unsigned int faceCounter = 0;
    
    //read faces
    do {
        std::stringstream ssFace;
        ssFace << line;
        unsigned int numFaceVerts;
        ssFace >> numFaceVerts;

    
        vtkIdType cell[256];
    
        for(int i = 0; i < numFaceVerts; i++) {
            unsigned int vert;
            ssFace >> vert;
            cell[i] = vert;
        }
    
        polys->InsertNextCell(numFaceVerts, cell);
    
        faceCounter++;
    } while(getline(in, line) && faceCounter < numberOfFaces);
  
    // we have finished with the file
    in.close();

    output->SetPoints(points);
    output->SetPolys(polys);
  
    return 1;
}


///////////////////////////////////////////////////////////////////////////////
void io::vtkOFFReader::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os,indent);
    os << indent << "File Name: " << (this->FileName ? this->FileName : "(none)") << "\n";
}
