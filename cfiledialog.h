#ifndef CFILEDIALOG_H
#define CFILEDIALOG_H

#pragma once
#include <QDialog>
#include <QDir>
#include <QListView>

namespace Ui {
class CFileDialog;
}

class CFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CFileDialog(QWidget *parent = nullptr);
    ~CFileDialog();

    QString filePath();
    QString fileName();
    QString fileDirectory();

private:
    Ui::CFileDialog *ui;
    QString m_filePath;
    QString m_fileName;
    QString m_directory;

    QMap<QString,QIcon> m_fileIcons;

    bool updateFileListView();

    QDir::SortFlags getSortFlags();
    void loadResources();
    void populateListViewWithDirectory(QListView* pListView, QDir directory);
};

#endif // CFILEDIALOG_H
