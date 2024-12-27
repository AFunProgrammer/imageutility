#include "cfiledialog.h"
#include "ui_cfiledialog.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <QStandardPaths>
#include <QStringListModel>


void changeWidgetFontColor(QWidget* pWidget, QColor color)
{
    if ( !pWidget ){
        return;
    }

    QPalette palette = pWidget->palette();
    palette.setColor(QPalette::WindowText, color);
    pWidget->setPalette(palette);
}

void populateComboBoxWithKnownPaths(QComboBox* pComboBox){
    if ( !pComboBox ){
        return;
    }

#if defined(Q_OS_ANDROID)
    QStringList knownPaths;
    knownPaths.push_back(QString("/storage/emulated/0"));
#else
    QStringList knownPaths = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
#endif

    QStringListModel *model;
    model = new QStringListModel(pComboBox);

    model->setStringList(knownPaths);
    pComboBox->setModel(model);
}


void populateListViewWithDirectory(QListView* pListView, QDir directory)
{
    if ( !pListView ){
        return;
    }

    QStringList dirEntries = directory.entryList(QDir::NoFilter,QDir::DirsFirst);
    QStringListModel *model;

    model = new QStringListModel(pListView);

    model->setStringList(dirEntries);
    pListView->setModel(model);

    //delete oldModel;
}



CFileDialog::CFileDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CFileDialog)
{
    //set the geometry before updating ui components
    if ( parent ){
        this->setGeometry(parent->geometry());
    }

    //setup ui components
    ui->setupUi(this);

    // Populate Initial Data
    populateComboBoxWithKnownPaths(ui->cbPathList);
    populateListViewWithDirectory(ui->lvDirectory,QDir(ui->cbPathList->currentText()));

    // Setup Ui Events
    ui->lvDirectory->connect(ui->lvDirectory, &QListView::clicked, [this](const QModelIndex index){
        if ( !index.isValid() ){
            return;
        }

        m_filePath = "";
        m_fileName = "";

        QFileInfo fileInfo = QFileInfo(QDir(ui->cbPathList->currentText()), index.data().toString());

        if ( !fileInfo.exists() ){
            return;
        }

        // Change the directory
        if ( fileInfo.isDir() ){
            QDir newDir = QDir(fileInfo.canonicalFilePath());
            ui->cbPathList->setCurrentText(newDir.canonicalPath());
            populateListViewWithDirectory(ui->lvDirectory,newDir);
        } else { // Actual file data, record the data
            m_filePath = fileInfo.canonicalFilePath();
            m_fileName = fileInfo.fileName();
            m_directory = fileInfo.dir().canonicalPath();
        }
    });

    ui->cbPathList->connect(ui->cbPathList, &QComboBox::currentTextChanged, [this](){
        if ( ui->cbPathList->currentText() == "" ){
            return;
        }

        //validate is an existing path
        if ( QDir(ui->cbPathList->currentText()).exists() ){
            changeWidgetFontColor(ui->cbPathList, Qt::black);
            this->m_directory = ui->cbPathList->currentText();
            // draw now just in case retrieving directory data takes too long
            ui->cbPathList->update();
            // get the directory data
            populateListViewWithDirectory(ui->lvDirectory, this->m_directory);
        } else { // is not a valid path so gray out the text
            changeWidgetFontColor(ui->cbPathList, Qt::gray);
        }
    });

    // Setup Ui Buttons
    ui->btnCancel->connect(ui->btnCancel, &QPushButton::clicked, [this](){
        this->close();
    });

    ui->btnOkay->connect(ui->btnOkay, &QPushButton::clicked, [this](){
        if ( m_fileName == "" || m_filePath == "" ){
            this->reject();
        }

        this->accept();
    });

}


QString CFileDialog::getFilePath()
{
    return m_filePath;
}

QString CFileDialog::getFileName()
{
    return m_fileName;
}

QString CFileDialog::getFileDirectory()
{
    return m_directory;
}


CFileDialog::~CFileDialog()
{
    delete ui;
}
