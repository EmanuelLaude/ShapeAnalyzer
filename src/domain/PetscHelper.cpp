#include "PetscHelper.h"

///////////////////////////////////////////////////////////////////////////////
void PetscHelper::setRow(Mat &A, Vec &ai, PetscInt i) {
    PetscInt size;
    VecGetSize(ai, &size);
    PetscInt* colIdx = new PetscInt[size];
    
    PetscScalar* row;
    VecGetArray(ai, &row);
    
    for(PetscInt j = 0; j < size; j++) {
        colIdx[j] = j;
    }
    
    
    MatSetValues(A, 1, &i, size, colIdx, row, INSERT_VALUES);

    VecRestoreArray(ai, &row);
    delete [] colIdx;
}


///////////////////////////////////////////////////////////////////////////////
void PetscHelper::getRow(Vec& ai, Mat& A, PetscInt i) {
    PetscInt m;
    MatGetSize(A, NULL, &m);

    
    const PetscScalar* row;
    MatGetRow(A, i, NULL, NULL, &row);
    PetscInt* idx = new PetscInt[m];
    
    for(PetscInt j = 0; j < m; j++) {
        idx[j] = j;
    }

    VecSetValues(ai, m, idx, row, INSERT_VALUES);
    VecAssemblyBegin(ai);
    VecAssemblyEnd(ai);
    
    MatRestoreRow(A, i, NULL, NULL, &row);
    delete [] idx;
}


///////////////////////////////////////////////////////////////////////////////
void PetscHelper::setColumn(Mat &A, Vec &ai, PetscInt i) {
    PetscInt size;
    VecGetSize(ai, &size);
    PetscInt* rowIdx = new PetscInt[size];
    
    PetscScalar* column;
    VecGetArray(ai, &column);
    
    for(PetscInt j = 0; j < size; j++) {
        rowIdx[j] = j;
    }
    
    MatSetValues(A, size, rowIdx, 1, &i, column, INSERT_VALUES);
    VecRestoreArray(ai, &column);
    delete [] rowIdx;
}


///////////////////////////////////////////////////////////////////////////////
void PetscHelper::setBlock(Mat &A, Mat &B, PetscInt i, PetscInt j) {
    PetscInt m;
    PetscInt n;
    MatGetSize(B, &m, &n);
    
    PetscScalar* values = new PetscScalar[m*n];
    
    PetscInt* rowIdx = new PetscInt[m];
    for(PetscInt l = 0; l < m; l++) {
        rowIdx[l] = l;
    }
    
    PetscInt* colIdx = new PetscInt[n];
    for(PetscInt k = 0; k < n; k++) {
        colIdx[k] = k;
    }
    
    MatGetValues(B, m, rowIdx, n, colIdx, values);
    
    for(PetscInt l = 0; l < m; l++) {
        rowIdx[l] += i;
    }

    for(PetscInt m = 0; m < n; m++) {
        colIdx[m] += j;
    }
    
    MatSetValues(A, m, rowIdx, n, colIdx, values, INSERT_VALUES);

    delete [] values;
    delete [] colIdx;
    delete [] rowIdx;
}


///////////////////////////////////////////////////////////////////////////////
void PetscHelper::reshape(Mat &B, Vec &b, PetscInt m, PetscInt n) {
    PetscInt* rowIdx = new PetscInt[m];
    for(PetscInt l = 0; l < m; l++) {
        rowIdx[l] = l;
    }
    
    PetscInt* colIdx = new PetscInt[n];
    for(PetscInt k = 0; k < n; k++) {
        colIdx[k] = k;
    }
    PetscScalar* values;
    VecGetArray(b, &values);
    MatSetValues(B, m, rowIdx, n, colIdx, values, INSERT_VALUES);
    MatAssemblyBegin(B, MAT_FINAL_ASSEMBLY);
    MatAssemblyEnd(B, MAT_FINAL_ASSEMBLY);
    
    VecRestoreArray(b, &values);
    delete [] colIdx;
    delete [] rowIdx;
}


///////////////////////////////////////////////////////////////////////////////
void PetscHelper::vtkDataArrayToPetscVec(vtkSmartPointer<vtkDataArray> arr, Vec &vec) {
    PetscInt size = arr->GetNumberOfTuples();
    
    if(arr->GetNumberOfComponents() != 1) {
        throw std::invalid_argument(std::string("Number of components of arr is not equal to 1 in ").append(__PRETTY_FUNCTION__));
    }
    
    for(PetscInt i = 0; i < size; i++) {
        VecSetValue(vec, i, arr->GetComponent(i, 0), INSERT_VALUES);
    }
    
    VecAssemblyBegin(vec);
    VecAssemblyEnd(vec);
}


///////////////////////////////////////////////////////////////////////////////
vtkSmartPointer<vtkDoubleArray> PetscHelper::petscVecToVtkDoubleArray(Vec& vec) {
    PetscScalar* arr;
    VecGetArray(vec, &arr);
    PetscInt size;
    VecGetSize(vec, &size);
    
    vtkSmartPointer<vtkDoubleArray> res = petscScalarArrayToVtkDoubleArray(arr, size);
    VecRestoreArray(vec, &arr);
    return res;
}


///////////////////////////////////////////////////////////////////////////////
vtkSmartPointer<vtkDoubleArray> PetscHelper::petscScalarArrayToVtkDoubleArray(const PetscScalar *array, PetscInt size) {
    vtkSmartPointer<vtkDoubleArray> vtkArr = vtkSmartPointer<vtkDoubleArray>::New();
    vtkArr->SetNumberOfValues(size);
    for(PetscInt j = 0; j < size; j++) {
        vtkArr->SetValue(j, array[j]);
    }
    
    return vtkArr;
}



