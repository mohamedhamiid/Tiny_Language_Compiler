#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <string>
#include <iostream>
#include <QApplication>
#include <QMessageBox>
#include <QMessageBox>
#include <QString>
#include <QPair>
#include <regex>

#include "mainwindow.h"
#include "scanner.h"
#include "Parser.h"


using namespace std;

extern QString Types[];
extern vector<pair<token,int>> allTokens;
extern int tokenIndex;
extern char error ;
vector<pair<int,bool>> lastX;

int padding = 10;

string label[]={
    "if", "repeat", "assign\n", "read\n", "write",
    "op\n", "const\n", "id\n", "STMT_SEQ_NODE"
};

string op[16];

void drawTree(QGraphicsScene* scene, Tree* node, int x, int y, int xOffset, int yStep, int childXoffset,int level) {
    if (!node) return;

    if(level&&lastX[level].second){
        lastX[level].second = 0;
        lastX[level].first = lastX[level-1].first;
    }
    // Calculate node dimensions based on text length
    int textWidth = label[node->node_kind].size() * 8; // Approximate width per character
    //int nodeWidth = textWidth + 20; // Add padding
    int nodeWidth = 100;
    int nodeHeight = 40;

    // Draw the current node
    if(node->node_kind == OPER_NODE || node->node_kind == NUM_NODE || node->node_kind == ID_NODE)
        QGraphicsEllipseItem* ellipse = scene->addEllipse(x, y, nodeWidth, nodeHeight);
    else
        QGraphicsRectItem* rect = scene->addRect(x, y, nodeWidth, nodeHeight);

    QGraphicsTextItem* text;

    string s;
    if(node->node_kind == NUM_NODE){
        s = label[node->node_kind];
        s.push_back('(');
        s = s + to_string(node->num);
        s.push_back(')');

    }
    else if(node->node_kind == IF_NODE || node->node_kind == REPEAT_NODE ||node->node_kind == WRITE_NODE){
        s = label[node->node_kind];
    }
    else if(node->node_kind == READ_NODE){
        s = label[node->node_kind];
        s.append((node->child[0]->id));
    }
    else if(node->node_kind == OPER_NODE){
        op[10] = "<";
        op[11] = "=";
        op[12] = "+";
        op[13] = "-";
        op[14] = "*";
        op[15] = "/";
        s = label[node->node_kind];
        s.push_back('(');
        s.append(op[node->oper]);
        s.push_back(')');
    }
    else{
        s = label[node->node_kind];
        s.push_back('(');
        s.append((node->id));
        s.push_back(')');
    }
    //cout<< s<<endl;
    text = scene->addText(QString::fromStdString(s));
    text->setPos(x + (nodeWidth - textWidth) / 2, y +1);

    lastX[level].first = x + nodeWidth + padding;

    // Draw children
    // Configure dashed line pen
    QPen dashedPen(Qt::DashLine);

    if(node->node_kind != READ_NODE){
        for (int i = 0; i < 3; ++i) {
            if (node->child[i]) {

                if(level<lastX.size()-1&&lastX[level+1].second){
                    lastX[level+1].second = 0;
                    lastX[level+1].first = lastX[level].first-200;
                }

                int childX = lastX[level+1].first; // Start slightly to the left if there are children
                cout<< "Last Value at Level "<<level<<" "<<lastX[level].first<<" "<< label[node->node_kind]<< " " << x<<endl ;

                if(i&&(node->child[i-1]->sibling!=NULL)){
                    childX = childX + xOffset - 150;
                }

                int childY = y + yStep;
                int childWidth = label[node->child[i]->node_kind].size() * 8 + 20; // Calculate child's width
                QGraphicsLineItem* line = scene->addLine(x + nodeWidth / 2, y + nodeHeight,
                                                         childX + childWidth / 2, childY+1);

                if(i==2 && node->node_kind == IF_NODE)
                    line->setPen(dashedPen); // Set dashed line style

                drawTree(scene, node->child[i], childX, childY, xOffset, yStep,childXoffset,level+1);
                //childX += childXoffset * 4; // Adjusted horizontal spacing between children
                childX = lastX[level].first;
            }
        }
    }

    // Draw sibling
    if (node->sibling) {
        int siblingX = x + xOffset * 1; // Increased space between siblings
        QGraphicsLineItem* line = scene->addLine(x + nodeWidth, y + nodeHeight / 2,
                                                 siblingX, y + nodeHeight / 2);
        drawTree(scene, node->sibling, siblingX, y, xOffset, yStep,childXoffset,level);
    }
}
int getTreeDepth(Tree* node) {
    if (!node) return 0;

    int maxChildDepth = 0;
    for (int i = 0; i < 3; ++i) {
        if (node->child[i]) {
            maxChildDepth = std::max(maxChildDepth, getTreeDepth(node->child[i]));
        }
    }

    int siblingDepth = getTreeDepth(node->sibling);

    return std::max(maxChildDepth + 1, siblingDepth);
}
int countDigits(int number) {
    if (number == 0) return 1; // Special case for 0
    return std::log10(std::abs(number)) + 1;
}
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    fileContentDisplay = new QTextEdit(this);
    fileContentDisplay->setReadOnly(true);

    // Create radio buttons
    codeInputRadioButton = new QRadioButton("Code", this);
    tokensInputRadioButton = new QRadioButton("Tokens", this);
    codeInputRadioButton->setStyleSheet(darkThemeStyleSheet());
    tokensInputRadioButton->setStyleSheet(darkThemeStyleSheet());
    codeInputRadioButton->setChecked(true); // Set default selection to "Code"

    // Group the radio buttons
    QButtonGroup *inputTypeGroup = new QButtonGroup(this);
    inputTypeGroup->addButton(codeInputRadioButton);
    inputTypeGroup->addButton(tokensInputRadioButton);

    // Create buttons
    browseButton = new QPushButton("Browse File", this);
    scannerButton = new QPushButton("Run Scanner", this);
    scannerButton->setEnabled(false); // Initially disabled until file is loaded
    parserButton = new QPushButton("Run Parser", this);
    parserButton->setEnabled(false); // Initially disabled until file is loaded

    // Add widgets to layout
    layout->addWidget(fileContentDisplay);

    QHBoxLayout *radioLayout = new QHBoxLayout();
    radioLayout->addWidget(codeInputRadioButton);
    radioLayout->addWidget(tokensInputRadioButton);
    layout->addLayout(radioLayout);

    layout->addWidget(browseButton);
    layout->addWidget(scannerButton);
    layout->addWidget(parserButton);

    // Set the title of the main window
    setWindowTitle("Tiny Compiler");

    // Set the initial size of the window
    resize(800, 600); // Adjust the size as needed

    setCentralWidget(centralWidget);

    // Connect signals and slots
    connect(browseButton, &QPushButton::clicked, this, &MainWindow::browseFile);
    connect(scannerButton, &QPushButton::clicked, this, &MainWindow::runScanner);
    connect(parserButton, &QPushButton::clicked, this, &MainWindow::runParser);


    // Disable scanner button when "Tokens" is selected
    connect(tokensInputRadioButton, &QRadioButton::toggled, this, [this](bool checked) {
        scannerButton->setEnabled(!checked); // Disable if "Tokens" is selected
    });
}


