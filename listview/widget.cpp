#include "widget.h"
#include "ui_widget.h"

#include <QListView>
#include <QDir>
#include <windows.h>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QMenu>
#include <QFileInfo>
#include <QFile>
#include <QIODevice>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    model = new QFileSystemModel(this);
    GetDrives();

    // select item is actived, chuot phai hoac chuot trai
    connect(ui->listView->selectionModel(), &QItemSelectionModel::selectionChanged, [&](const QItemSelection &selected, const QItemSelection &deselected) {
        // Get the selected indexes
        QModelIndexList selectedIndexes = selected.indexes();

        // Check if there is any selected item
        if (!selectedIndexes.isEmpty()) {
            // Get the data of the selected item
            QModelIndex selectedIndex = selectedIndexes.first();
            QVariant data = model->data(selectedIndex, Qt::DisplayRole);
            //qDebug() << "Selected item:" << data.toString();
            QString filePath = mStack.top() + "\\" + data.toString();
            qDebug() << "Selected item:" +filePath;
            // neu la file, khong ho tro thu muc cho copy, cut ...
            if(!checkFileIsDir(filePath)){
                selectedItemPath = filePath;
            }
        }
    });
}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_listView_doubleClicked(const QModelIndex &index)
{
    auto path = index.model()->data(index);
    //qDebug() << "Double-clicked: " << path.toString();
    const QString itemText = mStack.top()+"\\"+path.toString();
    ui->labelPath->setText(itemText);
    mStack.push(itemText);
    loadItemsOnListView(itemText);
}

void Widget::GetDrives(){
    // initial value
    TCHAR szDrive[] = _T(" A:");
    DWORD uDriveMask = GetLogicalDrives();
    if(uDriveMask == 0)
        qDebug()<< "\nGetLogicalDrives() failed with failure code: " << QString::number(static_cast<qulonglong>(GetLastError()));
    else
    {
        while(uDriveMask)
        {// use the bitwise AND, 1–available, 0-not available
            if(uDriveMask & 1){
                ui->comboBox->addItem(tcharToString(szDrive).trimmed());
            }
            // increment...
            ++szDrive[1];
            // shift the bitmask binary right
            uDriveMask >>= 1;
        }
    }
}

QString Widget::tcharToString(const TCHAR* tcharString) {
    // Convert TCHAR to wide string (UTF-16) using QString::fromWCharArray
    QString wideString = QString::fromWCharArray(tcharString);

    // Convert wide string to UTF-8 using QString::toUtf8
    QByteArray utf8Data = wideString.toUtf8();

    // Convert UTF-8 to QString using QString::fromUtf8
    QString result = QString::fromUtf8(utf8Data);

    return result;
}

void Widget::on_comboBox_currentTextChanged(const QString &arg1)
{
    qDebug() << arg1; // D:
    ui->labelPath->setText(arg1);
    mStack.clear();
    mStack.push(arg1);
    loadItemsOnListView(arg1);
}

void Widget::loadItemsOnListView(const QString &path){
    model->setRootPath(path);
    ui->listView->setModel(model);
    ui->listView->setRootIndex(model->index(path));
}


void Widget::on_pushButtonBack_clicked()
{
    if(mStack.count() > 1){
        mStack.pop();
        QString prePath = mStack.top();
        ui->labelPath->setText(prePath);
        loadItemsOnListView(prePath);
    }
    else{
        QMessageBox::warning(this, "Warning", "Stack is empty!");
    }
}


void Widget::on_listView_customContextMenuRequested(const QPoint &pos)
{
    QListView *listView = qobject_cast<QListView*>(sender());
    if (!listView)
        return;

    // Get the item index at the clicked position
    QModelIndex index = listView->indexAt(pos);
    if (!index.isValid())
        return;

    // Create a context menu
    QMenu contextMenu(this);

    // Add actions to the context menu
    QAction *actionCopy = new QAction("Copy", this);
    QAction *actionRename = new QAction("Rename", this);
    QAction *actionCut = new QAction("Cut", this);
    QAction *actionPaste = new QAction("Paste", this);

    contextMenu.addAction(actionCopy);
    contextMenu.addAction(actionRename);
    contextMenu.addAction(actionCut);
    contextMenu.addAction(actionPaste);

    connect(actionCopy, &QAction::triggered, this, &Widget::actionCopyTriggered);
    connect(actionCut, &QAction::triggered, this, &Widget::actionCutTriggered);
    connect(actionPaste, &QAction::triggered, this, &Widget::actionPasteTriggered);

    // Show the context menu at the cursor position
    contextMenu.exec(listView->mapToGlobal(pos));
}

