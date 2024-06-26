#include "vtkToscaReader.h"

namespace io {
vtkStandardNewMacro(vtkToscaReader);
}


///////////////////////////////////////////////////////////////////////////////
// Description:
// Instantiate object with NULL filename.
io::vtkToscaReader::vtkToscaReader() {
    this->FileName = NULL;
    this->SetNumberOfInputPorts(0);
    this->SetNumberOfOutputPorts(1);
}


///////////////////////////////////////////////////////////////////////////////
io::vtkToscaReader::~vtkToscaReader() {
}


///////////////////////////////////////////////////////////////////////////////
int io::vtkToscaReader::FillOutputPortInformation(int port, vtkInformation* info) {
    if (port == 0) {
        info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
        return 1;
    }
    
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
int io::vtkToscaReader::RequestData(vtkInformation *vtkNotUsed(request),
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
    
    std::ifstream vert(this->FileName);
    if (!vert) {
        vtkErrorMacro(<< "Vert file " << this->FileName << " not found");
        return 0;
    }
    vtkDebugMacro(<< "Reading vert file");

    // Regular Expressions are broken in gcc until 4.9
    //  this is the prettier solution though, maybe change it again when 4.9 is updated in the lab
    // and in the writer too
    //std::regex reg(".vert$", std::regex_constants::ECMAScript | std::regex_constants::icase);
    //std::string fileNameTri = std::regex_replace(std::string(FileName), reg, std::string(".tri"));
    std::string fileNameTri = std::string(this->FileName);
    fileNameTri.erase(fileNameTri.find_last_of("."), std::string::npos);
    fileNameTri.append(".tri");
    
    std::ifstream tri(fileNameTri);
    if (!tri) {
        vtkErrorMacro(<< "Tri file " << fileNameTri << " not found");
        return 0;
    }
    vtkDebugMacro(<< "Reading tri file");
    

    // intialise some structures to store the file contents in
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    std::string line;
    while(getline(vert, line)) {
        std::stringstream ss;
        ss << line;
        float x, y, z;
        ss >> x >> y >> z;
        
        points->InsertNextPoint(x, y, z);
    } // (end of vertex while loop)
    
    //read faces
    while(getline(tri, line)) {
        std::stringstream ss;
        ss << line;
        
        vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
        
        for(int i = 0; i < 3; i++) {
            vtkIdType vertId;
            ss >> vertId;
            triangle->GetPointIds()->SetId(i, vertId-1); //substract 1 from vertID. This is the TOSCA standart.
        }
        
        polys->InsertNextCell(triangle);
        
    }
    
    // we have finished with the files
    vert.close();
    tri.close();
    
    output->SetPoints(points);
    output->SetPolys(polys);
    
    return 1;
}


///////////////////////////////////////////////////////////////////////////////
void io::vtkToscaReader::PrintSelf(ostream& os, vtkIndent indent) {
    this->Superclass::PrintSelf(os,indent);
    os << indent << "File Name: " << (this->FileName ? this->FileName : "(none)") << "\n";
}