MainWindow::~MainWindow() {}

void MainWindow::toggleInputType() {
    if (codeInputRadioButton->isChecked()) {
        scannerButton->setEnabled(true); // Enable scanner button for "Code" input
    } else if (tokensInputRadioButton->isChecked()) {
        scannerButton->setEnabled(false); // Disable scanner button for "Tokens" input
    }
}

void MainWindow::browseFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Text File", lastDirectory, "Text Files (*.txt)");
    if (!fileName.isEmpty())
    {
        // Update the last directory path
        QFileInfo fileInfo(fileName);
        lastDirectory = fileInfo.absolutePath();

        currentFilePath = fileName;
        QFile file(fileName);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            fileContent = file.readAll();

            // Reset content
            fileContentwithNumbering.clear();
            fileContentwithNumbering = fileContent;

            int lines=1;
            for(int i=0;i<fileContentwithNumbering.length();i++){

                if(!i || fileContentwithNumbering[i-1]=='\n'){
                    string s = to_string(lines)+". ";
                    fileContentwithNumbering.insert(i,s);
                    lines++;
                }

            }


            fileContentDisplay->setText(fileContentwithNumbering);
            scannerButton->setEnabled(true);
            parserButton->setEnabled(true);
        }
    }
}
void MainWindow::runScanner()
{
    if (fileContent.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please load a file first!");
        return;
    }
    std::ostringstream scannerOutput;
    std::streambuf *originalCoutBuffer = std::cout.rdbuf(scannerOutput.rdbuf()); // Redirect std::cout

    std::vector<pair<token,int>> tokens = scanner(fileContent.toStdString());

    std::cout.rdbuf(originalCoutBuffer); // Restore std::cout

    QString errorOutput = QString::fromStdString(scannerOutput.str());
    QDialog *errorDialog = nullptr;

    if (!errorOutput.isEmpty())
    {
        errorDialog = new QDialog(this);
        errorDialog->setWindowTitle("Scanner Errors");

        QVBoxLayout *errorLayout = new QVBoxLayout(errorDialog);

        QLabel *errorLabel = new QLabel(errorDialog);
        errorLabel->setText(errorOutput);
        errorLabel->setWordWrap(true);

        QScrollArea *errorScrollArea = new QScrollArea(errorDialog);
        errorScrollArea->setWidget(errorLabel);
        errorScrollArea->setWidgetResizable(true);

        errorLayout->addWidget(errorScrollArea);
        errorDialog->resize(400, 300);

        QPushButton *closeButton = new QPushButton("Close", errorDialog);
        connect(closeButton, &QPushButton::clicked, errorDialog, &QDialog::accept);
        errorLayout->addWidget(closeButton);

        errorDialog->move(100, 100); // Set position for the error dialog
        errorDialog->show();

        // Apply dark theme for the error dialog
        errorDialog->setStyleSheet(darkThemeStyleSheet());
    }

    QDialog *outputDialog = new QDialog(this);
    outputDialog->setWindowTitle("Scanner Output");

    QVBoxLayout *outputLayout = new QVBoxLayout(outputDialog);
    QLabel *outputLabel = new QLabel(outputDialog);
    outputLabel->setWordWrap(true);

    QString outputText;

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        outputText += QString("%1  ,   %2\n")
                          .arg(QString::fromStdString(tokens[i].first.value))
                          .arg(Types[tokens[i].first.type]);
    }

    outputLabel->setText(outputText);

    QScrollArea *outputScrollArea = new QScrollArea(outputDialog);
    outputScrollArea->setWidget(outputLabel);
    outputScrollArea->setWidgetResizable(true);

    QPushButton *saveButton = new QPushButton("Save", outputDialog);
    connect(saveButton, &QPushButton::clicked, [outputText, this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Scanner Output", lastDirectory + "/scanner_output.txt", "Text Files (*.txt)");
        if (!fileName.isEmpty())
        {
            // Update the last directory path
            QFileInfo fileInfo(fileName);
            lastDirectory = fileInfo.absolutePath();

            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&file);
                out << outputText;
                file.close();

                // Show a success message
                QMessageBox::information(this, "Success", "Scanner output saved successfully!");
            }
            else
            {
                QMessageBox::warning(this, "Error", "Failed to save the scanner output.");
            }
        }
    });


    outputLayout->addWidget(outputScrollArea);
    outputLayout->addWidget(saveButton);
    outputDialog->resize(400, 300);
    outputDialog->move(550, 100); // Set position for the scanner output dialog
    outputDialog->show();

    // Apply dark theme for the output dialog
    outputDialog->setStyleSheet(darkThemeStyleSheet());

    // Optional: If there are no errors, notify the user in the error dialog.
    if (errorDialog == nullptr)
    {
        errorDialog = new QDialog(this);
        errorDialog->setWindowTitle("Scanner Errors");

        QVBoxLayout *errorLayout = new QVBoxLayout(errorDialog);
        QLabel *noErrorsLabel = new QLabel("No errors found!", errorDialog);
        errorLayout->addWidget(noErrorsLabel);

        QPushButton *closeButton = new QPushButton("Close", errorDialog);
        connect(closeButton, &QPushButton::clicked, errorDialog, &QDialog::accept);
        errorLayout->addWidget(closeButton);

        errorDialog->resize(300, 150);
        errorDialog->move(100, 100); // Set position for the error dialog
        errorDialog->show();

        // Apply dark theme for the error dialog
        errorDialog->setStyleSheet(darkThemeStyleSheet());
    }
}

