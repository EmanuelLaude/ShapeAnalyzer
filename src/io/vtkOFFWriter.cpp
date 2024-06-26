#include "vtkOFFWriter.h"

namespace io {
vtkStandardNewMacro(vtkOFFWriter);
}

///////////////////////////////////////////////////////////////////////////////
io::vtkOFFWriter::vtkOFFWriter() {
    std::ofstream fout; // only used to extract the default precision
    this->DecimalPrecision = fout.precision();
    this->FileName = NULL;
}


///////////////////////////////////////////////////////////////////////////////
io::vtkOFFWriter::~vtkOFFWriter() {}

void io::vtkOFFWriter::WriteData() {
    vtkPolyData *input = this->GetInput();
    if (!input)
        return;
    
    if (!this->FileName) {
        vtkErrorMacro(<< "No FileName specified! Can't write!");
        this->SetErrorCode(vtkErrorCode::NoFileNameError);
        return;
    }
    
    vtkDebugMacro(<<"Opening vtk file for writing...");
    ostream *outfilep = new ofstream(this->FileName, ios::out);
    if (outfilep->fail()) {
        vtkErrorMacro(<< "Unable to open file: "<< this->FileName);
        this->SetErrorCode(vtkErrorCode::CannotOpenFileError);
        delete outfilep;
        return;
    }
    
    std::ostream& outfile = *outfilep;
    
    outfile << "OFF"<<endl;
    outfile << input->GetNumberOfPoints() <<" "<<input->GetNumberOfCells()<< " " << "0"<<endl;
    
    for(vtkIdType i = 0; i < input->GetNumberOfPoints(); i++) {
        double p[3];
        input->GetPoints()->GetPoint(i, p);
        outfile << p[0] <<" "<< p[1]<<" " << p[2]<<endl;
    }
    
    
    for(vtkIdType i = 0; i < input->GetNumberOfCells(); i++) {
        vtkCell* cell = input->GetCell(i);
        outfile << cell->GetNumberOfPoints();
        for(vtkIdType j = 0; j < cell->GetNumberOfPoints(); j++) {
            outfile <<" "<<cell->GetPointId(j);
        }
        outfile <<endl;
    }
    
    
    vtkDebugMacro(<<"Closing vtk file\n");
    delete outfilep;
    
    // Delete the file if an error occurred
    if (this->ErrorCode == vtkErrorCode::OutOfDiskSpaceError) {
        vtkErrorMacro("Ran out of disk space; deleting file: " << this->FileName);
        std::remove(this->FileName);
    }
}


///////////////////////////////////////////////////////////////////////////////
void io::vtkOFFWriter::PrintSelf(ostream& os, vtkIndent indent) {
    Superclass::PrintSelf(os, indent);
    os << indent << "DecimalPrecision: " << DecimalPrecision << "\n";
}


///////////////////////////////////////////////////////////////////////////////
int io::vtkOFFWriter::FillInputPortInformation(int, vtkInformation *info) {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    return 1;
}


///////////////////////////////////////////////////////////////////////////////
vtkPolyData* io::vtkOFFWriter::GetInput() {
    return vtkPolyData::SafeDownCast(this->Superclass::GetInput());
}


///////////////////////////////////////////////////////////////////////////////
vtkPolyData* io::vtkOFFWriter::GetInput(int port) {
    return vtkPolyData::SafeDownCast(this->Superclass::GetInput(port));
}