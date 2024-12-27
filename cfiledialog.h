#ifndef CFILEDIALOG_H
#define CFILEDIALOG_H

#include <QDialog>

namespace Ui {
class CFileDialog;
}

class CFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CFileDialog(QWidget *parent = nullptr);
    ~CFileDialog();

    QString getFilePath();
    QString getFileName();
    QString getFileDirectory();

private:
    Ui::CFileDialog *ui;
    QString m_filePath;
    QString m_fileName;
    QString m_directory;
};

#endif // CFILEDIALOG_H