// Function to map string to tokenType
tokenType getTokenType(const QString &typeStr)
{
    for (int i = 0; i < 20; i++)
    {
        if (Types[i] == typeStr)
        {
            return static_cast<tokenType>(i);
        }
    }
    throw invalid_argument("Invalid token type string");
}

// Assuming runParser is part of MainWindow class
void MainWindow::runParser() {
    if (fileContent.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please load a file first!");
        return;
    }
    try {
        allTokens.clear();
        if (tokensInputRadioButton != nullptr && tokensInputRadioButton->isChecked()) {
            QStringList lines = fileContent.split("\n");  // Split the input into lines
            int lineNum = 1;

            // Create a regex to match and remove all whitespace characters
            std::regex whitespace_regex("\\s+");

            // Parse each line
            for (const QString &line : lines)
            {
                // Remove all whitespaces from the line using std::regex_replace
                std::string cleanedLine = std::regex_replace(line.toStdString(), whitespace_regex, "");

                // Skip empty lines after removing spaces
                if (cleanedLine.empty())
                {
                    continue;
                }

                // Convert cleaned line back to QString for further processing
                QString cleanedQStringLine = QString::fromStdString(cleanedLine);

                // Split the cleaned line by comma
                QStringList parts = cleanedQStringLine.split(",");
                if (parts.size() != 2)
                {
                    continue;  // Skip malformed lines
                }

                // Token value and type without extra spaces
                QString tokenValue = parts[0];  // First part is the token value
                QString typeStr = parts[1];     // Second part is the token type

                // Create the token object
                tokenType type = getTokenType(typeStr);  // Get token type from string
                token tok = {type, tokenValue.toStdString()};

                // Add the pair (token, line number) to the vector
                allTokens.push_back(QPair<token, int>(tok, lineNum));

                // Increment the line number
                lineNum++;
            }



            } else if (codeInputRadioButton != nullptr && codeInputRadioButton->isChecked()) {
                // Parse the tokens starting from the first statement sequence
                allTokens = scanner(fileContent.toStdString());
            }

        if(error){
            QMessageBox::warning(this, "Error", "Error in scanner please run scanner to know errors!");
            return;
        }
        lastX.clear();
        tokenIndex = 0;
        Tree* syntaxTree = stmt_seq();


        if(tokenIndex<allTokens.size())
            handleError("Unexpected token : " + allTokens[tokenIndex].first.value);


        // Set up the scene and view
        QGraphicsScene *scene = new QGraphicsScene(this);
        int startX = 400; // Initial x position
        int startY = 50;  // Initial y position
        int xOffset = 200; // Horizontal distance between nodes
        int yStep = 150;  // Vertical distance between levels
        int childXoffset =25;

        int d = getTreeDepth(syntaxTree);
        for (int i = 0; i < d; ++i) {
            lastX.push_back(make_pair(startX,1));
        }

        drawTree(scene, syntaxTree, startX, startY, xOffset, yStep ,childXoffset,0);

        QDialog *treeDialog = new QDialog(this);
        treeDialog->setWindowTitle("Syntax Tree Viewer");

        QVBoxLayout *layout = new QVBoxLayout(treeDialog);
        QGraphicsView *view = new QGraphicsView(scene, treeDialog);
        view->setRenderHint(QPainter::Antialiasing);
        layout->addWidget(view);

        treeDialog->resize(1024, 720);
        treeDialog->exec();

    } catch (const std::exception& e) {
        // Display the error in a Qt Message Box
        QMessageBox errorBox;
        errorBox.setIcon(QMessageBox::Critical);
        errorBox.setWindowTitle("Parser Error");
        errorBox.setText("A syntax error occurred while parsing.");
        errorBox.setInformativeText(QString::fromStdString(e.what()));
        errorBox.exec();

        // Optionally log the error or perform additional actions
    } catch (...) {
        // Catch any other types of errors
        QMessageBox errorBox;
        errorBox.setIcon(QMessageBox::Critical);
        errorBox.setWindowTitle("Parser Error");
        errorBox.setText("An unknown error occurred while parsing.");
        errorBox.exec();
    }
}
