
#include "ShapeInfoContextMenuItem.h"

///////////////////////////////////////////////////////////////////////////////
void custom::contextMenuItems::ShapeInfoContextMenuItem::onClick(
                                                vtkIdType pointId,
                                                vtkIdType faceId,
                                                QWidget* parent)
{

    QDialog* dialog = new QDialog(parent, 0);
    Ui_ShapeInfoDialog ui;
    ui.setupUi(dialog);
    ui.tableWidgetInfo->setRowCount(5);
    ui.tableWidgetInfo->setColumnCount(1);
    
    
    ui.tableWidgetInfo->horizontalHeader()->hide();
    
    QStringList tableVerticalHeader;
    tableVerticalHeader<<"ID"<<"Name"<<"#Vertices"<<"#Faces"<<"Area";
    ui.tableWidgetInfo->setVerticalHeaderLabels(tableVerticalHeader);
    
    //insert data
    
    // shape id
    ui.tableWidgetInfo->setItem(0, 0, new QTableWidgetItem(QString::number(shape_->getId())));
    // shape name
    ui.tableWidgetInfo->setItem(1, 0, new QTableWidgetItem(QString::fromStdString(shape_->getName())));
    // number of vertices
    ui.tableWidgetInfo->setItem(2, 0, new QTableWidgetItem(
                                                    QString::number(
                                                                    (int) shape_->getPolyData()->GetPoints()->GetNumberOfPoints()
                                                                    )
                                                    ));
    // number of cells
    ui.tableWidgetInfo->setItem(3, 0, new QTableWidgetItem(
                                                    QString::number(
                                                                    (int) shape_->getPolyData()->GetNumberOfCells()
                                                                    )
                                                    )
                         );
    // area of the shape
    ui.tableWidgetInfo->setItem(4, 0, new QTableWidgetItem(
                                                    QString::number(
                                                                    (double) shape_->getArea()
                                                                    )
                                                    )
                         );
    
    dialog->show();
}
