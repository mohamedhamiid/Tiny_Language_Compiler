#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDialog>
#include <QLabel>
#include <QScrollArea>
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <sstream>
#include <string>
#include <vector>

#include "LineNumbering.h"
#include "scanner.h"

QString darkThemeStyleSheet();

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void browseFile();
    void runScanner();
    void runParser();
    void toggleInputType();

private:
    void displayErrors(const QString &errors);
    void displayTokens(const std::vector<token> &tokens);

    QPushButton *browseButton;
    QPushButton *scannerButton;
    QPushButton *parserButton;

    QRadioButton *codeInputRadioButton;
    QRadioButton *tokensInputRadioButton;
    QButtonGroup *inputTypeGroup;

    QString currentFilePath;
    QTextEdit *fileContentDisplay; // Custom text editor with line numbers
    QString fileContent;
    QString fileContentwithNumbering;
    QString lastDirectory; // Stores the last accessed directory
};

QString darkThemeStyleSheet();

#endif // MAINWINDOW_H
