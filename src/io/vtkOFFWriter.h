#ifndef __ShapeAnalyzer__vtkOFFWriter__
#define __ShapeAnalyzer__vtkOFFWriter__

#include <vtkObjectFactory.h>
#include <vtkWriter.h>
#include <vtkErrorCode.h>
#include <vtkPolyData.h>
#include <vtkCell.h>
#include <vtkInformation.h>

#include <cstdio>
#include <cstdlib>

namespace io {

/// \brief Vtk class to write shapes into .off format.
class vtkOFFWriter : public vtkWriter {
public:
    static vtkOFFWriter *New();
    vtkTypeMacro(vtkOFFWriter, vtkWriter)
    void PrintSelf(ostream& os, vtkIndent indent);
    
    vtkGetMacro(DecimalPrecision, int)
    vtkSetMacro(DecimalPrecision, int)
    
    // Description:
    // Specify file name of data file to write.
    vtkSetStringMacro(FileName)
    vtkGetStringMacro(FileName)
    
    // Description:
    // Get the input to this writer.
    vtkPolyData* GetInput();
    vtkPolyData* GetInput(int port);
    
protected:
    vtkOFFWriter();
    ~vtkOFFWriter();
    
    void WriteData();
    int FillInputPortInformation(int port, vtkInformation *info);
    
    int DecimalPrecision;
    char *FileName;
    
private:
    vtkOFFWriter(const vtkOFFWriter&);  // Not implemented.
    void operator=(const vtkOFFWriter&);  // Not implemented.
};

}


#endif /* defined(__ShapeAnalyzer__vtkOFFWriter__) */