void Widget::actionCopyTriggered() {
    action = "copy";
}

void Widget::actionPasteTriggered() {

}

void Widget::actionCutTriggered(){
    action = "cut";
}

void Widget::on_listView_clicked(const QModelIndex &index)
{
    //auto path = index.model()->data(index);
    //qDebug() << "clicked: " << path.toString();
}


void Widget::on_listView_activated(const QModelIndex &index)
{
    //auto path = index.model()->data(index);
    //qDebug() << "activated: " << path.toString();
}

bool Widget::checkFileIsDir(QString path){
    QFileInfo fileInfo(path);
    // Check if the file is a directory
    if (fileInfo.isDir()) {
        qDebug() << "The file is a directory.";
        return true;
    } else {
        qDebug() << "The file is not a directory.";
        return false;
    }
}

bool Widget::copyFile(const QString &sourceFilePath, const QString &destinationFilePath) {
    try {
        // Create a QFile object for the source file
        QFile sourceFile(sourceFilePath);

        // Check if the source file exists and is readable
        if (!sourceFile.exists() || !sourceFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Error: Source file does not exist or cannot be read.";
            return false;
        }

        // Create a QFile object for the destination file
        QFile destinationFile(destinationFilePath);

        // Check if the destination file already exists
        if (destinationFile.exists()) {
            qDebug() << "Error: Destination file already exists.";
            return false;
        }

        // Open the destination file for writing
        if (!destinationFile.open(QIODevice::WriteOnly)) {
            qDebug() << "Error: Unable to open destination file for writing.";
            return false;
        }

        // Read data from the source file and write it to the destination file
        QByteArray data = sourceFile.readAll();
        destinationFile.write(data);

        // Close both files
        sourceFile.close();
        destinationFile.close();

        qDebug() << "File copied successfully.";
        return true;
    } catch (const QString &errorMessage) {
        // Catch the exception and handle it
        qDebug() << "Caught exception:" << errorMessage;
        return false;
    } catch (const std::exception &stdException) {
        // Catch standard C++ exceptions
        qDebug() << "Caught std::exception:" << stdException.what();
        return false;
    } catch (...) {
        // Catch any other unhandled exceptions
        qDebug() << "Caught an unknown exception";
        return false;
    }
}

void Widget::on_pushButtonPaste_clicked()
{
    if(selectedItemPath != "" && action == "copy"){
        auto listTemp = selectedItemPath.split("\\");
        //qDebug() <<"File Name cuoi cung:" <<listTemp.last();
        QString desPath = mStack.top() + "\\" + listTemp.last();
        //qDebug() << "Des path: " <<desPath;
        if(copyFile(selectedItemPath, desPath)){
            selectedItemPath = "";
            action = "";
        }
    }else if(selectedItemPath != "" && action == "cut") {
        auto listTemp = selectedItemPath.split("\\");
        QString desPath = mStack.top() + "\\" + listTemp.last();
        if(cutFile(selectedItemPath, desPath)){
            selectedItemPath = "";
            action = "";
        }
    }
}

bool Widget::cutFile(const QString &sourceFilePath, const QString &destinationFilePath){
    try {
        // Check if the source file exists
        if (QFile::exists(sourceFilePath)) {
            // Create a QFile object for the source file
            QFile sourceFile(sourceFilePath);

            // Check if the file can be opened for reading
            if (sourceFile.open(QIODevice::ReadOnly)) {
                // Create a QFile object for the destination file
                QFile destinationFile(destinationFilePath);

                // Check if the file can be opened for writing
                if (destinationFile.open(QIODevice::WriteOnly)) {
                    // Copy the contents of the source file to the destination file
                    destinationFile.write(sourceFile.readAll());

                    // Close both files
                    sourceFile.close();
                    destinationFile.close();

                    // Remove the source file
                    if (QFile::remove(sourceFilePath)) {
                        qDebug() << "File cut successfully.";
                        return true;
                    } else {
                        qDebug() << "Error removing the source file.";
                        return false;
                    }
                } else {
                    qDebug() << "Error opening the destination file for writing.";
                    return false;
                }
            } else {
                qDebug() << "Error opening the source file for reading.";
                return false;
            }
        } else {
            qDebug() << "Source file does not exist.";
            return false;
        }
    } catch (const QString &errorMessage) {
        // Catch the exception and handle it
        qDebug() << "Caught exception:" << errorMessage;
        return false;
    } catch (const std::exception &stdException) {
        // Catch standard C++ exceptions
        qDebug() << "Caught std::exception:" << stdException.what();
        return false;
    } catch (...) {
        // Catch any other unhandled exceptions
        qDebug() << "Caught an unknown exception";
        return false;
    }
}
