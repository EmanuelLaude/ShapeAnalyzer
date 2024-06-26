///
/// \brief Main class containt the main function. Starts the ShapeAnalyzer-App.
///

#include <exception>

#include <QApplication>
#include <QMessageBox>

#include "ShapeAnalyzer.h"

class MyApplication : public QApplication {
public:
    MyApplication(int argc, char** argv) : QApplication(argc, argv) {}
private:
    bool notify(QObject *receiver, QEvent *event) {
        try {
            return QApplication::notify(receiver, event);
        } catch (std::exception &e) {
            QMessageBox::critical(nullptr, "Exception", tr("Error sending event ") + typeid(*event).name() + " to object " + qPrintable(receiver->objectName() + " (" + typeid(*receiver).name() + ")\nAn exception of type \"" + typeid(e).name() + "\" has been thrown: " + e.what()));
            exit(EXIT_FAILURE);
        } catch (...) {
            QMessageBox::critical(nullptr, "Exception", tr("Error <unknown> sending event ") + typeid(*event).name() + " to object " + qPrintable(receiver->objectName() + " (" + typeid(*receiver).name() + ")"));
            exit(EXIT_FAILURE);
        }
        
        return false;
    }
};

int main( int argc, char** argv ) {
    // QT Stuff
    try {
        MyApplication app(argc, argv);
        
        ShapeAnalyzer shapeAnalyzer;
        shapeAnalyzer.show();
        return app.exec();
    } catch (std::exception &e) {
        //qFatal(qPrintable(QString("An exception of type \"") + typeid(e).name() + QString("\" has been thrown: ") + e.what()));
        exit(EXIT_FAILURE);
    } catch(...) {
        qFatal("Error <unknown> has occured\n");
        exit(EXIT_FAILURE);
    }
}



