#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileSystemModel>
#include <tchar.h>
#include <QStack>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_listView_doubleClicked(const QModelIndex &index);

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_pushButtonBack_clicked();

    void on_listView_customContextMenuRequested(const QPoint &pos);

    void actionCopyTriggered();
    void actionPasteTriggered();
    void actionCutTriggered();

    void on_listView_clicked(const QModelIndex &index);

    void on_listView_activated(const QModelIndex &index);

    void on_pushButtonPaste_clicked();

private:
    void GetDrives();
    QString tcharToString(const TCHAR* tcharString);
    void loadItemsOnListView(const QString &path);
    bool checkFileIsDir(QString path);
    bool copyFile(const QString &sourceFilePath, const QString &destinationFilePath);
    bool cutFile(const QString &sourceFilePath, const QString &destinationFilePath);
    Ui::Widget *ui;
    QFileSystemModel* model;
    QStack<QString> mStack;
    QString selectedItemPath = "";
    QString action = "";
};
#endif // WIDGET_H
