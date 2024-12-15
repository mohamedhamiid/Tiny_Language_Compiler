#include "MainWindow.h"
#include "Parser.h"
#include <QApplication>
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QFont>
#include <fstream>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <iostream>


#include "TreeVisualizer.h"
#include "Parser.h"

using namespace std;

QString darkThemeStyleSheet() {
    return R"(
        QMainWindow {
            background-color: #2B2B2B;
            color: #CCCCCC;
        }
        QLabel {
            color: #FFFFFF;
            background-color: transparent; /* Make label background transparent */
        }
        QPushButton {
            background-color: #3C3F41;
            color: #FFFFFF;
            border: 1px solid #565656;
            border-radius: 4px;
            padding: 5px;
        }
        QPushButton:hover {
            background-color: #45494A;
        }
        QPushButton:pressed {
            background-color: #292C2D;
        }
        QScrollArea {
            background-color: #2B2B2B;
            border: 1px solid #565656;
        }
        QDialog {
            background-color: #2B2B2B;
            color: #CCCCCC;
        }
        QTextEdit, QLineEdit {
            background-color: #3C3F41;
            color: #FFFFFF;
            border: 1px solid #565656;
            border-radius: 4px;
        }
        QScrollArea > QWidget {
            background-color: #3C3F41; /* Ensure scroll area's widget has dark background */
        }
        QLabel {
            color: #FFFFFF; /* Make sure labels inside dialogs have white text */
        }
        QRadioButton {
            color: #D4D4D4; /* Light gray label for readability */
            font-size: 14px;
            font-family: Arial, sans-serif;
        }
        QRadioButton::indicator {
            width: 15px;
            height: 15px;
            border: 2px solid #5A5A5A; /* Neutral gray border */
            border-radius: 7px;
            background-color: #3C3F41; /* Dark gray background */
        }
        QRadioButton::indicator:checked {
            background-color: #4C8DF5; /* Bright blue for the checked state */
            border: 2px solid #4C8DF5;
        }

    )";
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Apply the dark theme
    app.setStyleSheet(darkThemeStyleSheet());

    MainWindow w;
    w.show();

    return app.exec();
}
